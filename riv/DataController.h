//
//  DataController.h
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#ifndef __embree__DataController__
#define __embree__DataController__

#include <mutex>

#include "Data/DataSet.h"
#include "devices/device_singleray/dataconnector.h"

class DataController {
private:
	
	std::vector<RIVDataSetListener*> dataListeners;
	void notifyFilterListeners();
	void notifyDataListeners();
	
	std::mutex mutex;
	
	/* Shortcut pointers for quick access */
	RIVTable* pathTable;
	RIVTable* isectsTable;
	
	RIVUnsignedShortRecord *rendererId;
	RIVFloatRecord* xPixels;
	RIVFloatRecord* yPixels;
	RIVFloatRecord* lensUs;
	RIVFloatRecord* lensVs;
	RIVFloatRecord* times;
	RIVFloatRecord* colorRs;
	RIVFloatRecord* colorGs;
	RIVFloatRecord* colorBs;
	RIVFloatRecord* throughputRs;
	RIVFloatRecord* throughputGs;
	RIVFloatRecord* throughputBs;
	RIVUnsignedShortRecord *depths;
	
	RIVUnsignedShortRecord* bounceNrs;
	RIVFloatRecord* xs;
	RIVFloatRecord* ys;
	RIVFloatRecord* zs;
	RIVFloatRecord* isectColorRs;
	RIVFloatRecord* isectColorGs;
	RIVFloatRecord* isectColorBs;
	RIVUnsignedShortRecord *primitiveIds;
	RIVUnsignedShortRecord *shapeIds;
	RIVUnsignedShortRecord *interactionTypes;
	RIVUnsignedShortRecord *lightIds;
	
	RIVDataSet* dataset;
	
	RIVMultiReference* pathsToIsectRef;
	RIVSingleReference* isectToPathsRef;
	
	//The number of data points per renderer
	std::map<ushort,size_t> pathCounts;
	
	bool paused = false;
	const size_t maxPaths;
	size_t updateThrottle = 0;
	
	//Generate the dataset; create tables records etc.
	void createDataSet();
	void clearPathCounts();
public:
	bool IsPaused();
	//The number of renderers to expect data from and the maximum number of paths per renderer before data reduction should kick in
	DataController(const ushort renderers, const size_t maxPaths);
	RIVDataSet* GetDataSet();
	void ProcessNewPath(ushort renderer, PathData* newPath);
	//Reduce the data
	void Reduce();
};

#endif /* defined(__embree__DataController__) */
