//
//  DataController.cpp
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#include "Configuration.h"
#include "DataController.h"
#include <algorithm>

DataController::DataController(const ushort renderers, const size_t maxPaths) : maxPaths(maxPaths) {
	if(updateThrottle == 0) {
		updateThrottle = maxPaths;
	}
	createDataStructures();
	
	for(ushort i = 0 ; i < renderers ; ++i) {
		pathCounts[i] = 0;
	}
	
	createHistograms();
}

void DataController::createHistograms() {
	
//	xPixelHistogram = Histogram<float>(0,1,bins);
//	yPixelHistogram = Histogram<float>(0,1,bins);
//	lensUHistogram = Histogram<float>(0,1,bins);
//	lensVHistogram = Histogram<float>(0,1,bins);
//	colorRHistogram = Histogram<float>(0,1,bins);
//	colorGHistogram = Histogram<float>(0,1,bins);
//	colorBHistogram = Histogram<float>(0,1,bins);;
//	throughputRHistogram = Histogram<float>(0,1,bins);;
//	throughputGHistogram = Histogram<float>(0,1,bins);;
//	throughputBHistogram = Histogram<float>(0,1,bins);;
//	depthsHistogram = Histogram<ushort>(0,5);
//	
//	bounceNrsHistogram = Histogram<ushort>(0,5);
//	xsHistogram = Histogram<float>(0,500,bins);
//	ysHistogram = Histogram<float>(0,500,bins);
//	zsHistogram = Histogram<float>(0,500,bins);
//	isectColorRHistogram = Histogram<float>(0,1,bins);
//	isectColorGHistogram = Histogram<float>(0,1,bins);
//	isectColorBHistogram = Histogram<float>(0,1,bins);;
//	primitiveIdsHistogram = Histogram<ushort>(0,6);
//	
	//	shapeIdsHistogram = Histogram<ushort>(0,1);
	//	interactionTypesHistogram = Histogram<ushort>(0,1,bins);;
	//	lightIdsHistogram = Histogram<ushort>(0,5,5);
	

}

void DataController::initDataSet(RIVDataSet<float, ushort> *dataset) {
	
	RIVTable<float,ushort>* pathTable = dataset->CreateTable(PATHS_TABLE);
	
	pathTable->CreateRecord<ushort>(RENDERER_ID,0,1);
	pathTable->CreateRecord<float>(PIXEL_X,0,1,true);
	pathTable->CreateRecord<float>(PIXEL_Y,0,1,true);
	//	lensUs = pathTable->CreateFloatRecord("lens U");
	//	lensVs = pathTable->CreateFloatRecord("lens V");
	//	times = pathTable->CreateFloatRecord("time");
	pathTable->CreateRecord<float>(PATH_R,0,1);
	pathTable->CreateRecord<float>(PATH_G,0,1);
	pathTable->CreateRecord<float>(PATH_B,0,1);
	pathTable->CreateRecord<float>(THROUGHPUT_R,0,1);
	pathTable->CreateRecord<float>(THROUGHPUT_G,0,1);
	pathTable->CreateRecord<float>(THROUGHPUT_B,0,1);
	pathTable->CreateRecord<ushort>(DEPTH,0,5);
	
	RIVTable<float,ushort>* isectsTable = dataset->CreateTable(INTERSECTIONS_TABLE);
	
	//TODO: Determine this by reading from the renderer settings
	isectsTable->CreateRecord<ushort>(BOUNCE_NR,0,5);
	isectsTable->CreateRecord<float>(POS_X,0,600,true);
	isectsTable->CreateRecord<float>(POS_Y,0,600,true);
	isectsTable->CreateRecord<float>(POS_Z,0,600,true);
	isectsTable->CreateRecord<float>(INTERSECTION_R,0,1);
	isectsTable->CreateRecord<float>(INTERSECTION_G,0,1);
	isectsTable->CreateRecord<float>(INTERSECTION_B,0,1);
	isectsTable->CreateRecord<ushort>(PRIMITIVE_ID,0,10);
	//	shapeIds = isectsTable->CreateShortRecord("shape ID");
	//	interactionTypes = isectsTable->CreateShortRecord("interaction");
	//	lightIds = isectsTable->CreateShortRecord("light ID");
	
	RIVMultiReference* pathsToIsectRef = new RIVMultiReference(pathTable,isectsTable);
	pathTable->SetReference(pathsToIsectRef);
	RIVSingleReference* isectToPathsRef = new RIVSingleReference(isectsTable,pathTable);
	isectsTable->SetReference(isectToPathsRef);
	
	
}

void DataController::createDataStructures() {
	
	currentData = new RIVDataSet<float,ushort>();
	candidateData = new RIVDataSet<float,ushort>();
	
	initDataSet(currentData);
	initDataSet(candidateData);
	
	trueDistributions = currentData->CreateHistogramSet(bins);
	trueDistributions.Print();
	
	resetPointers(currentData);
}

