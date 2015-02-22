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
#include <set>

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
	
	//The datasets currently being used, this is what the views use to draw
	RIVDataSet<float,ushort>* currentData;
	
	//Fresh dataset that is a candidate for becoming the current data set
	RIVDataSet<float,ushort>* candidateData;
	
	//The scores of the bootstrap sets for the renderers
	float bestBootstrapResult = -1;
	
	/* Histograms to approximate the true distribution */
	HistogramSet<float,ushort> trueDistributions;
	
	/* Shortcut pointers for quick access */
	RIVTable<float,ushort>* currentPathTable;
	RIVTable<float,ushort>* currentIntersectionsTable;
//
//	RIVShortRecord *rendererId = NULL;
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
	RIVShortRecord *depths = NULL;
	
	RIVShortRecord* bounceNrs = NULL;
	RIVFloatRecord* xs = NULL;
	RIVFloatRecord* ys = NULL;
	RIVFloatRecord* zs = NULL;
	RIVFloatRecord* dirX = NULL;
	RIVFloatRecord* dirY = NULL;
	RIVFloatRecord* dirZ = NULL;
	RIVFloatRecord* isectColorRs = NULL;
	RIVFloatRecord* isectColorGs = NULL;
	RIVFloatRecord* isectColorBs = NULL;
	RIVShortRecord *primitiveIds = NULL;
	RIVShortRecord *shapeIds = NULL;
	RIVShortRecord *interactionTypes = NULL;
	RIVShortRecord *lightIds = NULL;
	
	RIVMultiReference* pathsToIsectRef = NULL;
	RIVSingleReference* isectsToPathsRef = NULL;
	
	const std::set<std::string> dataTables = {PATHS_TABLE,INTERSECTIONS_TABLE};
	
	const int bootstrapBeforePause = 3;
	//The number of bins we use for the histograms that keep track of the true distribution of the data
	const int bins = 10;
	int reduceRounds = 0;
	
	int lastFrame = 0;
	float acceptProbability = .5F;
	
	//The number of data points per renderer
//	size_t pathCount = 0;
	
	//The first time we will fill both the candidate and current, after which we will bootstrap, keep the best bootstrap as current and only fill up candidate
	bool firstTime = true;
	
	bool paused = false;
	
	bool delayed = false;
	clock_t startDelay;
	int delayTimerInterval = 10000;
	
	size_t maxPaths;
	size_t bootstrapRepeat;
	
	//Generate the datasets; create tables records and the histogramset
	void createDataStructures(const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, ushort nrPrimitives);
	void clearPathCounts();
	
	void initDataSet(RIVDataSet<float,ushort> *dataset,const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, ushort nrPrimitives) ;
	//Setup the shortcut pointer to the innards of the dataset, tables etc.
	void resetPointers(RIVDataSet<float,ushort>* dataset);
public:
	RIVDataSet<float,ushort>* Bootstrap(RIVDataSet<float, ushort>* dataset, const size_t N);
	HistogramSet<float,ushort>* GetTrueDistributions() {
		return &trueDistributions;
	}

	void AddMembershipDataStructures(RIVDataSet<float,ushort>* dataset);
	void SetAcceptProbability(float newProb);
	//The number of renderers to expect data from and the maximum number of paths per renderer before data reduction should kick in
	DataController(const ushort renderers, const size_t maxPaths, const size_t bootstrapRepeat, const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, size_t nrPrimitives);
	//Returns a pointer to a pointer of the dataset for renderer one
	RIVDataSet<float,ushort>** GetDataSet();
	bool ProcessNewPath(int frame, PathData* newPath);
	//Reduce the data, first dataset is the current data being used for a renderer, candidate data is the new dataset, bestBootstrap is the slot used for creating and maintaining the best bootstrap and the best bootstrap results so far...
	void Reduce();
    void Reset();
};
#endif /* defined(__embree__DataController__) */
