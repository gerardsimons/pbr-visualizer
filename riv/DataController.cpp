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

RIVDataSet<float,ushort>* DataController::Bootstrap(RIVDataSet<float, ushort>* dataset,const size_t N) {
	
	RIVDataSet<float,ushort>* bootstrap = dataset->CloneStructure();
	RIVTable<float,ushort>* bootstrapPaths = bootstrap->GetTable("paths");
	RIVTable<float,ushort>* bootstrapIsects = bootstrap->GetTable("intersections");
	
	RIVTable<float,ushort>* paths = dataset->GetTable("paths");
	RIVTable<float,ushort>* intersections = dataset->GetTable("intersections");
	size_t rows = paths->NumberOfRows();

	
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
	
	RIVMultiReference* ref = static_cast<RIVMultiReference*>(paths->GetReference());
	
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
			auto shortRec = shortRecords->at(i);
			
			bootShort->AddValue(shortRec->Value(index));
		}
		
		std::pair<size_t*,ushort> refRows = ref->GetReferenceRows(index);
		for(size_t i = 0 ; i < isectFloatRecords->size() ; ++i) {
			for(size_t j = 0 ; j < refRows.second ; ++j) {
				auto bootFloat = bootstrapIsectFloatRecords->at(i);
				auto floatRec = isectFloatRecords->at(i);
				
				bootFloat->AddValue(floatRec->Value(refRows.first[j]));
			}
		}
		
		for(size_t i = 0 ; i < isectShortRecords->size() ; ++i) {
			for(size_t j = 0 ; j < refRows.second ; ++j) {
				auto bootShort = bootstrapIsectShortRecords->at(i);
				auto shortRec = isectShortRecords->at(i);
				
				bootShort->AddValue(shortRec->Value(refRows.first[j]));
			}
		}
	}
	//		bootstrap.Print(1000);
	
	return bootstrap;
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
	
	//	pathTable->CreateRecord<ushort>(RENDERER_ID,0,1);
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
	
	rendererDataOne = new RIVDataSet<float,ushort>(DATASET_ONE);
	rendererDataTwo = new RIVDataSet<float,ushort>(DATASET_TWO);
	
	candidateDataOne = new RIVDataSet<float,ushort>(DATASET_ONE);
	candidateDataTwo = new RIVDataSet<float,ushort>(DATASET_TWO);
	
	initDataSet(rendererDataOne);
	initDataSet(candidateDataOne);
	
	initDataSet(rendererDataTwo);
	initDataSet(candidateDataTwo);
	
	trueDistributionsOne = rendererDataOne->CreateHistogramSet(bins);
	trueDistributionsTwo = rendererDataTwo->CreateHistogramSet(bins);
}

