//
//  DataController.cpp
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#include "DataController.h"
#include <algorithm>

DataController::DataController(const ushort renderers, const size_t maxPaths) : maxPaths(maxPaths) {
	if(updateThrottle == 0) {
		updateThrottle = maxPaths;
	}
	createDataSets();
	
	for(ushort i = 0 ; i < renderers ; ++i) {
		pathCounts[i] = 0;
	}
	
	createHistograms();
}

void DataController::createHistograms() {
	
	xPixelHistogram = Histogram<float>(0,1,bins);
	yPixelHistogram = Histogram<float>(0,1,bins);
	lensUHistogram = Histogram<float>(0,1,bins);
	lensVHistogram = Histogram<float>(0,1,bins);
	colorRHistogram = Histogram<float>(0,1,bins);
	colorGHistogram = Histogram<float>(0,1,bins);
	colorBHistogram = Histogram<float>(0,1,bins);;
	throughputRHistogram = Histogram<float>(0,1,bins);;
	throughputGHistogram = Histogram<float>(0,1,bins);;
	throughputBHistogram = Histogram<float>(0,1,bins);;
	depthsHistogram = Histogram<ushort>(0,5);
	
	bounceNrsHistogram = Histogram<ushort>(0,5);
	xsHistogram = Histogram<float>(0,500,bins);
	ysHistogram = Histogram<float>(0,500,bins);
	zsHistogram = Histogram<float>(0,500,bins);
	isectColorRHistogram = Histogram<float>(0,1,bins);
	isectColorGHistogram = Histogram<float>(0,1,bins);
	isectColorBHistogram = Histogram<float>(0,1,bins);;
	primitiveIdsHistogram = Histogram<ushort>(0,6);
	
//	shapeIdsHistogram = Histogram<ushort>(0,1);
//	interactionTypesHistogram = Histogram<ushort>(0,1,bins);;
//	lightIdsHistogram = Histogram<ushort>(0,5,5);
}

void DataController::initDataSet(RIVDataSet *dataset) {
	
	RIVTable* pathTable = dataset->CreateTable("paths");
	
	pathTable->CreateShortRecord("renderer");
	pathTable->CreateFloatRecord("pixel x");
	pathTable->CreateFloatRecord("pixel y");
	//	lensUs = pathTable->CreateFloatRecord("lens U");
	//	lensVs = pathTable->CreateFloatRecord("lens V");
	//	times = pathTable->CreateFloatRecord("time");
	pathTable->CreateFloatRecord("R");
	pathTable->CreateFloatRecord("G");
	pathTable->CreateFloatRecord("B");
	pathTable->CreateFloatRecord("throughput R");
	pathTable->CreateFloatRecord("throughput G");
	pathTable->CreateFloatRecord("throughput B");
	pathTable->CreateShortRecord("depth");
	
	RIVTable* isectsTable = dataset->CreateTable("intersections");
	
	isectsTable->CreateShortRecord("bounce_nr");
	isectsTable->CreateFloatRecord("x");
	isectsTable->CreateFloatRecord("y");
	isectsTable->CreateFloatRecord("z");
	isectsTable->CreateFloatRecord("R");
	isectsTable->CreateFloatRecord("G");
	isectsTable->CreateFloatRecord("B");
	isectsTable->CreateShortRecord("primitive ID");
	//	shapeIds = isectsTable->CreateShortRecord("shape ID");
	//	interactionTypes = isectsTable->CreateShortRecord("interaction");
	//	lightIds = isectsTable->CreateShortRecord("light ID");
	
	RIVMultiReference* pathsToIsectRef = new RIVMultiReference(pathTable,isectsTable);
	pathTable->AddReference(pathsToIsectRef);
	RIVSingleReference* isectToPathsRef = new RIVSingleReference(isectsTable,pathTable);
	isectsTable->AddReference(isectToPathsRef);
}

void DataController::createDataSets() {
	
	currentData = new RIVDataSet();
	candidateData = new RIVDataSet();
	
	initDataSet(currentData);
	initDataSet(candidateData);
	
	resetPointers();
}

