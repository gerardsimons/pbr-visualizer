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
#include "Data/SampleSet.h"
#include "Data/Histogram.h"

#include "devices/device_singleray/dataconnector.h"

class DataController {
private:
	
	std::vector<RIVDataSetListener*> dataListeners;
	void notifyFilterListeners();
	void notifyDataListeners();
	
	std::mutex mutex;
	
	//The dataset currently being used
	RIVDataSet* currentData;
	//Fresh dataset that is a candidate for becoming the current data set
	RIVDataSet* candidateData;
	
	/* Shortcut pointers for quick access */
	RIVTable* pathTable;
	RIVTable* intersectionsTable;
//
	RIVUnsignedShortRecord *rendererId = NULL;
	RIVFloatRecord* xPixels = NULL;
	RIVFloatRecord* yPixels = NULL;
	RIVFloatRecord* lensUs = NULL;
	RIVFloatRecord* lensVs = NULL;
	RIVFloatRecord* times = NULL;
	RIVFloatRecord* colorRs = NULL;
	RIVFloatRecord* colorGs = NULL;
	RIVFloatRecord* colorBs = NULL;
	RIVFloatRecord* throughputRs = NULL;
	RIVFloatRecord* throughputGs = NULL;
	RIVFloatRecord* throughputBs = NULL;
	RIVUnsignedShortRecord *depths = NULL;
	
	RIVUnsignedShortRecord* bounceNrs = NULL;
	RIVFloatRecord* xs = NULL;
	RIVFloatRecord* ys = NULL;
	RIVFloatRecord* zs = NULL;
	RIVFloatRecord* isectColorRs = NULL;
	RIVFloatRecord* isectColorGs = NULL;
	RIVFloatRecord* isectColorBs = NULL;
	RIVUnsignedShortRecord *primitiveIds = NULL;
	RIVUnsignedShortRecord *shapeIds = NULL;
	RIVUnsignedShortRecord *interactionTypes = NULL;
	RIVUnsignedShortRecord *lightIds = NULL;
	
//	RIVDataSet* dataset;
	
	RIVMultiReference* pathsToIsectRef = NULL;
	RIVSingleReference* isectsToPathsRef = NULL;
	
	const int bins = 10;
	
	/* Histograms to approximate the true distribution */
	HistogramSet<ushort,float> trueDistributions;
	
	Histogram<float> xPixelHistogram;
	Histogram<float> yPixelHistogram;
	Histogram<float> lensUHistogram;
	Histogram<float> lensVHistogram;
	Histogram<float> timeHistogram;
	Histogram<float> colorRHistogram;
	Histogram<float> colorGHistogram;
	Histogram<float> colorBHistogram;
	Histogram<float> throughputRHistogram;
	Histogram<float> throughputGHistogram;
	Histogram<float> throughputBHistogram;
	Histogram<ushort> depthsHistogram;
	
	Histogram<ushort> bounceNrsHistogram;
	Histogram<float> xsHistogram;
	Histogram<float> ysHistogram;
	Histogram<float> zsHistogram;
	Histogram<float> isectColorRHistogram;
	Histogram<float> isectColorGHistogram;
	Histogram<float> isectColorBHistogram;
	Histogram<ushort> primitiveIdsHistogram;
	Histogram<ushort> shapeIdsHistogram;
	Histogram<ushort> interactionTypesHistogram;
	Histogram<ushort> lightIdsHistogram;
	
	//The number of data points per renderer
	std::map<ushort,size_t> pathCounts;
	
	bool paused = false;
	const size_t maxPaths;
	size_t updateThrottle = 0;
	
	//Generate the datasets; create tables records etc.
	void createDataSets();
	void clearPathCounts();
	void createHistograms();
	void initDataSet(RIVDataSet* dataset);
	//Setup the shortcut pointer to the innards of the dataset, tables etc.
	void resetPointers();
public:
	RIVDataSet* Bootstrap(RIVDataSet* dataset, size_t N);
	bool IsPaused();
	//The number of renderers to expect data from and the maximum number of paths per renderer before data reduction should kick in
	DataController(const ushort renderers, const size_t maxPaths);
	RIVDataSet* GetDataSet();
	void ProcessNewPath(ushort renderer, PathData* newPath);
	//Reduce the data
	void Reduce();
};

#endif /* defined(__embree__DataController__) */
