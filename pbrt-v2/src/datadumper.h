#ifndef DATADUMPER_H
#define DATADUMPER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "core/pbrt.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include "core/reflection.h"

//Struct wrapper for all intersection related data
typedef struct IntersectData {
	Point position; //world position
    Spectrum spectrum;
	unsigned short primitiveId;
	unsigned short shapeId;
	unsigned short lightId;
	unsigned short interactionType;
} IntersectData;

class PathData {
public:
    PathData(float imageX, float imageY, float lensU, float lensV, float timestamp, Spectrum& throughput, std::vector<IntersectData>& intersectionData) {
        this->imageX = imageX;
        this->imageY = imageY;
        this->lensU = lensU;
        this->lensV = lensV;
        this->timestamp = timestamp;
        this->throughput = throughput;
        this->intersectionData = intersectionData;
    }
    //The camera sample data
    float imageX; // Continuously sampled image positions
    float imageY;
    float lensU; // Lens parameters
    float lensV;
    float timestamp;
    Spectrum throughput;
    std::vector<IntersectData> intersectionData;
};

class DataDumper {
private:
	static ushort objectIdCounter;
    static std::vector<DataDumper> instances;
	DataDumper(const std::string& imageName, int ID);
	static std::string ascii_file_name;
	static std::string binary_file_name;
	
	static FILE* geometryFile;
	static FILE* pathASCII; //The file to write in ASCII chars to
	static FILE* pathBinary;
	static unsigned long long FileSize(FILE*);
    
    static const size_t bufferSize;
    std::vector<PathData> bufferData;
    
    static size_t pathsWritten;
    static size_t intersectionsWritten;
public:
	static void Init(const std::string&, int);
	static void Finish();
	static bool FileIsOpen();
	
	static ushort objectIdForPrimitiveId(ushort primitiveId);
	static void AddObjectPrimitiveMapping(const std::vector<ushort> primitveIDs);
    static void AddToBuffer(size_t dumperId, const PathData& pathData);
    void AddToBuffer(const PathData& pathData);
    
	void WriteBufferToFile();
};

#endif