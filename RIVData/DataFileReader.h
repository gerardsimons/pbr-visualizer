
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <string>
#include "DataSet.h"
#include "BMPImage.h"

class DataFileReader
{
private:
	DataFileReader();
public:
	~DataFileReader(void);
    
    static RIVDataSet ReadAsciiData(const std::string& fileName,const BMPImage &image, const size_t pathsLimit = -1);
	static RIVDataSet ReadBinaryData(const std::string& fileName); //TODO: not working, fails at some float bytes
    static RIVTable* ReadImageData(const BMPImage& image);
    
    static std::vector<float> ReadModelData(const std::string& fileName);
};
#endif