void DataController::resetPointers(RIVDataSet<float,ushort>* dataset) {
	currentPathTable = dataset->GetTable(PATHS_TABLE);
	//	currentPathTable->ClearFilters();
	//	currentPathTable->ClearFilter("");
	currentIntersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
	
	isectsToPathsRef = (RIVSingleReference*)currentIntersectionsTable->GetReference();
	pathsToIsectRef = (RIVMultiReference*)currentPathTable->GetReference();
	
	//	rendererId = currentPathTable->GetRecord<ushort>(RENDERER_ID);
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
RIVDataSet<float,ushort>** DataController::GetDataSetOne() {
	return &rendererDataOne;
}
RIVDataSet<float,ushort>** DataController::GetDataSetTwo() {
	return &rendererDataTwo;
}
bool DataController::ProcessNewPath(int frame, ushort renderer, PathData* newPath) {
	int* currentFrame = &lastFrameOne;
	if(renderer == 0) {
//		++pathsPerFrameOne;
//		raysPerFrameOne += newPath->intersectionData.size();
//		printf("Paths per frame ONE = %zu\n",pathsPerFrameOne);
	}
	else {
		currentFrame = &lastFrameTwo;
//		++pathsPerFrameTwo;
//		raysPerFrameTwo += newPath->intersectionData.size();
//		printf("Paths per frame ONE = %zu\n",pathsPerFrameTwo);
	}
	if(IsActive()) {
		HistogramSet<float,ushort>* trueDistributions;
		RIVDataSet<float,ushort>** currentData;
		RIVDataSet<float,ushort>** candidateData;
		RIVDataSet<float,ushort>** bestBootstrap;
		float* bestResult;
		float acceptProbability;
		
		if(renderer == 0) {
			resetPointers(rendererDataOne);
			currentData = &rendererDataOne;
			candidateData = &candidateDataOne;
			trueDistributions = &trueDistributionsOne;
			bestResult = &bestBootstrapResultOne;
			bestBootstrap = &bootstrapOne;
			
			acceptProbability = acceptProbabilityOne;
		}
		else {
			resetPointers(rendererDataTwo);
			currentData = &rendererDataTwo;
			candidateData = &candidateDataTwo;
			trueDistributions = &trueDistributionsTwo;
			bestResult = &bestBootstrapResultTwo;
			bestBootstrap = &bootstrapTwo;
//			++pathsPerFrameTwo;
			acceptProbability = acceptProbabilityTwo;
		}
		
		//ALWAYS update the histograms
		ushort nrIntersections = newPath->intersectionData.size();
		
		trueDistributions->AddToHistogram(PIXEL_X, newPath->imageX);
		trueDistributions->AddToHistogram(PIXEL_Y, newPath->imageY);
		trueDistributions->AddToHistogram(PATH_R, std::min(newPath->radiance[0],1.F));
		trueDistributions->AddToHistogram(PATH_G, std::min(newPath->radiance[1],1.F));
		trueDistributions->AddToHistogram(PATH_B, std::min(newPath->radiance[2],1.F));
		trueDistributions->AddToHistogram(THROUGHPUT_R, std::min(newPath->throughput[0],1.F));
		trueDistributions->AddToHistogram(THROUGHPUT_G, std::min(newPath->throughput[1],1.F));
		trueDistributions->AddToHistogram(THROUGHPUT_B, std::min(newPath->throughput[2],1.F));
		trueDistributions->AddToHistogram(DEPTH, nrIntersections);
		
		for(ushort i = 0 ; i < nrIntersections ; ++i) {
			IntersectData& isect = newPath->intersectionData[i];
			
			trueDistributions->AddToHistogram(BOUNCE_NR,(ushort)(i+1));
			trueDistributions->AddToHistogram(POS_X, isect.position[0]);
			trueDistributions->AddToHistogram(POS_Y, isect.position[1]);
			trueDistributions->AddToHistogram(POS_Z, isect.position[2]);
			trueDistributions->AddToHistogram(INTERSECTION_R, std::min(isect.color[0],1.F));
			trueDistributions->AddToHistogram(INTERSECTION_G, std::min(isect.color[1],1.F));
			trueDistributions->AddToHistogram(INTERSECTION_B, std::min(isect.color[2],1.F));
			//			trueDistributions.AddToHistogram("depth", nrIntersections);
		}
		//				mutex.lock();
		//				printf("Adding new path.\n");
		bool tableFull = pathCounts[renderer] >= maxPaths;
		if(!tableFull) {
			float accept = rand() / (float)RAND_MAX;
//			printf("Accept prob = %f\n",accept);
			if(accept <= acceptProbability) {
				++pathCounts[renderer];
				//			rendererId->AddValue(renderer);
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
				
				pathsToIsectRef->AddReferences(xPixels->Size() - 1, std::pair<size_t*,ushort>(indices,nrIntersections));
				
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
					
					isectsToPathsRef->AddReference(xs->Size() - 1, xPixels->Size() - 1);
					indices[i] = xs->Size() - 1;
				}
				return true;
			}
			else { //I AM SO FULL, I CANNOT EAT ONE MORE BYTE OF DATA
				
				//Any other renderers that are not full?
				for(ushort i = 0 ; i < pathCounts.size() ; ++i) {
					if(i != renderer && pathCounts[i] < maxPaths) {
						//					printf("Renderer #%d still has some room to spare.\n",i);
						return true;
					}
				}
				
				//Print the histograms
				//			trueDistributions.Print();
				//			currentData->Print(50);
				
				Reduce(currentData,candidateData,trueDistributions,bestBootstrap, bestResult);
				
				return false;
			}
		}
	}
	else {
		if(paused) {
//			printf("... is paused.\n");
		}
		else if(IsDelayed()) {
//			printf("... is delayed.\n");
		}
//		clock_t t = clock() - startPause;
//		int millipassed = t / (float)CLOCKS_PER_SEC * 1000;
//		printf ("It took me %d milliseconds).\n",millipassed);
//		pausedTimer -= millipassed;
//		printf ("%d milliseconds left.\n",pausedTimer);
//		printf("DataProcessing is paused (t = %d)\n",pausedTimer);
		return false;
	}
}

