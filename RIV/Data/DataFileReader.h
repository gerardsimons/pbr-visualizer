
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <string>
#include "DataSet.h"
#include "../PBRTConfig.h"
#include "../Geometry/MeshModel.h"
#include "../Geometry/TriangleMesh.h"
#include "../Graphics/BMPImage.h"
#include "Database.h"

class DataFileReader
{
private:
	DataFileReader();
public:
	~DataFileReader(void);
    
	//Data reading functions
    static RIVDataSet ReadAsciiData(const std::string& fileName,BMPImage *image, const size_t pathsLimit = 0);
	static RIVDataSet ReadBinaryData(const std::string& fileName, BMPImage *image, const size_t pathsLimit = 0);
	static sqlite::Database ReadBinaryDataToDB(const std::string& fileName, const size_t pathsLimit = 0, bool inMemory = true);
    static RIVTable* ReadImageData(const BMPImage& image);
    static PBRTConfig ReadPBRTFile(const std::string& fileName);
	
	//Data processing functions
//	static void AssignShapeIDsToPrimitives(RIVTable* isectTable, MeshModel& model);
};

#endif

