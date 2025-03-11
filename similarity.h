#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "DataHash2D.h"

#include <unordered_map>

class Similarity {
public:
    // Returns cosine similarity between two given vectors.
    float cosineSimilarity(const std::unordered_map<int, float>& vec1, const std::unordered_map<int, float>& vec2);

    // Returns adjusted cosine similarity between two given vectors. - Not implemented for now.
    float adjustedCosineSimilarity(const std::unordered_map<int, float>& vec1, 
                                   const std::unordered_map<int, float>& vec2, 
                                   float vec1Avg, float vec2Avg);
    
	// Returns Pearson Correlation Coefficient between two given vectors. - Not implemented for now.                             
    float pearsonCorrelation(const std::unordered_map<int, float>& vec1 ,const std::unordered_map<int, float>& vec2);
    
    // Returns Pearson Correlation Coefficient between two given vectors. - Not implemented for now.
    float jaccardSimilarity(const std::unordered_map<int, float>& vec1 ,const std::unordered_map<int, float>& vec2);
    
    /* Creates a similarity matrix for given dataset.
    isMovieBased = true: Generates similarity matrix of movies.
    isMovieBased = false: Generates similarity matrix of users. */          
    RatingMap similarityMatrix(bool isMovieBased, const DataHash2D& dh);
    
    // Prints the similarity matrix created from Similarity::similarityMatrix
    void printSimilarityMatrix(RatingMap& matrix);

};

#endif // SIMILARITY_H