void DataController::resetPointers() {
	pathTable = candidateData->GetTable("paths");
	intersectionsTable = candidateData->GetTable("intersections");
	
	isectsToPathsRef = (RIVSingleReference*)intersectionsTable->GetReference(0);
	pathsToIsectRef = (RIVMultiReference*)pathTable->GetReference(0);
	
	rendererId = pathTable->GetRecord<RIVUnsignedShortRecord>("renderer");
	xPixels = pathTable->GetRecord<RIVFloatRecord>("pixel x");
	yPixels = pathTable->GetRecord<RIVFloatRecord>("pixel y");
	//	lensUs = pathTable->GetRecord<RIVFloatRecord>("lens U");
	//	lensVs = pathTable->GetRecord<RIVFloatRecord>("lens V");
	//	times = pathTable->GetRecord<RIVFloatRecord>("time");
	colorRs = pathTable->GetRecord<RIVFloatRecord>("R");
	colorGs = pathTable->GetRecord<RIVFloatRecord>("G");
	colorBs = pathTable->GetRecord<RIVFloatRecord>("B");
	throughputRs = pathTable->GetRecord<RIVFloatRecord>("throughput R");
	throughputGs = pathTable->GetRecord<RIVFloatRecord>("throughput G");
	throughputBs = pathTable->GetRecord<RIVFloatRecord>("throughput B");
	depths = pathTable->GetRecord<RIVUnsignedShortRecord>("depth");
	
	bounceNrs = intersectionsTable->GetRecord<RIVUnsignedShortRecord>("bounce_nr");
	xs = intersectionsTable->GetRecord<RIVFloatRecord>("x");
	ys = intersectionsTable->GetRecord<RIVFloatRecord>("y");
	zs = intersectionsTable->GetRecord<RIVFloatRecord>("z");
	isectColorRs = intersectionsTable->GetRecord<RIVFloatRecord>("R");
	isectColorGs = intersectionsTable->GetRecord<RIVFloatRecord>("G");
	isectColorBs = intersectionsTable->GetRecord<RIVFloatRecord>("B");
	primitiveIds = intersectionsTable->GetRecord<RIVUnsignedShortRecord>("primitive ID");
	//	shapeIds = isectsTable->CreateShortRecord("shape ID");
	//	interactionTypes = isectsTable->CreateShortRecord("interaction");
	//	lightIds = isectsTable->CreateShortRecord("light ID");

}

RIVDataSet* DataController::GetDataSet() {
	return currentData;
}

void DataController::ProcessNewPath(ushort renderer, PathData* newPath) {
	//	printf("New path from renderer #1 received!\n");
	//	return;
	if(!paused) {
		//ALWAYS update the histograms
		ushort nrIntersections = newPath->intersectionData.size();
		xPixelHistogram.Add(newPath->imageX);
		yPixelHistogram.Add(newPath->imageY);
		lensUHistogram.Add(newPath->lensU);
		lensVHistogram.Add(newPath->lensV);
		timeHistogram.Add(newPath->timestamp);
		colorRHistogram.Add(std::min(newPath->radiance[0],1.F));
		colorGHistogram.Add(std::min(newPath->radiance[1],1.F));
		colorBHistogram.Add(std::min(newPath->radiance[2],1.F));
		throughputRHistogram.Add(newPath->throughput[0]);
		throughputGHistogram.Add(newPath->throughput[1]);
		throughputBHistogram.Add(newPath->throughput[2]);
		depthsHistogram.Add((ushort)newPath->intersectionData.size());
		for(int i = 0 ; i < nrIntersections ; ++i) {
			IntersectData& isect = newPath->intersectionData[i];
			bounceNrsHistogram.Add(i+1);
			xsHistogram.Add(isect.position[0]);
			ysHistogram.Add(isect.position[1]);
			zsHistogram.Add(isect.position[2]);
			isectColorRHistogram.Add(std::min(isect.color[0],1.F));
			isectColorGHistogram.Add(std::min(isect.color[1],1.F));
			isectColorBHistogram.Add(std::min(isect.color[2],1.F));
			primitiveIdsHistogram.Add(isect.primitiveId);
		}
		//				mutex.lock();
		//				printf("Adding new path.\n");
		bool tableFull = pathCounts[renderer] >= maxPaths;
		
		if(!tableFull) {
			++pathCounts[renderer];
			rendererId->AddValue(renderer);
			xPixels->AddValue(newPath->imageX);
			yPixels->AddValue(newPath->imageY);
			//				lensUs->AddValue(newPath->lensU);
			//				lensVs->AddValue(newPath->lensV);
			//				times->AddValue(newPath->timestamp);
			colorRs->AddValue(std::min(newPath->radiance[0],1.F));
			colorGs->AddValue(std::min(newPath->radiance[1],1.F));
			colorBs->AddValue(std::min(newPath->radiance[2],1.F));
			throughputRs->AddValue(newPath->throughput[0]);
			throughputGs->AddValue(newPath->throughput[1]);
			throughputBs->AddValue(newPath->throughput[2]);
			depths->AddValue((ushort)newPath->intersectionData.size());
			size_t* indices = new size_t[nrIntersections];
			
			pathsToIsectRef->AddReferences(rendererId->Size() - 1, std::pair<size_t*,ushort>(indices,nrIntersections));
			
			for(int i = 0 ; i < nrIntersections ; ++i) {
				IntersectData& isect = newPath->intersectionData[i];
				
				bounceNrs->AddValue(i+1);
				xs->AddValue(isect.position[0]);
				ys->AddValue(isect.position[1]);
				zs->AddValue(isect.position[2]);
				isectColorRs->AddValue(std::min(isect.color[0],1.F));
				isectColorGs->AddValue(std::min(isect.color[1],1.F));
				isectColorBs->AddValue(std::min(isect.color[2],1.F));
				primitiveIds->AddValue(isect.primitiveId);
				//					shapeIds->AddValue(isect.shapeId);
				//					interactionTypes->AddValue(isect.interactionType);
				//					lightIds->AddValue(isect.lightId);
				
				isectsToPathsRef->AddReference(xs->Size() - 1, rendererId->Size() - 1);
				indices[i] = xs->Size() - 1;
			}
		}
		else { //I AM SO FULL, I CANNOT EAT ONE MORE BYTE OF DATA
			
			//Any other renderers that are not full?
			for(ushort i = 0 ; i < pathCounts.size() ; ++i) {
				if(i != renderer && pathCounts[i] < maxPaths) {
					//					printf("Renderer #%d still has some room to spare.\n",i);
					return;
				}
			}
			
			//How to resolve the pause, what do we throw out to create more space?
			candidateData->Print(50);
			candidateData->NotifyDataListeners();
			
			//Print the histograms
			xPixelHistogram.Print();
			yPixelHistogram.Print();
			
			Reduce();
		}
		
		
		//				//Should I notify the dataset listeners?
		//				if(rendererId->Size() % updateThrottle == 0) {
		//					currentData->NotifyDataListeners();
		//				}
		
	}
}

