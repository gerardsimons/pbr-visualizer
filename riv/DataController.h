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
	
	bool paused = false;
	size_t maxPaths = 100000;
	size_t updateThrottle = 0;
	
	//Generate the dataset; create tables records etc.
	void createDataSet();
public:
	bool IsPaused();
	DataController();
	RIVDataSet* GetDataSet();
	void ProcessNewPath(ushort renderer, PathData* newPath);
};

#endif /* defined(__embree__DataController__) */
