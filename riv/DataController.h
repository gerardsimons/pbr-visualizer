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

#include "Octree/Octree.h"
#include "Data/DataSet.h"
#include "Data/SampleSet.h"
#include "Data/Histogram.h"

#include "devices/device_singleray/dataconnector.h"

class DataController {
private:
    
    ushort imageWidth;
    ushort imageHeight;
	std::vector<RIVDataSetListener*> dataListeners;
	void notifyFilterListeners();
	void notifyDataListeners();
	
    Octree* energyDistribution3D = NULL;
    
    Histogram2DSet<float,ushort> imageDistributions;
//    Histogram2D<float> pixelThroughput;
//    Histogram2D<float> energyDistribution2D;
	
	//The datasets currently being used, this is what the views use to draw
	RIVDataSet<float,ushort>* currentData;
	
	//Fresh dataset that is a candidate for becoming the current data set
	RIVDataSet<float,ushort>* candidateData;
	
	//The scores of the bootstrap sets for the renderers
	float bestBootstrapResult = -1;
	
	/* 1D Histograms to approximate the true distribution of each data dimension */
	HistogramSet<float,ushort> trueDistributions;
	
	/* Shortcut pointers for quick access */
	RIVTable<float,ushort>* currentPathTable;
	RIVTable<float,ushort>* currentIntersectionsTable;
    RIVTable<float,ushort>* currentLightsTable;

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
	RIVShortRecord* primitiveIds = NULL;
	RIVShortRecord* shapeIds = NULL;
	RIVShortRecord* interactionTypes = NULL;
    RIVShortRecord* occluderCounts = NULL;
    
    RIVShortRecord* lightIds = NULL;
    RIVShortRecord* occluderIds = NULL;
    RIVFloatRecord* lightRs = NULL;
    RIVFloatRecord* lightGs = NULL;
    RIVFloatRecord* lightBs = NULL;
	
	RIVMultiReference* pathsToIsectRef = NULL;
	RIVSingleReference* isectsToPathsRef = NULL;
    RIVFixedReference* isectsToLightsRef = NULL;
    RIVSingleReference* lightsToIsectsRef = NULL;
	
    const std::set<std::string> histogramTables = {PATHS_TABLE,INTERSECTIONS_TABLE};
	const std::set<std::string> dataTables = {PATHS_TABLE,INTERSECTIONS_TABLE,LIGHTS_TABLE};
	
	const int bootstrapBeforePause = 3;
	//The number of bins we use for the histograms that keep track of the true distribution of the data
	const int bins = 10;
	int reduceRounds = 0;
    ushort maxNrLights = 0;
	int lastFrame = 0;
	float acceptProbability = .5F;
	
	//The number of data points per renderer
//	size_t pathCount = 0;
	
	//The first time we will fill both the candidate and current, after which we will bootstrap, keep the best bootstrap as current and only fill up candidate
	bool firstTime = true;
	bool paused = false;
	
    ushort maxDepth = 5;
	int maxPaths;
	int maxBootstrapRepeat;
    int bootstrapRepeat;
	
	//Generate the datasets; create tables records and the histogramset
	void createDataStructures(const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, ushort nrPrimitives);
	void clearPathCounts();
	
	void initDataSet(RIVDataSet<float,ushort> *dataset,const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, ushort nrPrimitives) ;
	//Setup the shortcut pointer to the innards of the dataset, tables etc.
	void resetPointers(RIVDataSet<float,ushort>* dataset);
public:
    enum DataCollectionMode {
        ALL,
        DISTRIBUTIONS,
        NONE
    };
    enum AcceptMode {
        UNIFORM,
        RADIANCE
    };
    
    DataCollectionMode collectionMode = ALL;
    AcceptMode acceptMode = UNIFORM;

    /* METHODS */
    
	RIVDataSet<float,ushort>* Bootstrap(RIVDataSet<float, ushort>* dataset, const size_t N);
	HistogramSet<float,ushort>* GetTrueDistributions() {
		return &trueDistributions;
	}
    Histogram2DSet<float,ushort>* GetImageDistributions() {
        return &imageDistributions;
    }
    
//    Histogram2D<float>* GetPixelThroughputDistribution();
    Octree* GetEnergyDistribution3D();
    Histogram2D<float>* GetEnergyDistribution2D();
	void AddMembershipDataStructures(RIVDataSet<float,ushort>* dataset);
	void SetAcceptProbability(float newProb);
	//The number of renderers to expect data from and the maximum number of paths per renderer before data reduction should kick in
	DataController(const int maxPaths, const int bootstrapRepeat, const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, size_t nrPrimitives, ushort nrLights, ushort imageWidth, ushort imageHeight,ushort maxDepth);
	//Returns a pointer to a pointer of the dataset for renderer one
	RIVDataSet<float,ushort>** GetDataSet();
	bool ProcessNewPath(int frame, PathData* newPath);
	//Reduce the data, first dataset is the current data being used for a renderer, candidate data is the new dataset, bestBootstrap is the slot used for creating and maintaining the best bootstrap and the best bootstrap results so far...
	void Reduce();
    void Reset();
    void SetMaxPaths(int maxPaths);
    void SetDataCollectionMode(DataCollectionMode newMode);
    void CycleDataCollectionMode();
    void SetDataAcceptMode(AcceptMode newMode);
    void CycleDataAcceptMode();
};
#endif /* defined(__embree__DataController__) */