void DataController::Reduce(RIVDataSet<float,ushort>** currentData, RIVDataSet<float,ushort>** candidateData, HistogramSet<float,ushort>* trueDistributions, RIVDataSet<float,ushort>** bestBootstrap, float* bestBootstrapResult) {
	//This was the first time we filled up, we already have our data to bootstrap from
	if(firstTime) {
//		printf("Current data is full...\n");
//		printf("Swapping candidate and current data\n");
		
//		printf("\nCURRENT DATA = \n");
//		(*currentData)->Print(100);
		
		//Important to notify before the swap
		(*currentData)->NotifyDataListeners();
		
		//Swap current with candidate
		(*candidateData)->SetDataListeners((*currentData)->GetDataListeners());
		RIVDataSet<float,ushort>* temp = *currentData;
		(*currentData) = *candidateData;
		(*candidateData) = temp;
		
		pathCounts.clear();
		
		firstTime = false;
	}
	else { //Both are full, join the two sets, bootstrap
		
		//Join the two datasets
		(*currentData)->AddDataSet(*candidateData);
		
		printHeader("BOOTSTRAPPING",100);
		HistogramSet<float,ushort> bootstrapHistograms;
		
		bool newBootstrapFound = false;

		RIVDataSet<float,unsigned short>* bootstrap = NULL;
		for(int i = 0 ; i < bootstrapRepeat ; ++i) {
//			printf("Round #%d\n",i);
			bootstrap = Bootstrap((*currentData), maxPaths);
			
			bootstrapHistograms = bootstrap->CreateHistogramSet(bins);
			float score = trueDistributions->DistanceTo(bootstrapHistograms);
//			float score = maxPaths - i;
			
			if(*bestBootstrapResult < 0 || score < *bestBootstrapResult) {
				
				printf("BETTER BOOTSTRAP FOUND! SCORE = %f\n",score);
				if((*bestBootstrap)) {
//					printf("Delete previous best bootstrap\n");
					delete (*bestBootstrap);
				}
				
				(*bestBootstrap) = bootstrap;
				
//					printf("\nTRUE HISTOGRAMS = \n");
//					trueDistributions->Print();
//				
//					printf("\nBOOTSTRAP HISTOGRAMS = \n");
//					bootstrapHistograms.Print();
				
				//	printf("Bootstrap = \n\n");
				//	bootstrap->Print(100);
				
				
				*bestBootstrapResult = score;
				newBootstrapFound = true;
			}
			else {
//				printf("No good, delete bootstrap\n");
				delete bootstrap;
			}
			bootstrap = NULL;
		}
		if(newBootstrapFound) {
			
			printf("\n\n***** NEW BOOTSTRAP FOUND! ***** \n\n");
			
//			printf("ALL DATA:\n");
//			(*currentData)->Print();
			
//			(*bestBootstrap)->Print();
			
			printf("\nTRUE HISTOGRAMS = \n");
			trueDistributions->Print();
			
			printf("\nBOOTSTRAP HISTOGRAMS = \n");
			bootstrapHistograms.Print();
			
			(*bestBootstrap)->SetDataListeners((*currentData)->GetDataListeners());
			auto pathsTable = (*bestBootstrap)->GetTable(PATHS_TABLE);
			auto isectsTable = (*bestBootstrap)->GetTable(INTERSECTIONS_TABLE);
			
			//Create new references as they are not created by the bootstrapping
			pathsTable->SetReference(new RIVMultiReference(pathsTable,isectsTable));
			isectsTable->SetReference(new RIVSingleReference(isectsTable,pathsTable));
			
			//Delete the old current data
			delete (*currentData);
			(*currentData) = (*bestBootstrap);
			(*bestBootstrap) = NULL;
			resetPointers((*currentData));//Reset the different pointers to the new current data
			
			(*candidateData)->ClearData();
			
			//Fix the new reference paths
			size_t intersectionsCount = 0;
			size_t pathsCount = 0;
			
			//			printf("\nBOOTSTRAP RESULT = \n");
			//			(*currentData)->Print(100);
			
			for(size_t i = 0 ; i < depths->Size() ; ++i) {
				ushort depth = depths->Value(i);
				std::pair<size_t*,ushort> rowsMapping;
				size_t* rows = new size_t[depth];
				for(size_t j = 0 ; j < depth ; ++j) {
					rows[j] = intersectionsCount;
					isectsToPathsRef->AddReference(intersectionsCount, pathsCount);
					++intersectionsCount;
				}
				rowsMapping.second = depth;
				rowsMapping.first = rows;
				pathsToIsectRef->AddReferences(i, rowsMapping);
				++pathsCount;
			}
			
			//			printf("\nBOOTSTRAP RESULT = \n");
			//			(*currentData)->Print(100);
			
			pathCounts.clear();
			(*currentData)->NotifyDataListeners();
			++reduceRounds;
			if(reduceRounds % 2 == 0) {
				Pause();	
			}
		}
		else {
			printf("\n Could not find a better bootstrap... \n");
		}
	}
}