RIVDataSet* DataController::Bootstrap(RIVDataSet* dataset, size_t N) {

	RIVDataSet* bootstrap = dataset->CloneStructure();
	RIVTable* bootstrapPaths = bootstrap->GetTable("paths");
	RIVTable* bootstrapIsects = bootstrap->GetTable("intersections");
	
	RIVTable* paths = dataset->GetTable("paths");
	RIVTable* intersections = dataset->GetTable("intersections");
	size_t rows = paths->GetNumRows();
	
	RIVReferenceChain chain;
	paths->GetReferenceChainToTable("intersections", chain);
	
	//Choose N paths, also add the referencing rows
	for(size_t i = 0 ; i < N ; ++i) {
		size_t index = rand() % rows;
		
		bootstrapPaths->CopyRow(paths, index);
		RIVMultiReference* ref = static_cast<RIVMultiReference*>(paths->GetReferenceToTable("intersections"));
		std::pair<size_t*,ushort> refRows = ref->GetIndexReferences(index);
		
		for(ushort i = 0 ; i < refRows.second ; ++i) {
			bootstrapIsects->CopyRow(intersections, refRows.first[i]);
		}
	}
	bootstrap->Print(1000);
	
	return bootstrap;
}

void DataController::Reduce() {
	//This was the first time we filled up, we already have our data to bootstrap from
	if(currentData->IsEmpty()) {
		
		//Swap current with candidate
		RIVDataSet* temp = currentData;
		currentData = candidateData;
		candidateData = temp;
		
		pathCounts.clear();
	}
	else {
		RIVDataSet* bootstrap = Bootstrap(currentData, 1000);
		
		RIVTable* paths = bootstrap->GetTable("paths");
		
		//Compare bootstrap histograms with given histograms
		
//		HistogramSet<float,ushort> bootstrapHistograms = paths->CreateHistograms();
		
//		Histogram<float> bootstrapXHist =
//		Histogram<float> bootstrapYHist = Histogram<float>(0,1,bins);
//		Histogram<float> bootstrapLensUHist = Histogram<float>(0,1,bins);
//		Histogram<float> bootstrapLensVHist = Histogram<float>(0,1,bins);
//		Histogram<float> bootstrapColorRHist = Histogram<float>(0,1,bins);
//		Histogram<float> bootstrapColorGHist = Histogram<float>(0,1,bins);
//		Histogram<float> bootstrapColorBHist = Histogram<float>(0,1,bins);;
//		Histogram<float> bootstrapThroughputRHist = Histogram<float>(0,1,bins);;
//		Histogram<float> bootstrapThroughputGHist = Histogram<float>(0,1,bins);;
//		Histogram<float> bootstrapThroughputBHist = Histogram<float>(0,1,bins);;
//		Histogram<ushort> bootstrapDepthsHist = Histogram<ushort>(0,5);
		
		Histogram<ushort> bootstrapBounceNrs = Histogram<ushort>(0,5);
		Histogram<float> bootstrapXs = Histogram<float>(0,500,bins);
		Histogram<float> bootstrapYs = Histogram<float>(0,500,bins);
		Histogram<float> bootstrapZs = Histogram<float>(0,500,bins);
		Histogram<float> bootstrapIsectColorR = Histogram<float>(0,1,bins);
		Histogram<float> bootstrapIsectColorG = Histogram<float>(0,1,bins);
		Histogram<float> bootstrapIsectColorB = Histogram<float>(0,1,bins);;
		Histogram<ushort> bootstrapPrimitiveIds = Histogram<ushort>(0,6);
		
		
	}
}

bool DataController::IsPaused() {
	return paused;
}