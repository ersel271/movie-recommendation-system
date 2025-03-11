#include "Prediction.h"
#include "DataHash2D.h"
#include "Similarity.h"
#include "ThreadHandler.h"

#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <queue>
#include <mutex>

/*
 * Used for test purposes.
 * Generates a synthetic dataset of movie ratings for a given number of users and movies. 
 * The dataset consists of randomly generated ratings, and the following logic is applied:
 * 
 * - Popular movies are identified (top 10% of movies) and have a higher chance of being rated.
 * - Each user rates a random subset of movies, with the number of ratings per user being randomly 
 *   chosen between `minRatings` and `maxRatings`.
 * - Users are grouped into three categories (high, medium, and low ratings) to simulate different rating behaviors.
 *   - Group 0 (high ratings) generally rates movies between 3.5 and 5.
 *   - Group 1 (medium ratings) generally rates movies between 2.5 and 3.5.
 *   - Group 2 (low ratings) generally rates movies between 0 and 2.5.
 * - Ratings are slightly adjusted by a random factor within a standard deviation of 1.0, ensuring realistic variation in scores.
 * 
 * The generated ratings are then added to the provided `DataHash2D` object, which stores the user-item ratings matrix.
 * 
 * @param dh The DataHash2D object to store the generated ratings.
 * @param numberOfMovies The total number of movies in the dataset.
 * @param numberOfUsers The total number of users in the dataset.
 * @param minRatings The minimum number of movies each user will rate.
 * @param maxRatings The maximum number of movies each user will rate.
 */
void createRandomDataset(DataHash2D& dh, int numberOfMovies, int numberOfUsers, int minRatings, int maxRatings) {
    size_t sum = 0;
    std::vector<int> popularMovies;	

    //Identifying popular movies (for example, in this case the top 10% as popular).
    for (int i = 1; i <= numberOfMovies / 10; i++) popularMovies.push_back(i);
    for (int user = 1; user <= numberOfUsers; user++) {
        int numRatings = rand() % (maxRatings - minRatings + 1) + minRatings;
        std::unordered_set<int> ratedMovies;

        //Review popular movies more often.
        if (rand() % 100 < 50) { //50% chance a popular movie will be added
            int popularMovie = popularMovies[rand() % popularMovies.size()];
            ratedMovies.insert(popularMovie);
        }
        while (ratedMovies.size() < numRatings) {
            int movie = rand() % numberOfMovies + 1;
            ratedMovies.insert(movie);
        }
        sum += ratedMovies.size();
        std::cout << "user-id: " << user << " total-movies: " << ratedMovies.size() << "\n";

        //Scoring based on user groups (In this case, divided into 3 groups)
        int group = user % 3; //3 Groups: High rating, medium rating and low rating.
        for (int movie : ratedMovies) {
            float baseRating;
            if (group == 0) baseRating = 4.0f; //Group 0: High rating.
            else if (group == 1) baseRating = 3.0f; //Group 1: Medium rating.
            else baseRating = 2.0f; //Group 2: Low rating.

            float rating = baseRating + static_cast<float>(rand() % 20 - 10) / 10.0f; // +- 1.0 standart devaiation.
            rating = std::min(std::max(rating, 0.0f), 5.0f); //Limit ratings from 0 to 5.
            dh.addRating(movie, user, rating); //Add rating to dataset.
        }
    }
    std::cout << "size-of-the-dataset: " << sum << "\n";
}

Prediction::Prediction(const std::string& trainData, const std::string& testData) {
    this->trainData = fileHandler.readFromTXT(trainData);
    this->testData = fileHandler.readFromTXT(testData);
}

std::vector<std::pair<int, float>> Prediction::kNN(RatingMap& similarityMatrix, int Id, int k) {
    if (similarityMatrix.find(Id) == similarityMatrix.end()) throw std::invalid_argument("err: id-not-found-in-similarity-matrix.");
    const auto& similarities = similarityMatrix.at(Id);

    std::mutex heapMutex; 
    std::priority_queue<std::pair<float, int>, 
						std::vector<std::pair<float, int>>,
						std::greater<std::pair<float, int>>> 
						globalMinHeap; //(similarity, entityId)

    ThreadHandler th;
    auto processChunk = [&](size_t start, size_t end) {
        std::priority_queue<std::pair<float, int>,
							std::vector<std::pair<float, int>>,
							std::greater<std::pair<float, int>>>
							localMinHeap;
        auto it = similarities.begin();
        std::advance(it, start);
        for (size_t i = start; i < end && it != similarities.end(); ++i, ++it) {
            int entityId = it->first;
            float similarity = it->second;
            if (similarity <= 0.0f || entityId == Id) continue; //Skip if similarity is non-positive or if the entity is itself.

            localMinHeap.emplace(similarity, entityId);
            if (localMinHeap.size() > static_cast<size_t>(k)) localMinHeap.pop();
        }
        std::lock_guard<std::mutex> lock(heapMutex);
        while (!localMinHeap.empty()) {
            globalMinHeap.push(localMinHeap.top());
            localMinHeap.pop();
            if (globalMinHeap.size() > static_cast<size_t>(k)) globalMinHeap.pop();
        }
    };
    size_t totalWork = similarities.size();
    th.runParallel(processChunk, totalWork);
    
    std::vector<std::pair<int, float>> kNearestNeighbors;
    while (!globalMinHeap.empty()) {
        std::pair<float, int> top = globalMinHeap.top();
        kNearestNeighbors.emplace_back(top.second, top.first);
        globalMinHeap.pop();
    }
    std::sort(kNearestNeighbors.begin(), kNearestNeighbors.end(), 
		[](const std::pair<float, int>& a, const std::pair<float, int>& b) 
		{ return b.second > a.second; });
    return kNearestNeighbors;
}

