//
//  DataController.h
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#ifndef __embree__DataController__
#define __embree__DataController__

#include "Data/DataSet.h"

#include "devices/device_singleray/dataconnector.h"

class DataController {
private:
	
	std::vector<RIVDataSetListener*> dataListeners;
	void notifyFilterListeners();
	void notifyDataListeners();
	
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
	size_t maxPaths = 1000;
	size_t updateThrottle = 10000;
	
	//Generate the dataset; create tables records etc.
	void createDataSet();
public:
	DataController();
	RIVDataSet* GetDataSet();
	void ProcessNewPath(ushort renderer, PathData* newPath);
};

#endif /* defined(__embree__DataController__) */
