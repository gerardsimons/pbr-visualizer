
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
    

	static RIVDataSet LoadData(std::string fileName);
};
#endif

