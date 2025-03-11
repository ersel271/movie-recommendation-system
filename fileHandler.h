#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "DataHash2D.h"
#include "ThreadHandler.h"

#include <string>

class FileHandler {
public:
	//Read methods: Reads from target file type.
    DataHash2D readFromCSV(const std::string& fileName);
    DataHash2D readFromTXT(const std::string& fileName);
    
    //Write methods: Writes to target file type.
    void printToCSV(const DataHash2D& data, const std::string& fileName);
    void printToTXT(const DataHash2D& data, const std::string& fileName);
};

#endif // FILEHANDLER_H
