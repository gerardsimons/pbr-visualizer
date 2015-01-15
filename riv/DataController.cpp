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

DataController::DataController(const ushort renderers, const size_t maxPaths, const size_t bootstrapRepeat) : maxPaths(maxPaths), bootstrapRepeat(bootstrapRepeat) {
	if(updateThrottle == 0) {
		updateThrottle = maxPaths;
	}
	createDataStructures();
}

RIVDataSet<float,ushort>* DataController::Bootstrap(RIVDataSet<float, ushort>* dataset,const size_t N) {
	
	
//	const std::string taskName = "Creating bootstrap";
//	reporter::startTask("Creating bootstrap", N);
	
	RIVDataSet<float,ushort>* bootstrap = dataset->CloneStructure();
	RIVTable<float,ushort>* bootstrapPaths = bootstrap->GetTable(PATHS_TABLE);
	RIVTable<float,ushort>* bootstrapIsects = bootstrap->GetTable(INTERSECTIONS_TABLE);
	
	RIVTable<float,ushort>* paths = dataset->GetTable(PATHS_TABLE);
	RIVTable<float,ushort>* intersections = dataset->GetTable(INTERSECTIONS_TABLE);
	size_t rows = paths->NumberOfRows();
	
	//Give the underlying data structure a chance to reserve space
	bootstrapPaths->ReserveRows(N);

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
	
//	bool includeReference = (rand() % 2) > 0;
//	if(includeReference)
//		printf("Include references...\n");
//	else {
//		printf("NOT include references...\n");
//	}
	//Choose N paths, also add the referencing rows
	for(size_t i = 0 ; i < N ; ++i) {
//		reporter::update(taskName);
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

		const std::pair<size_t*,ushort>& refRows = ref->GetReferenceRows(index);
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
	
//	reporter::stop("Creating bootstrap");
	return bootstrap;
}

void DataController::initDataSet(RIVDataSet<float, ushort> *dataset) {
	
	RIVTable<float,ushort>* pathTable = dataset->CreateTable(PATHS_TABLE);
	
	pathTable->CreateRecord<float>(PIXEL_X,0,1,true);
	pathTable->CreateRecord<float>(PIXEL_Y,0,1,true);
	pathTable->CreateRecord<float>(PATH_R,0,1);
	pathTable->CreateRecord<float>(PATH_G,0,1);
	pathTable->CreateRecord<float>(PATH_B,0,1);
	pathTable->CreateRecord<float>(THROUGHPUT_R,0,1);
	pathTable->CreateRecord<float>(THROUGHPUT_G,0,1);
	pathTable->CreateRecord<float>(THROUGHPUT_B,0,1);
	pathTable->CreateRecord<ushort>(DEPTH,0,5,true);
	
	RIVTable<float,ushort>* isectsTable = dataset->CreateTable(INTERSECTIONS_TABLE);
	
	//TODO: Determine this by reading from the renderer settings
	isectsTable->CreateRecord<ushort>(BOUNCE_NR,1,5,true);
	isectsTable->CreateRecord<float>(POS_X,0,600,true);
	isectsTable->CreateRecord<float>(POS_Y,0,600,true);
	isectsTable->CreateRecord<float>(POS_Z,0,600,true);
	isectsTable->CreateRecord<float>(INTERSECTION_R,0,1);
	isectsTable->CreateRecord<float>(INTERSECTION_G,0,1);
	isectsTable->CreateRecord<float>(INTERSECTION_B,0,1);
	isectsTable->CreateRecord<ushort>(PRIMITIVE_ID,0,10,true);
	//	shapeIds = isectsTable->CreateShortRecord("shape ID");
	//	interactionTypes = isectsTable->CreateShortRecord("interaction");
	//	lightIds = isectsTable->CreateShortRecord("light ID");
	
	RIVMultiReference* pathsToIsectRef = new RIVMultiReference(pathTable,isectsTable);
	pathTable->SetReference(pathsToIsectRef);
	RIVSingleReference* isectToPathsRef = new RIVSingleReference(isectsTable,pathTable);
	isectsTable->SetReference(isectToPathsRef);
	
	auto pathMembershipOneTable = dataset->CreateTable(PATH_MEMBERSHIP_TABLE);
	auto isectMembershipOneTable = dataset->CreateTable(ISECT_MEMBERSHIP_TABLE);
	
	auto pathMembershipTwoTable = dataset->CreateTable(PATH_MEMBERSHIP_TABLE);
	auto isectMembershipTwoTable = dataset->CreateTable(ISECT_MEMBERSHIP_TABLE);
	
	pathMembershipOneTable->CreateRecord<float>(MEMBERSHIP);
	isectMembershipOneTable->CreateRecord<float>(MEMBERSHIP);
	pathMembershipTwoTable->CreateRecord<float>(MEMBERSHIP);
	isectMembershipTwoTable->CreateRecord<float>(MEMBERSHIP);
}

void DataController::createDataStructures() {
	
	rendererData = new RIVDataSet<float,ushort>(DATASET_ONE);
	
	candidateData = new RIVDataSet<float,ushort>(DATASET_ONE);
	
	//Create the records and such for the datasets
	initDataSet(rendererData);
	initDataSet(candidateData);
	
//	initDataSet(rendererDataTwo);
//	initDataSet(candidateDataTwo);
	
	trueDistributions = rendererData->CreateHistogramSet(bins);
//	trueDistributionsTwo = rendererDataTwo->CreateHistogramSet(bins);
	
	resetPointers(rendererData);
}

void DataController::resetPointers(RIVDataSet<float,ushort>* dataset) {
	currentPathTable = dataset->GetTable(PATHS_TABLE);
	//	currentPathTable->ClearFilters();
	//	currentPathTable->ClearFilter("");
	currentIntersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
	
	isectsToPathsRef = (RIVSingleReference*)currentIntersectionsTable->GetReference();
	pathsToIsectRef = (RIVMultiReference*)currentPathTable->GetReference();
	
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
RIVDataSet<float,ushort>** DataController::GetDataSet() {
	return &rendererData;
}
//RIVDataSet<float,ushort>** DataController::GetDataSetTwo() {
//	return &rendererDataTwo;
//}
bool DataController::ProcessNewPath(int frame, PathData* newPath) {
	if(IsActive()) {
		//ALWAYS update the histograms
		ushort nrIntersections = newPath->intersectionData.size();
		
		trueDistributions.AddToHistogram(PIXEL_X, newPath->imageX);
		trueDistributions.AddToHistogram(PIXEL_Y, newPath->imageY);
		trueDistributions.AddToHistogram(PATH_R, std::min(newPath->radiance[0],1.F));
		trueDistributions.AddToHistogram(PATH_G, std::min(newPath->radiance[1],1.F));
		trueDistributions.AddToHistogram(PATH_B, std::min(newPath->radiance[2],1.F));
		trueDistributions.AddToHistogram(THROUGHPUT_R, std::min(newPath->throughput[0],1.F));
		trueDistributions.AddToHistogram(THROUGHPUT_G, std::min(newPath->throughput[1],1.F));
		trueDistributions.AddToHistogram(THROUGHPUT_B, std::min(newPath->throughput[2],1.F));
		trueDistributions.AddToHistogram(DEPTH, nrIntersections);
		
		for(ushort i = 0 ; i < nrIntersections ; ++i) {
			IntersectData& isect = newPath->intersectionData[i];
			
			trueDistributions.AddToHistogram(BOUNCE_NR,(ushort)(i+1));
			trueDistributions.AddToHistogram(POS_X, isect.position[0]);
			trueDistributions.AddToHistogram(POS_Y, isect.position[1]);
			trueDistributions.AddToHistogram(POS_Z, isect.position[2]);
			trueDistributions.AddToHistogram(INTERSECTION_R, std::min(isect.color[0],1.F));
			trueDistributions.AddToHistogram(INTERSECTION_G, std::min(isect.color[1],1.F));
			trueDistributions.AddToHistogram(INTERSECTION_B, std::min(isect.color[2],1.F));
			//			trueDistributions.AddToHistogram("depth", nrIntersections);
		}
		//				mutex.lock();
		//				printf("Adding new path.\n");
		bool tableFull = pathCount >= maxPaths;
		if(!tableFull) {
			float accept = rand() / (float)RAND_MAX;
//			printf("Accept prob = %f\n",accept);
			if(accept <= acceptProbability) {
				++pathCount;
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
				
				pathsToIsectRef->AddReferences(colorRs->Size() - 1, std::pair<size_t*,ushort>(indices,nrIntersections));
				
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
					
					isectsToPathsRef->AddReference(xs->Size() - 1, colorRs->Size() - 1);
					indices[i] = xs->Size() - 1;
				}
				return true;
			}
		}
		else { //I AM SO FULL, I CANNOT EAT ONE MORE BYTE OF DATA
			

			
			//Print the histograms
			//			trueDistributions.Print();
			//			currentData->Print(50);
			
			Reduce();
			
			return false;
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

void DataController::Reduce() {
	//This was the first time we filled up, we already have our data to bootstrap from
	if(firstTime) {
//		printf("Current data is full...\n");
//		printf("Swapping candidate and current data\n");
		
//		printf("\nCURRENT DATA = \n");
//		rendererData->Print(100);
		
		//Important to notify before the swap
		rendererData->NotifyDataListeners();
		
		//Swap current with candidate
		candidateData->SetDataListeners(rendererData->GetDataListeners());
		RIVDataSet<float,ushort>* temp = rendererData;
		rendererData = candidateData;
		candidateData = temp;
		
		pathCount = 0;
		
		firstTime = false;
	}
	else { //Both are full, join the two sets, bootstrap
		
		//Join the two datasets
		rendererData->AddDataSet(candidateData);
		
		printHeader("BOOTSTRAPPING",100);
		
		const std::string taskName = "bootstrapping";
		reporter::startTask("bootstrapping");
		HistogramSet<float,ushort> bootstrapHistograms;
		
		bool newBootstrapFound = false;

		for(int i = 0 ; i < bootstrapRepeat ; ++i) {
//			printf("Round #%d\n",i);
			auto bootstrap = Bootstrap(rendererData, maxPaths);
			
			bootstrapHistograms = bootstrap->CreateHistogramSet(bins);
			float score = trueDistributions.DistanceTo(bootstrapHistograms);
//			float score = maxPaths - i;
			
			if(bestBootstrapResult < 0 || score < bestBootstrapResult) {
				
				printf("BETTER BOOTSTRAP FOUND! SCORE = %f\n",score);
				if(bestBootstrap) { //A new and better bootstrap was found, delete the old bestBootstrap
//					printf("Delete previous best bootstrap\n");
					delete bestBootstrap;
				}
				
				bestBootstrap = bootstrap;
				
//					printf("\nTRUE HISTOGRAMS = \n");
//					trueDistributions.Print();
//				
//					printf("\nBOOTSTRAP HISTOGRAMS = \n");
//					bootstrapHistograms.Print();
				
				//	printf("Bootstrap = \n\n");
				//	bootstrap->Print(100);
				
				
				bestBootstrapResult = score;
				newBootstrapFound = true;
			}
			else { //This bootstrap is not a better fit than the previous one, delete it
				delete bootstrap;
			}
			bootstrap = NULL;
		}
		if(newBootstrapFound) {
			
			printf("\n\n***** NEW BOOTSTRAP FOUND! ***** \n\n");
			
//			printf("ALL DATA:\n");
//			(*currentData)->Print();
			
//			(*bestBootstrap)->Print();
			
//			printf("\nTRUE HISTOGRAMS = \n");
//			trueDistributions.Print();
//			
//			printf("\nBOOTSTRAP HISTOGRAMS = \n");
//			bootstrapHistograms.Print();
			
			bestBootstrap->SetDataListeners(rendererData->GetDataListeners());
			auto pathsTable = bestBootstrap->GetTable(PATHS_TABLE);
			auto isectsTable = bestBootstrap->GetTable(INTERSECTIONS_TABLE);
			
			//Create new references as they are not created by the bootstrapping
			pathsTable->SetReference(new RIVMultiReference(pathsTable,isectsTable));
			isectsTable->SetReference(new RIVSingleReference(isectsTable,pathsTable));
			
			//Delete the old renderer data and replace it with the bootstrap dataset,
			delete rendererData;
			rendererData = bestBootstrap;
			bestBootstrap = NULL;
			resetPointers(rendererData);	//Reset the shortcut pointers to the new data
			
			candidateData->ClearData();
			
			//Fix the new reference paths
			size_t intersectionsCount = 0;
			size_t pathsCount = 0;
			
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
			
			//	printf("\nBOOTSTRAP RESULT = \n");
			//	(*currentData)->Print(100);
			
			pathCount = 0;
			rendererData->NotifyDataListeners();
//			++reduceRounds;
//			if(reduceRounds % 2 == 0) {
//			}
		}
		else {
			printf("\n Could not find a better bootstrap... \n");
		}
		reporter::stop("bootstrapping");
		Pause();
	}
}

void DataController::RendererOneFinishedFrame(size_t numPaths,size_t numRays) {
	//Recompute the acceptance probability
//	if(maxPaths > pathsPerFrameOne) {
//		acceptProbabilityOne = 1;
//	}
//	else {
//		acceptProbabilityOne = maxPaths / (float)numPaths;
//	}
//	printf("Renderer one finished a frame.\n");
//	printf("%zu rays generated\n",raysPerFrameOne);
//	printf("%zu paths generated\n",pathsPerFrameOne);
//	printf("New acceptance probability = %f\n",acceptProbabilityOne);
//	pathsPerFrameOne = 0;
//	raysPerFrameOne = 0;
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
//	pathsPerFrameTwo = 0;
//	raysPerFrameTwo = 0;
}

void DataController::SetAcceptProbability(float newProb) {
	acceptProbability = newProb;
}
//
//void DataController::SetAcceptProbabilityTwo(float newProb) {
//	acceptProbabilityTwo = newProb;
//}