#ifndef DATAHASH_2D_H
#define DATAHASH_2D_H

#include <unordered_map>
#include <iostream>
#include <vector>

using RatingMap = std::unordered_map<int, std::unordered_map<int, float>>;

class DataHash2D {
public:
	//Returns the data set.
	RatingMap getRatingMap() const;
	
	//Assings new data set to the object.
	void setRatingMap(const RatingMap& newMovieRatings);
	
    // Adds new movieId-userId-rating triplet to the data.
    void addRating(int movieId, int userId, float rating);
    
    // Returns the movieId's rating from userId.
    float getRating(int movieId, int userId) const;
    
    // Returns all the rating data for a movieId.
    std::unordered_map<int, float> getMovieRatings(int movieId) const;
    
    // Returns all the rating data for a userId.
    std::unordered_map<int, float> getUserRatings(int userId) const;
    
    /* Returns the average rating of a movie or user.
    isMovie = true: Calculates average rating of a movie.
    isMovie = false: Calculates average rating of a user. */
    float getAverageRating(bool isMovie, int id) const;
    
    // Returns a list of all unique movieIds.
    std::vector<int> getAllMovies() const;
    
    // Returns a list of all uniqu userIds.
    std::vector<int> getAllUsers() const;

    //Returns most rated "topN" movies.
    std::vector<std::pair<int, int>> getTopMovies(int topN) const;
    
    //Returns top "topN" users with most ratings.
    std::vector<std::pair<int, int>> getTopUsers(int topN) const;
    
    //Returns total number of movies.
    size_t getMovieCount() const;

    //Returns total number of users.
    size_t getUserCount() const;

    //Returns the list of movies that not rated by a spesific user.
    std::vector<int> getUnratedMoviesByUser(int userId) const;
    
    //Returns the list of users that not rated a spesific movie.
    std::vector<int> getUsersWhoDidNotRateMovie(int movieId) const;

	//Retuns the number of rows in the data set.
    size_t getDatasetSize() const;
    
    //Prints dataset to the console.
    void printDataset() const;

private:
    RatingMap movieRatings; //{ movieId => { userId => rating } } std::unordered_map<int, std::unordered_map<int, float>>;
    
    /*Counts the number of ratings of a movie or user.
    isMovie = true: Counts the number of ratings of a movie.
    isMovie = false: Counts the number of ratings of a user.*/
    std::unordered_map<int, int> countRatings(bool isMovie) const;
};

#endif //DATAHASH_2D_H