void DataController::RendererOneFinishedFrame(size_t numPaths,size_t numRays) {
	//Recompute the acceptance probability
	if(maxPaths > pathsPerFrameOne) {
//		acceptProbabilityOne = 1;
	}
	else {
//		acceptProbabilityOne = maxPaths / (float)numPaths;
	}
//	printf("Renderer one finished a frame.\n");
//	printf("%zu rays generated\n",raysPerFrameOne);
//	printf("%zu paths generated\n",pathsPerFrameOne);
//	printf("New acceptance probability = %f\n",acceptProbabilityOne);
	pathsPerFrameOne = 0;
	raysPerFrameOne = 0;
}

void DataController::RendererTwoFinishedFrame(size_t numPaths,size_t numRays) {
//	if(maxPaths > pathsPerFrameTwo) {
//		acceptProbabilityTwo = 1;
//	}
//	else {
//		acceptProbabilityTwo = maxPaths / (float)pathsPerFrameTwo;
//	}
//	printf("Renderer two finished a frame.\n");
//	printf("%zu rays generated\n",raysPerFrameTwo);
//	printf("%zu paths generated\n",pathsPerFrameTwo);
//	printf("New acceptance probability = %f\n",acceptProbabilityTwo);
	pathsPerFrameTwo = 0;
	raysPerFrameTwo = 0;
}

void DataController::SetAcceptProbabilityOne(float newProb) {
	acceptProbabilityOne = newProb;
}

void DataController::SetAcceptProbabilityTwo(float newProb) {
	acceptProbabilityTwo = newProb;
}