DataHash2D Prediction::calculateIBCF(int k) {
    DataHash2D predictions;
    Similarity sm;
    RatingMap similarityMatrix = sm.similarityMatrix(true, trainData);
    std::vector<int> users = testData.getAllUsers();

    ThreadHandler th;
    auto processUser = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; ++i) {
            int userId = users[i];
            std::unordered_map<int, float> testMovies = testData.getUserRatings(userId);

            for (const auto& testMovie : testMovies) {
                int currentMovie = testMovie.first;
                float weightedSum = 0.0f;
                float similaritySum = 0.0f;
                std::vector<std::pair<int, float>> kNearestNeighbors = kNN(similarityMatrix, currentMovie, k);

                for (const auto& neighbor : kNearestNeighbors) {
                    int neighborMovie = neighbor.first;
                    float similarity = neighbor.second;
    
                    float rating = trainData.getRating(neighborMovie, userId);
                    if (rating >= 0.0f) { 
                        weightedSum += similarity * rating;
                        similaritySum += similarity;
                    }
                }
                float predictedRating;
                if (similaritySum > 0.0f) {
                    predictedRating = weightedSum / similaritySum;
                } else {
                	predictedRating = trainData.getAverageRating(true, currentMovie);
				}
				predictions.addRating(currentMovie, userId, predictedRating);
            }
        }
    };
	//Run all the threads.
    size_t totalWork = users.size();
    th.runParallel(processUser, totalWork);
    return predictions;
}

DataHash2D Prediction::calculateUBCF(int k) {
    DataHash2D predictions;
    Similarity sm;
    RatingMap similarityMatrix = sm.similarityMatrix(false, trainData);
    std::vector<int> users = testData.getAllUsers();

    ThreadHandler th;
    auto processUser = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; ++i) {
            int userId = users[i];
            std::unordered_map<int, float> testMovies = testData.getUserRatings(userId);

            for (const auto& testMovie : testMovies) {
                int currentMovie = testMovie.first;
                float weightedSum = 0.0f;
                float similaritySum = 0.0f;
                std::vector<std::pair<int, float>> kNearestNeighbors = kNN(similarityMatrix, userId, k);

                for (const auto& neighbor : kNearestNeighbors) {
                    int neighborUser = neighbor.first;
                    float similarity = neighbor.second;

                    float rating = trainData.getRating(currentMovie, neighborUser);
                    if (rating >= 0.0f) {
                        weightedSum += similarity * rating;
                        similaritySum += similarity;
                    }
                }
                float predictedRating;
                if (similaritySum > 0.0f) {
                    predictedRating = weightedSum / similaritySum;
                } else {
                	predictedRating = trainData.getAverageRating(false, userId);
				}
				predictions.addRating(currentMovie, userId, predictedRating);
            }
        }
    };
	//Run all the threads.
    size_t totalWork = users.size();
    th.runParallel(processUser, totalWork);

    return predictions;
}

DataHash2D Prediction::runIBCF(int k) {
    DataHash2D predictions = calculateIBCF(k);
    fileHandler.printToTXT(predictions, "submission.txt");
    return predictions;
}

DataHash2D Prediction::runUBCF(int k) {
    DataHash2D predictions = calculateUBCF(k);
    fileHandler.printToTXT(predictions, "submission.txt");
    return predictions;
}

float Prediction::RMSE(const DataHash2D& predictedRatings) const {
    float totalErr = 0.0f;
    int n = 0;

    std::vector<int> movies = testData.getAllMovies();
    for (auto mi = movies.cbegin(); mi != movies.cend(); mi++) { //mi: Movie Iterator.
        int movieId = *mi;

        std::unordered_map<int, float> testRatings = testData.getMovieRatings(movieId); //For actual test data set.
        std::unordered_map<int, float> predictions = predictedRatings.getMovieRatings(movieId); //For predictions.

        //Comparison.
        for (auto uri = testRatings.cbegin(); uri != testRatings.cend(); ++uri) { //uri: User Rating Iterator.
            int userId = uri->first;
            float actualRating = uri->second;

            // Check if there is a predicted rating for the same user.
            auto pri = predictions.find(userId); //pri: Predicted Iterator.
            if (pri != predictions.end()) {
                float predictedRating = pri->second;
                float error = actualRating - predictedRating;
                totalErr += error * error;
                n++;
            }
        }
    }
    //For preventing division errors.
    if (n == 0) throw std::cerr < "err: no-predictions-found-to-calculate-RMSE.";
    return std::sqrt(totalErr / n);
}