void DataController::resetPointers(RIVDataSet<float,ushort>* dataset) {
	currentPathTable = dataset->GetTable(PATHS_TABLE);
	currentIntersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
	
	isectsToPathsRef = (RIVSingleReference*)currentIntersectionsTable->GetReference();
	pathsToIsectRef = (RIVMultiReference*)currentPathTable->GetReference();
	
	rendererId = currentPathTable->GetRecord<ushort>(RENDERER_ID);
	xPixels = currentPathTable->GetRecord<float>(PIXEL_X);
	yPixels = currentPathTable->GetRecord<float>(PIXEL_Y);
	//	lensUs = pathTable->GetRecord<float>("lens U");
	//	lensVs = pathTable->GetRecord<float>("lens V");
	//	times = pathTable->GetRecord<float>("time");
	colorRs = currentPathTable->GetRecord<float>(PATH_R);
	colorGs = currentPathTable->GetRecord<float>(PATH_G);
	colorBs = currentPathTable->GetRecord<float>(PATH_B);
	throughputRs = currentPathTable->GetRecord<float>(THROUGHPUT_R);
	throughputGs = currentPathTable->GetRecord<float>(THROUGHPUT_G);
	throughputBs = currentPathTable->GetRecord<float>(THROUGHPUT_B);
	depths = currentPathTable->GetRecord<ushort>(DEPTH);
	
	bounceNrs = currentIntersectionsTable->GetRecord<ushort>(BOUNCE_NR);
	xs = currentIntersectionsTable->GetRecord<float>(POS_X);
	ys = currentIntersectionsTable->GetRecord<float>(POS_Y);
	zs = currentIntersectionsTable->GetRecord<float>(POS_Z);
	isectColorRs = currentIntersectionsTable->GetRecord<float>(INTERSECTION_R);
	isectColorGs = currentIntersectionsTable->GetRecord<float>(INTERSECTION_G);
	isectColorBs = currentIntersectionsTable->GetRecord<float>(INTERSECTION_B);
	primitiveIds = currentIntersectionsTable->GetRecord<ushort>(PRIMITIVE_ID);
	//	shapeIds = isectsTable->CreateShortRecord("shape ID");
	//	interactionTypes = isectsTable->CreateShortRecord("interaction");
	//	lightIds = isectsTable->CreateShortRecord("light ID");
	
}

RIVDataSet<float,ushort>* DataController::GetDataSet() {
	return currentData;
}

void DataController::ProcessNewPath(ushort renderer, PathData* newPath) {
	//	printf("New path from renderer #1 received!\n");
	//	return;
	if(!paused) {
		//ALWAYS update the histograms
		ushort nrIntersections = newPath->intersectionData.size();
		
//		trueDistributions.AddToHistogram(PIXEL_X, newPath->imageX);
//		trueDistributions.AddToHistogram(PIXEL_Y, newPath->imageY);
//		trueDistributions.AddToHistogram(PATH_R, std::min(newPath->radiance[0],1.F));
//		trueDistributions.AddToHistogram(PATH_G, std::min(newPath->radiance[1],1.F));
//		trueDistributions.AddToHistogram(PATH_B, std::min(newPath->radiance[2],1.F));
//		trueDistributions.AddToHistogram(THROUGHPUT_R, std::min(newPath->throughput[0],1.F));
//		trueDistributions.AddToHistogram(THROUGHPUT_G, std::min(newPath->throughput[1],1.F));
//		trueDistributions.AddToHistogram(THROUGHPUT_B, std::min(newPath->throughput[2],1.F));
//		trueDistributions.AddToHistogram(DEPTH, nrIntersections);
	
		for(ushort i = 0 ; i < nrIntersections ; ++i) {
			IntersectData& isect = newPath->intersectionData[i];
			
			trueDistributions.AddToHistogram(BOUNCE_NR,(ushort)(i+1));
//			trueDistributions.AddToHistogram("x", isect.position[0]);
//			trueDistributions.AddToHistogram("y", isect.position[1]);
//			trueDistributions.AddToHistogram("z", isect.position[2]);
//			trueDistributions.AddToHistogram("R", std::min(isect.color[0],1.F));
//			trueDistributions.AddToHistogram("G", std::min(isect.color[1],1.F));
//			trueDistributions.AddToHistogram("B", std::min(isect.color[2],1.F));
//			trueDistributions.AddToHistogram("throughput B", newPath->imageY);
//			trueDistributions.AddToHistogram("depth", nrIntersections);
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
			currentData->Print(50);
			currentData->NotifyDataListeners();
			
			//Print the histograms
//			trueDistributions.Print();
			
			paused = true;
			
//			Reduce();
		}
		
		
		//				//Should I notify the dataset listeners?
		//				if(rendererId->Size() % updateThrottle == 0) {
		//					currentData->NotifyDataListeners();
		//				}
		
	}
}



void DataController::Reduce() {
	//This was the first time we filled up, we already have our data to bootstrap from
	if(currentData == NULL || currentData->IsEmpty()) {
		
		//Swap current with candidate
		RIVDataSet<float,ushort>* temp = currentData;
		currentData = candidateData;
		candidateData = temp;
		
		pathCounts.clear();
	}
	else {
		RIVDataSet<float,ushort> bootstrap = Bootstrap(currentData, 1000);
		
//		RIVTable<float,ushort>* paths = bootstrap.GetTable("paths");
		
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
		
//		Histogram<ushort> bootstrapBounceNrs = Histogram<ushort>(0,5);
//		Histogram<float> bootstrapXs = Histogram<float>(0,500,bins);
//		Histogram<float> bootstrapYs = Histogram<float>(0,500,bins);
//		Histogram<float> bootstrapZs = Histogram<float>(0,500,bins);
//		Histogram<float> bootstrapIsectColorR = Histogram<float>(0,1,bins);
//		Histogram<float> bootstrapIsectColorG = Histogram<float>(0,1,bins);
//		Histogram<float> bootstrapIsectColorB = Histogram<float>(0,1,bins);;
//		Histogram<ushort> bootstrapPrimitiveIds = Histogram<ushort>(0,6);
		
		
	}
}

bool DataController::IsPaused() {
	return paused;
}