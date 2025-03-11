#include "DataHash2D.h"
#include "Prediction.h"

#include <string>
#include <chrono>

using namespace std;
int main() {
	std::cout << "process-started..." << std::endl;
	auto timerStart = std::chrono::high_resolution_clock::now(); //Timer: Start.

	int kNearestNeighbors = 27;
	string trainData = "datasets/public_training_data.txt"; //public_training_data.txt => The data set that used for the training purposes.
	string testData = "datasets/public_test_data.txt"; //public_test_data.txt => Test data set that needs to be predicted.

	Prediction prediction(trainData, testData); //Create an instance of Prediction class with training and test datasets.
    DataHash2D predictions = prediction.runUBCF(kNearestNeighbors); //Run the IBCF method using kNearestNeighbors parameter.
    
    float rmse = prediction.RMSE(predictions); //Root Mean Square Error (RMSE) calculation.
    std::cout << "\n*rmse: " << rmse << std::endl;

    auto timerEnd = std::chrono::high_resolution_clock::now(); //Timer: End.
    std::chrono::duration<double> runTime = timerEnd - timerStart;
    std::cout << "*runtime: " << runTime.count() << " seconds\n\n";

	std::cout << "process-ended...\nresults-are-saved-to-submission.txt" << std::endl;
    return 0;
}
