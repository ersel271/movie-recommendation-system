#include "DataHash2D.h"
#include "ThreadHandler.h"

#include <unordered_set>
#include <queue>

RatingMap DataHash2D::getRatingMap() const {
	return movieRatings;
}

void DataHash2D::setRatingMap(const RatingMap& newMovieRatings) {
	movieRatings = newMovieRatings;
}

void DataHash2D::addRating(int movieId, int userId, float rating) {
	if (rating >= 0.0f && rating <= 5.0f) movieRatings[movieId][userId] = rating;
	else std::cerr << "err: rating-can-be-minimum-of-0-and-maximum-of-5-your-rating-is-" << rating << ".\n";
}

float DataHash2D::getRating(int movieId, int userId) const {
    auto mi = movieRatings.find(movieId); // mi: Movie iterator
    if (mi != movieRatings.end()) {
        auto ui = mi->second.find(userId); // ui: User iterator
        if (ui != mi->second.end()) return ui->second;
    }
    return -1.0f; // If rating is not found, return -1.
}

std::unordered_map<int, float> DataHash2D::getMovieRatings(int movieId) const {
    std::unordered_map<int, float> ratings;
    auto mi = movieRatings.find(movieId); // mi: Movie iterator
    if (mi != movieRatings.end()) ratings = mi->second;
    return ratings;
}

std::unordered_map<int, float> DataHash2D::getUserRatings(int userId) const {
    std::unordered_map<int, float> userRatings;
    for (const auto& me : movieRatings) {// me: Movie entry
        auto ui = me.second.find(userId);// ui: User iterator
        if (ui != me.second.end()) userRatings[me.first] = ui->second;
    }
    return userRatings;
}

float DataHash2D::getAverageRating(bool isMovie, int id) const {
    std::unordered_map<int, float> ratings = isMovie ? getMovieRatings(id) : getUserRatings(id);
    if (ratings.empty()) return -1.0f;
    
    float sum = 0.0f;
    for (const auto& entry : ratings) sum += entry.second;
    return sum / ratings.size();
}

std::vector<int> DataHash2D::getAllMovies() const {
    std::vector<int> movies;
    for (const auto& movie : movieRatings) movies.push_back(movie.first);
    return movies;
}

std::vector<int> DataHash2D::getAllUsers() const {
    std::unordered_set<int> users;
    for (const auto& movie : movieRatings) {
        for (const auto& rating : movie.second) users.insert(rating.first);
    }
    return std::vector<int>(users.begin(), users.end());
}

std::unordered_map<int, int> DataHash2D::countRatings(bool isMovie) const {
    std::unordered_map<int, int> counts;

    std::vector<int> entities = isMovie ? getAllMovies() : getAllUsers();
    size_t numEntities = entities.size();

    ThreadHandler threadHandler;
    auto calculateChunk = [&](size_t start, size_t end) {
        std::unordered_map<int, int> localCounts;
        
        for (size_t i = start; i < end; ++i) {
            int entityId = entities[i];
            if (isMovie) localCounts[entityId] = movieRatings.at(entityId).size();
            else {
                for (const auto& movie : movieRatings) if (movie.second.count(entityId)) localCounts[entityId]++;
            }
        }

        threadHandler.lock();
        for (const auto& entry : localCounts) counts[entry.first] += entry.second;
        threadHandler.unlock();
    };
    //Run all the threads.
    threadHandler.runParallel(calculateChunk, numEntities);
    return counts;
}

std::vector<std::pair<int, int>> DataHash2D::getTopMovies(int topN) const {
    auto movieCounts = countRatings(true);

    std::priority_queue<std::pair<int, int>> pq;
    for (const auto& movieCount : movieCounts) {
        pq.emplace(movieCount.second, movieCount.first); //(count, movieId)
    }

    std::vector<std::pair<int, int>> topMovies;
    for (int i = 0; i < topN && !pq.empty(); ++i) {
        topMovies.push_back(pq.top());
        pq.pop();
    }
    return topMovies;
}

std::vector<std::pair<int, int>> DataHash2D::getTopUsers(int topN) const {
    auto userCounts = countRatings(false);

    std::priority_queue<std::pair<int, int>> pq;
    for (const auto& userCount : userCounts) {
        pq.emplace(userCount.second, userCount.first); //(count, userId)
    }

    std::vector<std::pair<int, int>> topUsers;
    for (int i = 0; i < topN && !pq.empty(); ++i) {
        topUsers.push_back(pq.top());
        pq.pop();
    }
    return topUsers;
}

size_t DataHash2D::getMovieCount() const {
    return this->movieRatings.size();
}

size_t DataHash2D::getUserCount() const {
    return this->getAllUsers().size();
}

std::vector<int> DataHash2D::getUnratedMoviesByUser(int userId) const {
    std::vector<int> unratedMovies;
    std::unordered_map<int, float> userRatings = getUserRatings(userId);

    for (const auto& me : movieRatings) { //me: Movie Entry.
        int movieId = me.first;
        //If movie not rated by the user.
        if (userRatings.find(movieId) == userRatings.end()) unratedMovies.push_back(movieId);
    }
    return unratedMovies;
}

std::vector<int> DataHash2D::getUsersWhoDidNotRateMovie(int movieId) const {
    std::vector<int> usersWhoDidNotRate;
    std::unordered_map<int, float> movieRatingsForMovie = getMovieRatings(movieId);  // Filmi oylayan kullan�c�lar

    for (const auto& ue : getAllUsers()) { //ue: User Entry.
    	//If user has no rating on this movie.
        if (movieRatingsForMovie.find(ue) == movieRatingsForMovie.end()) usersWhoDidNotRate.push_back(ue);
    }
    return usersWhoDidNotRate;
}

size_t DataHash2D::getDatasetSize() const {
    size_t totalSize = 0;
    //me: Movie Entry.
    for (const auto& me : movieRatings) totalSize += me.second.size();
    return totalSize;
}

void DataHash2D::printDataset() const {
    for (const auto& me : movieRatings) { //me: Movie entry
        std::cout << "[ ";
        //ue: User entry
        for (const auto& ue : me.second) std::cout << ue.second << " ";
        std::cout << "]\n";
    }
}
