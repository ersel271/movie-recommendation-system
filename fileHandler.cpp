#include "FileHandler.h"
#include "DataHash2D.h"
#include "ThreadHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>

//TODO: Modify the class to read and print from the desired file type with a single read and a single print method by adding a parameter to the methods.

DataHash2D FileHandler::readFromCSV(const std::string& fileName) {
    DataHash2D matrix;
    std::ifstream trainfile(fileName);
    if (!trainfile.is_open()) {
        std::cerr << "err: could-not-open-file-''" << fileName << "''\n";
        return matrix;
    }
    std::string line;
    bool firstLine = true;
    while (std::getline(trainfile, line)) {
        if (firstLine) { firstLine = false; continue; } //Do not consider the first line.

        //Seperate the line w/commas.
        std::stringstream ss(line);
        std::string userIdStr, itemIdStr, ratingStr;
        if (std::getline(ss, userIdStr, ',') &&
            std::getline(ss, itemIdStr, ',') &&
            std::getline(ss, ratingStr, ',')) {
            try {
                int userId = std::stoi(userIdStr);
                int itemId = std::stoi(itemIdStr);
                float rating = std::stof(ratingStr);

                matrix.addRating(itemId, userId, rating);
            } catch (const std::exception& e) {
                std::cerr << "err: failed-to-parse-line:-" << line << ". (" << e.what() << ")\n";
            }
        } else {
            std::cerr << "err: invalid-line-format: " << line << ".\n";
        }
    }
    trainfile.close();
    return matrix;
}

DataHash2D FileHandler::readFromTXT(const std::string& fileName) {
    DataHash2D matrix;
    std::ifstream trainfile(fileName);
    if (!trainfile.is_open()) {
        std::cerr << "err: could-not-open-file-''" << fileName << "''\n";
        return matrix;
    }
    std::string line;
    bool firstLine = true;
    while (std::getline(trainfile, line)) {
        if (firstLine) { firstLine = false; continue; } //Do not consider the first line.
        std::stringstream ss(line);
        std::string userIdStr, movieIdStr, ratingStr;

		//Seperate the line w/spaces.
        if (!(std::getline(ss, userIdStr, ' ') &&
              std::getline(ss, movieIdStr, ' ') &&
              std::getline(ss, ratingStr, ' '))) {
            std::cerr << "err: invalid-line-format: " << line << ".\n";
            continue;
        }

        try {
            int userId = std::stoi(userIdStr);
            int movieId = std::stoi(movieIdStr);
            float rating = std::stof(ratingStr);
            matrix.addRating(movieId, userId, rating);
            
        } catch (const std::exception& e) {
            std::cerr << "err: failed-to-parse-line:-" << line << ". (" << e.what() << ")\n";
        }
    }
    trainfile.close();
    return matrix;
}

void FileHandler::printToCSV(const DataHash2D& data, const std::string& fileName) {
    std::ofstream outfile(fileName);
    if (!outfile.is_open()) {
        std::cerr << "err: could-not-open-file-for-writing-''" << fileName << "''\n";
        return;
    }
    std::vector<int> allMovies = data.getAllMovies();
    for (const auto& movieId : allMovies) {
        std::unordered_map<int, float> ratings = data.getMovieRatings(movieId);
        for (auto i = ratings.begin(); i != ratings.end(); i++) { //i: Iterator.
            int userId = i->first;
            float rating = i->second;
            outfile << userId << "," << movieId << "," << rating << "\n";
        }
    }
    outfile.close();
}

void FileHandler::printToTXT(const DataHash2D& data, const std::string& fileName) {
    std::ofstream outfile(fileName);
    if (!outfile.is_open()) {
        std::cerr << "err: could-not-open-file-for-writing-''" << fileName << "''\n";;
        return;
    }
    std::vector<int> allMovies = data.getAllMovies();
    for (const auto& movieId : allMovies) {
        std::unordered_map<int, float> ratings = data.getMovieRatings(movieId);
        for (auto i = ratings.begin(); i != ratings.end(); i++) { //i: Iterator.
            int userId = i->first;
            float rating = i->second;
            outfile << userId << " " << movieId << " " << rating << "\n";
        }
    }
    outfile.close();
}
