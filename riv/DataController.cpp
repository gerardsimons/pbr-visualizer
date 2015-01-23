//
//  DataController.cpp
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#include "Configuration.h"
#include "DataController.h"

#include <set>
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
	
	RIVDataSet<float,ushort>* bootstrap = dataset->CloneStructure(tablesToBootstrap);
	RIVTable<float,ushort>* bootstrapPaths = bootstrap->GetTable(PATHS_TABLE);
	RIVTable<float,ushort>* bootstrapIsects = bootstrap->GetTable(INTERSECTIONS_TABLE);
	
	RIVTable<float,ushort>* paths = dataset->GetTable(PATHS_TABLE);
	RIVTable<float,ushort>* intersections = dataset->GetTable(INTERSECTIONS_TABLE);
	size_t rows = paths->NumberOfRows();
	
	//Give the underlying data structure a chance to reserve space
	bootstrapPaths->ReserveRows(N);
	
	RIVMultiReference* ref = static_cast<RIVMultiReference*>(paths->GetReference());
	
	auto& pathRecords = paths->GetRecords();
	auto& isectRecords = intersections->GetRecords();
	
	//	auto& bootstrapRecords = paths->GetRecords();
	
	//	bool includeReference = (rand() % 2) > 0;
	//	if(includeReference)
	//		printf("Include references...\n");
	//	else {
	//		printf("NOT include references...\n");
	//	}
	//Choose N paths, also add the referencing rows
	std::vector<size_t> sampledRows;
	for(size_t i = 0 ; i < N ; ++i) {
		//		reporter::update(taskName);
		sampledRows.push_back(rand() % rows);
	}
	//		size_t index = i;
	
	//		printf("Sampled row %zu\n",index);
	tuple_for_each(pathRecords, [&](auto tRecords) {
		for(auto record : tRecords) {
			typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
			auto bootRecord = bootstrapPaths->GetRecord<Type>(record->name);
			
			for(size_t index : sampledRows) {
				bootRecord->AddValue(record->Value(index));
			}
		}
	});
	
	tuple_for_each(isectRecords, [&](auto tRecords) {
		for(auto record : tRecords) {
			typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
			auto bootRecord = bootstrapIsects->GetRecord<Type>(record->name);
			for(size_t index : sampledRows) {
				
				const std::pair<size_t*,ushort>& refRows = ref->GetReferenceRows(index);
				for(int i = 0 ; i < refRows.second ; ++i) {
					
					bootRecord->AddValue(record->Value(refRows.first[i]));
					
				}
				
			}
		}
	});
	
	
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
	pathTable->CreateRecord<float>(THROUGHPUT_R,0,1,true);
	pathTable->CreateRecord<float>(THROUGHPUT_G,0,1,true);
	pathTable->CreateRecord<float>(THROUGHPUT_B,0,1,true);
	pathTable->CreateRecord<ushort>(DEPTH,0,5,true);
	
	RIVTable<float,ushort>* isectsTable = dataset->CreateTable(INTERSECTIONS_TABLE);
	
	//TODO: Determine this by reading from the renderer settings
	isectsTable->CreateRecord<ushort>(BOUNCE_NR,1,5,true);
	isectsTable->CreateRecord<float>(POS_X,0,550,true);
	isectsTable->CreateRecord<float>(POS_Y,0,550,true);
	isectsTable->CreateRecord<float>(POS_Z,0,550,true);
//	isectsTable->CreateRecord<float>(DIR_X,0,1,true);
//	isectsTable->CreateRecord<float>(DIR_Y,0,1,true);
//	isectsTable->CreateRecord<float>(DIR_Z,0,1,true);
	
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
}

