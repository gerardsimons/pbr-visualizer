
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

class DataFileReader
{
private:
	DataFileReader();
public:
	~DataFileReader(void);
	static void LoadData(char* fileName);
};

#endif

