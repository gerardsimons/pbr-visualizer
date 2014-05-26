
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <string>
#include "DataSet.h"

class DataFileReader
{
private:
	DataFileReader();
public:
	~DataFileReader(void);
    
    static RIVDataSet ReadAsciiData(std::string fileName,size_t pathsLimit = -1);
	static RIVDataSet ReadBinaryData(std::string fileName); //TODO: not working, fails at some float bytes
    
    static std::vector<float> ReadModelData(std::string fileName);
};
#endif