void DataController::AddMembershipDataStructures(RIVDataSet<float,ushort>* dataset) {
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
	
	
	
	trueDistributions = rendererData->CreateHistogramSet(bins,tablesToBootstrap);
	
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
//	dirX = currentIntersectionsTable->GetRecord<float>(DIR_X);
//	dirY = currentIntersectionsTable->GetRecord<float>(DIR_Y);
//	dirZ = currentIntersectionsTable->GetRecord<float>(DIR_Z);
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
		
		trueDistributions.AddToHistogram(PIXEL_X, newPath->pixel[0]);
		trueDistributions.AddToHistogram(PIXEL_Y, newPath->pixel[0]);
		trueDistributions.AddToHistogram(PATH_R, std::min(newPath->radiance.r,1.F));
		trueDistributions.AddToHistogram(PATH_G, std::min(newPath->radiance.g,1.F));
		trueDistributions.AddToHistogram(PATH_B, std::min(newPath->radiance.r,1.F));
		trueDistributions.AddToHistogram(THROUGHPUT_R, std::min(newPath->throughput.r,1.F));
		trueDistributions.AddToHistogram(THROUGHPUT_G, std::min(newPath->throughput.g,1.F));
		trueDistributions.AddToHistogram(THROUGHPUT_B, std::min(newPath->throughput.b,1.F));
		trueDistributions.AddToHistogram(DEPTH, nrIntersections);
		
		for(ushort i = 0 ; i < nrIntersections ; ++i) {
			IntersectData& isect = newPath->intersectionData[i];
			
			trueDistributions.AddToHistogram(BOUNCE_NR,(ushort)(i+1));
			trueDistributions.AddToHistogram(POS_X, isect.position[0]);
			trueDistributions.AddToHistogram(POS_Y, isect.position[1]);
			trueDistributions.AddToHistogram(POS_Z, isect.position[2]);
//						trueDistributions.AddToHistogram(DIR_X, isect.dir[0]);
//						trueDistributions.AddToHistogram(DIR_Y, isect.dir[1]);
//						trueDistributions.AddToHistogram(DIR_Z, isect.dir[2]);
			trueDistributions.AddToHistogram(INTERSECTION_R, std::min(isect.color.r,1.F));
			trueDistributions.AddToHistogram(INTERSECTION_G, std::min(isect.color.g,1.F));
			trueDistributions.AddToHistogram(INTERSECTION_B, std::min(isect.color.b,1.F));
			//			trueDistributions.AddToHistogram("depth", nrIntersections);
		}
		//				mutex.lock();
		//				printf("Adding new path.\n");
//		bool tableFull = pathCount >= maxPaths;
		if(currentPathTable->NumberOfRows() < maxPaths) {
			float accept = rand() / (float)RAND_MAX;
			//			printf("Accept prob = %f\n",accept);
			if(accept <= acceptProbability) {
//				++pathCount;
				//			rendererId->AddValue(renderer);
				xPixels->AddValue(newPath->pixel[0]);
				yPixels->AddValue(newPath->pixel[1]);
				//				lensUs->AddValue(newPath->lensU);
				//				lensVs->AddValue(newPath->lensV);
				//				times->AddValue(newPath->timestamp);
				
				colorRs->AddValue(std::min(newPath->radiance.r,1.F));
				colorGs->AddValue(std::min(newPath->radiance.g,1.F));
				colorBs->AddValue(std::min(newPath->radiance.b,1.F));
				throughputRs->AddValue(newPath->throughput.r);
				throughputGs->AddValue(newPath->throughput.g);
				throughputBs->AddValue(newPath->throughput.b);
				depths->AddValue((ushort)newPath->intersectionData.size());
				size_t* indices = new size_t[nrIntersections];
				
				pathsToIsectRef->AddReferences(colorRs->Size() - 1, std::pair<size_t*,ushort>(indices,nrIntersections));
				
				for(int i = 0 ; i < nrIntersections ; ++i) {
					IntersectData& isect = newPath->intersectionData[i];
					
					bounceNrs->AddValue(i+1);
					xs->AddValue(isect.position[0]);
					ys->AddValue(isect.position[1]);
					zs->AddValue(isect.position[2]);
					//					dirX->AddValue(isect.dir[0]);
					//					dirY->AddValue(isect.dir[1]);
					//					dirZ->AddValue(isect.dir[2]);
					isectColorRs->AddValue(std::min(isect.color.r,1.F));
					isectColorGs->AddValue(std::min(isect.color.g,1.F));
					isectColorBs->AddValue(std::min(isect.color.b,1.F));
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
		//		rendererData->Print(10);
		
		//Swap current with candidate
		candidateData->SetDataListeners(rendererData->GetDataListeners());
		RIVDataSet<float,ushort>* temp = rendererData;
		rendererData = candidateData;
		candidateData = temp;
		
		firstTime = false;
		
		resetPointers(rendererData);
	}
	else { //Both are full, join the two sets, bootstrap
		
		//		printf("\nCURRENT DATA = \n");
		//		rendererData->Print(10);
		//
		//		printf("\nCANDIDATE DATA = \n");
		//		candidateData->Print(10);
		
		//Join the two datasets
		rendererData->AddDataSet(candidateData);
		
		printHeader("BOOTSTRAPPING",100);
		
		//		printf("FULL DATA : \n");
		//		rendererData->Print();
		
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
				
				//					printf("Bootstrap = \n\n");
				//					bootstrap->Print();
				
				
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
			
			//			printf("BOOTSTRAP WITHOUT REFERENCES : ");
			//			(bestBootstrap)->Print();
			
			//			printf("\nTRUE HISTOGRAMS = \n");
			//			trueDistributions.Print();
			//
			//			printf("\nBOOTSTRAP HISTOGRAMS = \n");
			//			bootstrapHistograms.Print();
			
			bestBootstrap->SetDataListeners(rendererData->GetDataListeners());
			auto pathsTable = bestBootstrap->GetTable(PATHS_TABLE);
			auto isectsTable = bestBootstrap->GetTable(INTERSECTIONS_TABLE);
			
			//Create new references as they are not created by the bootstrapping
			RIVRecord<ushort>* bootstrapDepth = pathsTable->GetRecord<ushort>(DEPTH);
			auto bootIsectsToPathsRef = new RIVSingleReference(isectsTable,pathsTable);
			auto bootPathsToIsectRef = new RIVMultiReference(pathsTable,isectsTable);
			
			//Create new references as they are not created by the bootstrapping
			pathsTable->SetReference(bootPathsToIsectRef);
			isectsTable->SetReference(bootIsectsToPathsRef);
			
			//Fix the new reference paths
			size_t intersectionsCount = 0;
			size_t pathsCount = 0;
			
			for(size_t i = 0 ; i < bootstrapDepth->Size() ; ++i) {
				ushort depth = bootstrapDepth->Value(i);
				std::pair<size_t*,ushort> rowsMapping;
				size_t* rows = new size_t[depth];
				for(size_t j = 0 ; j < depth ; ++j) {
					rows[j] = intersectionsCount;
					bootIsectsToPathsRef->AddReference(intersectionsCount, pathsCount);
					++intersectionsCount;
				}
				rowsMapping.second = depth;
				rowsMapping.first = rows;
				bootPathsToIsectRef->AddReferences(i, rowsMapping);
				++pathsCount;
			}
			
//						printf("\nBOOTSTRAP RESULT = \n");
//						bestBootstrap->Print();
			
			//Delete the old renderer data and replace it with the bootstrap dataset,
			delete rendererData;
			rendererData = bestBootstrap;
			bestBootstrap = NULL;
			

			rendererData->NotifyDataListeners();
			
			candidateData->ClearData();
			resetPointers(candidateData);	//Reset the shortcut pointers to a new empty dataset
			//			++reduceRounds;
			//			if(reduceRounds % 2 == 0) {
			//			}
		}
		else {
			printf("\n Could not find a better bootstrap... \n");
		}
		
		AddMembershipDataStructures(rendererData);
		
		reporter::stop("bootstrapping");
		Pause();
	}
}
void DataController::SetAcceptProbability(float newProb) {
	acceptProbability = newProb;
}