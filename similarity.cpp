#include "Similarity.h"
#include "ThreadHandler.h"

#include <unordered_map>
#include <thread>
#include <mutex>
#include <cmath>

float Similarity::cosineSimilarity(const std::unordered_map<int, float>& vec1, 
                                   const std::unordered_map<int, float>& vec2) {
    float dotProduct = 0.0f, magnitude1 = 0.0f, magnitude2 = 0.0f;

    for (const auto& item1 : vec1) {
        float value1 = item1.second;
        magnitude1 += value1 * value1;

        auto i = vec2.find(item1.first); //i: Iterator
        if (i != vec2.end()) dotProduct += value1 * i->second;
    }

    for (const auto& item2 : vec2) magnitude2 += item2.second * item2.second;

    //For preventing division errors.
    if (magnitude1 == 0.0f || magnitude2 == 0.0f) return 0.0f;
    return dotProduct / (std::sqrt(magnitude1) * std::sqrt(magnitude2));
}

float Similarity::adjustedCosineSimilarity(const std::unordered_map<int, float>& vec1, 
                                           const std::unordered_map<int, float>& vec2, 
                                           float vec1Avg, float vec2Avg) {
    return 0.0f; //TODO: Actually implement this.
}

float Similarity::pearsonCorrelation(const std::unordered_map<int, float>& vec1 ,const std::unordered_map<int, float>& vec2) {
    return 0.0f; //TODO: Actually implement this.
}

float Similarity::jaccardSimilarity(const std::unordered_map<int, float>& vec1 ,const std::unordered_map<int, float>& vec2) {
    return 0.0f; //TODO: Actually implement this.
}

RatingMap Similarity::similarityMatrix(bool isMovieBased, const DataHash2D& dh) {
    std::unordered_map<int, std::unordered_map<int, float>> matrix;
	//Running DataHash2D::getAllMovies() or DataHash2D::getAllUsers() based on bool isMovieBased.
    std::vector<int> entities = isMovieBased ? dh.getAllMovies() : dh.getAllUsers();
    /*Pointer of function type for running DataHash2D::getMovieRatings() or later 
	on the program DataHash2D::getUserRatings() based on bool isMovieBased.*/
    std::unordered_map<int, float> (DataHash2D::*getRatings)(int) const = isMovieBased ? &DataHash2D::getMovieRatings : &DataHash2D::getUserRatings;

    size_t numEntities = entities.size();
    ThreadHandler th;

    auto calculateChunk = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; ++i) {
            int entity1 = entities[i];
            std::unordered_map<int, float> ratings1 = (dh.*getRatings)(entity1);

            for (size_t j = i + 1; j < numEntities; ++j) {
                int entity2 = entities[j];
                std::unordered_map<int, float> ratings2 = (dh.*getRatings)(entity2);
                float similarity = cosineSimilarity(ratings1, ratings2);

                th.lock();
                matrix[entity1][entity2] = similarity; //Upper triangular.
                matrix[entity2][entity1] = similarity; //Lower triangular.
                th.unlock();
		/*TODO: Update the programme so that only the upper triangle (or only the lower triangle) similarity 
		matrix can also be used. This can reduce the amount of memory spent on the similarity matrix by half.*/
            }
        }
    };
    //Run all the threads.
    th.runParallel(calculateChunk, numEntities);
    return matrix;
    
    //TODO: Modify the method so that different similarity metrics can be selected using a parameter.
}

void Similarity::printSimilarityMatrix(RatingMap& matrix) {
    for (const auto& row : matrix) {
        std::cout << "[ ";
        for (const auto& col : row.second) std::cout << col.second << " ";
        std::cout << "]\n";
    }
}
