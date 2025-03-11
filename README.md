# Movie Recommendation System

## Overview

This project implements a **Movie Recommendation System** using **Collaborative Filtering** techniques, specifically **k-Nearest Neighbors (k-NN)**. It calculates similarities between users or movies using similarity metrics like **Cosine Similarity** (currently implemented), with future support for **Adjusted Cosine**, **Pearson Correlation**, and **Jaccard Similarity**.

The system is designed to be **scalable and efficient**, utilizing **multithreading** to speed up similarity calculations. It can also generate synthetic datasets for testing purposes, where ratings are assigned to movies by users grouped into high, medium, and low rating categories.

The project has been developed following **Object-Oriented Programming (OOP)** principles, ensuring a **modular architecture** that promotes maintainability and scalability. Additionally, the system relies solely on **standard C++ libraries**, without the use of any external libraries or dependencies.

### Key Features:
- **Collaborative Filtering**: Supports both **user-based** and **item-based** collaborative filtering.
- **Similarity Measures**: **Cosine Similarity** is implemented, with future support for **Adjusted Cosine**, **Pearson**, and **Jaccard Similarity**.
- **Multithreading**: Parallel processing to calculate the similarity matrix efficiently.
- **Synthetic Dataset Generation**: Random dataset creation with controlled user rating distributions.

## Files

- **DataHash2D.cpp:** Handles the storage and manipulation of the rating matrix (user-movie ratings).
- **FileHandler.cpp:** Functions for reading and writing data from/to CSV and TXT files.
- **Prediction.cpp:** Implements the prediction algorithm and calculates recommendation scores.
- **Similarity.cpp:** Contains methods for calculating various similarity measures between users or movies.
- **ThreadHandler.cpp:** Manages multithreading for parallel processing.

### **DataHash2D**

The `DataHash2D` class represents the movie rating dataset, stored as a 2D hash map, where each movie ID is associated with a map of user IDs and their respective ratings.

#### **Methods**:
- **`getRatingMap()`**: 
  - Returns the entire dataset of movie ratings as a nested hash map, where the outer map's key is the movie ID and the value is another map with user IDs as keys and ratings as values.
  
- **`setRatingMap(const RatingMap& newMovieRatings)`**: 
  - Sets a new dataset (rating map) to the `DataHash2D` object, overwriting the existing dataset.

- **`addRating(int movieId, int userId, float rating)`**: 
  - Adds a new rating for a specific movie by a specific user. The rating is validated to be between 0.0 and 5.0.

- **`getRating(int movieId, int userId) const`**: 
  - Returns the rating for a given movie by a specific user. Returns `-1.0f` if the rating is not found.

- **`getMovieRatings(int movieId) const`**: 
  - Returns all ratings for a specific movie as a map, where the key is the user ID and the value is the rating.

- **`getUserRatings(int userId) const`**: 
  - Returns all ratings for a specific user as a map, where the key is the movie ID and the value is the rating.

- **`getAverageRating(bool isMovie, int id) const`**: 
  - Returns the average rating for either a movie or a user. 
    - If `isMovie` is `true`, it calculates the average rating for a movie.
    - If `isMovie` is `false`, it calculates the average rating for a user.

- **`getAllMovies() const`**: 
  - Returns a list of all unique movie IDs in the dataset.

- **`getAllUsers() const`**: 
  - Returns a list of all unique user IDs in the dataset.

- **`getTopMovies(int topN) const`**: 
  - Returns the top `N` most-rated movies, sorted by the number of ratings.

- **`getTopUsers(int topN) const`**: 
  - Returns the top `N` most active users who have rated the most movies.

- **`getMovieCount() const`**: 
  - Returns the total number of movies in the dataset.

- **`getUserCount() const`**: 
  - Returns the total number of users in the dataset.

- **`getUnratedMoviesByUser(int userId) const`**: 
  - Returns a list of movies that a specific user has not rated.

- **`getUsersWhoDidNotRateMovie(int movieId) const`**: 
  - Returns a list of users who have not rated a specific movie.

- **`getDatasetSize() const`**: 
  - Returns the total number of ratings in the dataset.

- **`printDataset() const`**: 
  - Prints the entire dataset to the console.

## Similarity Measures

Currently implemented similarity measure:
- **Cosine Similarity**: Measures the cosine of the angle between two vectors, indicating their similarity in terms of direction and magnitude.

### Planned for Future Implementation:
- **Adjusted Cosine Similarity**: Adjusts cosine similarity by accounting for user biases (average ratings).
- **Pearson Correlation**: Measures the linear correlation between two sets of ratings.
- **Jaccard Similarity**: Measures the similarity between two sets based on the ratio of their intersection to their union.

## Dataset Format

The dataset consists of a list of ratings in the following format:
- **CSV Format**: Each line represents a rating entry with `userId`, `movieId`, and `rating`. The columns are separated by commas.
  Example:

- **TXT Format**: Each line represents a rating entry with `userId`, `movieId`, and `rating`, separated by spaces.
Example:

The project includes three dataset files:
- **`public_training_data.txt`**: Used for training the model.
- **`public_test_data.txt`**: Used for testing the model.
- **`training_data.csv`**: A larger dataset in CSV format can also be used.

## Prerequisites
Ensure you have the following:
- C++ compiler with support for C++11 or higher.
- C++ Standard Library.
