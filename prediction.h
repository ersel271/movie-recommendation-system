#ifndef PREDICTION_H
#define PREDICTION_H

#include "DataHash2D.h"
#include "FileHandler.h"

#include <string>

class Prediction {
public:
	//Constructor.
    Prediction(const std::string& trainFile, const std::string& testFile);
       
    //Runs the IBCF method for NBCF.
    DataHash2D runIBCF(int k);
    
    //Runs the UBCF method for NBCF.
    DataHash2D runUBCF(int k);
    
	//Calculates the Root Mean Square Error between given dataset and this->testData.
    float RMSE(const DataHash2D& predictedRatings) const;
    
private:
	//Returns the most similar k user or movie to a given user or movie among with the similarity values.
	std::vector<std::pair<int, float>> kNN(RatingMap& similarityMatrix, int Id, int k);
	
	//Calculates the UBCF for this->testData respect to the k-Nearest Neighbors.
    DataHash2D calculateUBCF(int k);
    
    //Calculates the IBCF for this->testData respect to the k-Nearest Neighbors.
    DataHash2D calculateIBCF(int k);
    
    FileHandler fileHandler; //Instance of fileHandler for file read/write operations.
    DataHash2D trainData;    //Training dataset.
    DataHash2D testData;	 //Test dataset.
};

#endif // PREDICTION_H
