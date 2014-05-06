
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <string>

class DataFileReader
{
private:
	DataFileReader();
public:
	~DataFileReader(void);
	static void LoadData(std::string fileName);
};

#endif

