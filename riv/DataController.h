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
	RIVDataSet<float,ushort>* currentData;
	//Fresh dataset that is a candidate for becoming the current data set
	RIVDataSet<float,ushort>* candidateData;
	
	/* Shortcut pointers for quick access */
	RIVTable<float,ushort>* currentPathTable;
	RIVTable<float,ushort>* currentIntersectionsTable;
//
	RIVShortRecord *rendererId = NULL;
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
	RIVFloatRecord* isectColorRs = NULL;
	RIVFloatRecord* isectColorGs = NULL;
	RIVFloatRecord* isectColorBs = NULL;
	RIVShortRecord *primitiveIds = NULL;
	RIVShortRecord *shapeIds = NULL;
	RIVShortRecord *interactionTypes = NULL;
	RIVShortRecord *lightIds = NULL;
	
//	RIVDataSet* dataset;
	
	RIVMultiReference* pathsToIsectRef = NULL;
	RIVSingleReference* isectsToPathsRef = NULL;
	
	const int bins = 10;
	
	/* Histograms to approximate the true distribution */
	HistogramSet<float,ushort> trueDistributions;
	
//	Histogram<float> xPixelHistogram;
//	Histogram<float> yPixelHistogram;
//	Histogram<float> lensUHistogram;
//	Histogram<float> lensVHistogram;
//	Histogram<float> timeHistogram;
//	Histogram<float> colorRHistogram;
//	Histogram<float> colorGHistogram;
//	Histogram<float> colorBHistogram;
//	Histogram<float> throughputRHistogram;
//	Histogram<float> throughputGHistogram;
//	Histogram<float> throughputBHistogram;
//	Histogram<ushort> depthsHistogram;
//	
//	Histogram<ushort> bounceNrsHistogram;
//	Histogram<float> xsHistogram;
//	Histogram<float> ysHistogram;
//	Histogram<float> zsHistogram;
//	Histogram<float> isectColorRHistogram;
//	Histogram<float> isectColorGHistogram;
//	Histogram<float> isectColorBHistogram;
//	Histogram<ushort> primitiveIdsHistogram;
//	Histogram<ushort> shapeIdsHistogram;
//	Histogram<ushort> interactionTypesHistogram;
//	Histogram<ushort> lightIdsHistogram;
	
	//The number of data points per renderer
	std::map<ushort,size_t> pathCounts;
	
	bool paused = false;
	const size_t maxPaths;
	size_t updateThrottle = 0;
	
	//Generate the datasets; create tables records and the histogramset
	void createDataStructures();
	void clearPathCounts();
	void createHistograms();
	
	void initDataSet(RIVDataSet<float,ushort> *dataset) ;
	//Setup the shortcut pointer to the innards of the dataset, tables etc.
	void resetPointers(RIVDataSet<float,ushort>* dataset);
public:
	template<typename ...Ts>
	RIVDataSet<Ts...> Bootstrap(RIVDataSet<Ts...>* dataset, size_t N) {
		
		RIVDataSet<Ts...> bootstrap = dataset->CloneStructure();
		RIVTable<float,ushort>* bootstrapPaths = bootstrap.GetTable("paths");
		RIVTable<float,ushort>* bootstrapIsects = bootstrap.GetTable("intersections");
		
		RIVTable<float,ushort>* paths = dataset->GetTable("paths");
		RIVTable<float,ushort>* intersections = dataset->GetTable("intersections");
		size_t rows = paths->NumberOfRows();
		
//		RIVReferenceChain chain;
//		paths->GetReferenceChainToTable("intersections", chain);
		
		//path records
		std::vector<RIVRecord<float>*>* floatRecords = paths->GetRecords<float>();
		std::vector<RIVRecord<float>*>* bootstrapFloatRecords = bootstrapPaths->GetRecords<float>();
		std::vector<RIVRecord<ushort>*>* shortRecords = paths->GetRecords<ushort>();
		std::vector<RIVRecord<ushort>*>* bootstrapShortRecords = bootstrapPaths->GetRecords<ushort>();
		
		//Intersection records
		std::vector<RIVRecord<float>*>* isectFloatRecords = intersections->GetRecords<float>();
		std::vector<RIVRecord<float>*>* bootstrapIsectFloatRecords = bootstrapIsects->GetRecords<float>();
		std::vector<RIVRecord<ushort>*>* isectShortRecords = intersections->GetRecords<ushort>();
		std::vector<RIVRecord<ushort>*>* bootstrapIsectShortRecords = bootstrapIsects->GetRecords<ushort>();
		
		//Choose N paths, also add the referencing rows
		for(size_t i = 0 ; i < N ; ++i) {
			size_t index = rand() % rows;
			
			//TODO: Get the tuple from the table and iterate over the tuple and then iterate over the vectors
			for(size_t i = 0 ; i < floatRecords->size() ; ++i) {
				auto bootFloat = bootstrapFloatRecords->at(i);
				auto floatRec = floatRecords->at(i);
				
				bootFloat->AddValue(floatRec->Value(index));
			}

			for(size_t i = 0 ; i < shortRecords->size() ; ++i) {
				auto bootShort = bootstrapShortRecords->at(i);
				auto shortRec = isectShortRecords->at(i);
				
				bootShort->AddValue(shortRec->Value(index));
			}
			
			RIVMultiReference* ref = static_cast<RIVMultiReference*>(paths->GetReference());
			std::pair<size_t*,ushort> refRows = ref->GetReferenceRows(index);
			
			for(size_t i = 0 ; i < isectFloatRecords->size() ; ++i) {
				auto bootFloat = bootstrapIsectFloatRecords->at(i);
				auto floatRec = isectFloatRecords->at(i);
				
				bootFloat->AddValue(floatRec->Value(index));
			}
			
			for(size_t i = 0 ; i < isectShortRecords->size() ; ++i) {
				auto bootShort = bootstrapIsectShortRecords->at(i);
				auto shortRec = isectShortRecords->at(i);
				
				bootShort->AddValue(shortRec->Value(index));
			}
		}
		bootstrap.Print(1000);
		
		return bootstrap;
	}
	bool IsPaused();
	//The number of renderers to expect data from and the maximum number of paths per renderer before data reduction should kick in
	DataController(const ushort renderers, const size_t maxPaths);
	RIVDataSet<float,ushort>* GetDataSet();
	void ProcessNewPath(ushort renderer, PathData* newPath);
	//Reduce the data
	void Reduce();
};

#endif /* defined(__embree__DataController__) */
