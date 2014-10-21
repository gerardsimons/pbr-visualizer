#ifndef DATADUMPER_H
#define DATADUMPER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>

#include <sqlite3.h>

#include "core/pbrt.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include "core/reflection.h"


//Struct wrapper for all intersection related data
typedef struct IntersectData {
	Point position; //world position
    Spectrum spectrum;
	Spectrum throughput; //What was the throughput up until this point?
	unsigned short primitiveId;
	unsigned short shapeId;
	unsigned short lightId;
	unsigned short interactionType;
} IntersectData;

class PathData {
public:
	//The camera sample data
    float imageX; // Continuously sampled image positions
    float imageY;
    float lensU; // Lens parameters
    float lensV;
    float timestamp;
    Spectrum throughput;
	Spectrum radiance; //the computed radiance for the ray, this is set at (path)integerator so it DOES NOT take into account Le values from lights
	
	//Variable sized (=< MAX_BOUNCE) data about its intersections
    std::vector<IntersectData> intersectionData;
	
    PathData(float imageX, float imageY, float lensU, float lensV, float timestamp, const Spectrum& throughput, const Spectrum& radiance, const std::vector<IntersectData>& intersectionData) {
        this->imageX = imageX;
        this->imageY = imageY;
        this->lensU = lensU;
        this->lensV = lensV;
        this->timestamp = timestamp;
        this->throughput = throughput;
		this->radiance = radiance;
        this->intersectionData = intersectionData;
    }
	
	/** Copy Constructor */
//	PathData (const PathData& o)
//	{
//		this->imageX = o.imageX;
//		this->imageY = o.imageY;
//		this->lensU = o.lensU;
//		this->lensV = o.lensV;
//		this->timestamp = o.timestamp;
//		this->throughput = o.throughput;
//		this->radiance = o.radiance;
//		this->intersectionData = o.intersectionData;
//		
//	}
	
	//Move constructor
//	PathData(PathData&& pdata) {
//		printf("PATHDATA Copy constructor");
//		//I dont know
//	}
//	
	
};

class DataDumper {
private:
	static ushort objectIdCounter;
    static std::vector<DataDumper> instances;
	DataDumper(const std::string& imageName, int ID);
	
	static std::string ascii_file_name;
	static std::string binary_file_name;
	static std::string db_file_name;
	
	static sqlite3 *db;
	
	static bool writeASCII;
	static bool writeBinary;
	static bool writeToDatabase;
	
	static FILE* pathASCII; //The file to write in ASCII chars to
	static FILE* pathBinary;
	
	static std::mutex bufferLock;
	static std::mutex fileLock;
	
	static unsigned long long FileSize(FILE*);
	static void executeSQL(const char* sql);
	
    static const size_t bufferSize;
	int bufferIndex = 0;
    static std::vector<PathData> bufferData;
    
    static size_t pathsWritten;
    static size_t intersectionsWritten;
public:
	static void Init(const std::string&, int,bool,bool,bool);
	static void Finish();	
	static bool FileIsOpen();
	
	static ushort objectIdForPrimitiveId(ushort primitiveId);
	static void AddObjectPrimitiveMapping(ushort primitveID);
//    static void AddToBuffer(size_t dumperId, const PathData& pathData);
    static void AddToBuffer(const PathData& pathData);
    
	static void WriteBufferToFile();
	static void WriteBufferToDB();
};

#endif