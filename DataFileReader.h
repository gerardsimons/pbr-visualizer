
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
    
    static RIVDataSet ReadAsciiData(std::string fileName);
	static RIVDataSet ReadBinaryData(std::string fileName);
};
#endif

