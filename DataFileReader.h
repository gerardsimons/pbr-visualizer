
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <string>
#include "DataSet.h"
#include "MeshModel.h"
#include "BMPImage.h"

class DataFileReader
{
private:
	DataFileReader();
public:
	~DataFileReader(void);
    
	//Data reading functions
    static RIVDataSet ReadAsciiData(const std::string& fileName,const BMPImage &image, const size_t pathsLimit = 0);
	static RIVDataSet ReadBinaryData(const std::string& fileName, const BMPImage &image, const size_t pathsLimit = 0); 
    static RIVTable* ReadImageData(const BMPImage& image);
    static MeshModel ReadModelData(const std::string& fileName);
	
	//Data processing functions
	static void AssignShapeIDsToPrimitives(RIVTable* isectTable, MeshModel& model);
};
#endif

