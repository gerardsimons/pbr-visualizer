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

DataController::DataController(const int maxPaths, const int maxBootstrapRepeat, const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, size_t nrPrimitives, ushort nrLights) : maxPaths(maxPaths), maxBootstrapRepeat(maxBootstrapRepeat),bootstrapRepeat(maxBootstrapRepeat),maxNrLights(nrLights) {
    createDataStructures(xBounds,yBounds,zBounds,nrPrimitives);
}

RIVDataSet<float,ushort>* DataController::Bootstrap(RIVDataSet<float, ushort>* dataset,const size_t N) {
    
    
    //	const std::string taskName = "Creating bootstrap";
    //	reporter::startTask("Creating bootstrap", N);
    
    RIVDataSet<float,ushort>* bootstrap = dataset->CloneStructure(dataTables);
    RIVTable<float,ushort>* bootstrapPaths = bootstrap->GetTable(PATHS_TABLE);
    RIVTable<float,ushort>* bootstrapIsects = bootstrap->GetTable(INTERSECTIONS_TABLE);
    RIVTable<float,ushort>* bootstrapLights = bootstrap->GetTable(LIGHTS_TABLE);
    
    RIVTable<float,ushort>* paths = dataset->GetTable(PATHS_TABLE);
    RIVTable<float,ushort>* intersections = dataset->GetTable(INTERSECTIONS_TABLE);
    RIVTable<float,ushort>* lights = dataset->GetTable(LIGHTS_TABLE);
    size_t rows = paths->NumberOfRows();
    
    //Give the underlying data structure a chance to reserve space
    bootstrapPaths->ReserveRows(N);
    
    RIVMultiReference* refToIntersections = static_cast<RIVMultiReference*>(paths->GetReferenceTo(INTERSECTIONS_TABLE));
    RIVMultiReference* refToLights = static_cast<RIVMultiReference*>(intersections->GetReferenceTo(LIGHTS_TABLE));
    
    auto& pathRecords = paths->GetRecords();
    auto& isectRecords = intersections->GetRecords();
    auto& lightRecords = lights->GetRecords();

    std::vector<size_t> sampledRows(N);
    for(size_t i = 0 ; i < N ; ++i) {
        //		reporter::update(taskName);
        sampledRows[i] = (rand() % rows);
    }

    tuple_for_each(pathRecords, [&](auto tRecords) {
        for(auto record : tRecords) {
            typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
            auto bootRecord = bootstrapPaths->GetRecord<Type>(record->name);
            
            for(size_t index : sampledRows) {
                bootRecord->AddValue(record->Value(index));
            }
        }
    });
    
    std::vector<size_t> sampledIntersections;
    for(size_t index : sampledRows) {
        const std::pair<size_t*,ushort>& refRows = refToIntersections->GetReferenceRows(index);
        for(int i = 0 ; i < refRows.second ; ++i) {
            size_t intersectionRow = refRows.first[i];
            sampledIntersections.push_back(intersectionRow);
        
            tuple_for_each(isectRecords, [&](auto tRecords) {
                for(auto record : tRecords) {
                    typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
                    auto bootRecord = bootstrapIsects->GetRecord<Type>(record->name);
                    
                    bootRecord->AddValue(record->Value(intersectionRow));
                }
                
        });
        }
    }
    
    tuple_for_each(lightRecords, [&](auto tRecords) {
        for(auto record : tRecords) {
            typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
            auto bootRecord = bootstrapLights->GetRecord<Type>(record->name);
            for(size_t index : sampledIntersections) {
                const std::pair<size_t*,ushort>& refRows = refToLights->GetReferenceRows(index);
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

void DataController::initDataSet(RIVDataSet<float, ushort> *dataset,const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, ushort nrPrimitives) {
    
    RIVTable<float,ushort>* pathTable = dataset->CreateTable(PATHS_TABLE);
    
    pathTable->CreateRecord<float>(PIXEL_X,0,1,true);
    pathTable->CreateRecord<float>(PIXEL_Y,0,1,true);
    pathTable->CreateRecord<float>(PATH_R,0,1,true);
    pathTable->CreateRecord<float>(PATH_G,0,1,true);
    pathTable->CreateRecord<float>(PATH_B,0,1,true);
    pathTable->CreateRecord<float>(THROUGHPUT_R,0,1,true);
    pathTable->CreateRecord<float>(THROUGHPUT_G,0,1,true);
    pathTable->CreateRecord<float>(THROUGHPUT_B,0,1,true);
//    pathTable->CreateRecord<float>(THROUGHPUT_R);
//    pathTable->CreateRecord<float>(THROUGHPUT_G);
//    pathTable->CreateRecord<float>(THROUGHPUT_B);
    pathTable->CreateRecord<ushort>(DEPTH,0,maxDepth,true);
    
    RIVTable<float,ushort>* isectsTable = dataset->CreateTable(INTERSECTIONS_TABLE);
    
    //TODO: Determine this by reading from the renderer settings
    isectsTable->CreateRecord<ushort>(BOUNCE_NR,1,maxDepth,true);
    isectsTable->CreateRecord<float>(POS_X,xBounds[0],xBounds[1],true);
    isectsTable->CreateRecord<float>(POS_Y,yBounds[0],yBounds[1],true);
    isectsTable->CreateRecord<float>(POS_Z,zBounds[0],zBounds[1],true);
    //	isectsTable->CreateRecord<float>(DIR_X,0,1,true);
    //	isectsTable->CreateRecord<float>(DIR_Y,0,1,true);
    //	isectsTable->CreateRecord<float>(DIR_Z,0,1,true);
    
    isectsTable->CreateRecord<float>(INTERSECTION_R,0,1,true);
    isectsTable->CreateRecord<float>(INTERSECTION_G,0,1,true);
    isectsTable->CreateRecord<float>(INTERSECTION_B,0,1,true);
    isectsTable->CreateRecord<ushort>(PRIMITIVE_ID,0,nrPrimitives,true);
    isectsTable->CreateRecord<ushort>(OCCLUDER_COUNT,0,maxNrLights+1,true);
    //	shapeIds = isectsTable->CreateShortRecord("shape ID");
    //	interactionTypes = isectsTable->CreateShortRecord("interaction");
    //	lightIds = isectsTable->CreateShortRecord("light ID");
    
    RIVTable<float,ushort>* lightsTable = dataset->CreateTable(LIGHTS_TABLE);
    lightsTable->CreateRecord<ushort>(OCCLUDER_ID,0,nrPrimitives);
    
    RIVMultiReference* pathsToIsectRef = new RIVMultiReference(pathTable,isectsTable);
    pathTable->AddReference(pathsToIsectRef);
    RIVSingleReference* isectToPathsRef = new RIVSingleReference(isectsTable,pathTable);
    isectsTable->AddReference(isectToPathsRef);
    RIVMultiReference* isectsToLightsRef = new RIVMultiReference(isectsTable,lightsTable);
    isectsTable->AddReference(isectsToLightsRef);
}

void DataController::AddMembershipDataStructures(RIVDataSet<float,ushort>* dataset) {
    auto pathMembershipOneTable = dataset->CreateTable(PATH_MEMBERSHIP_TABLE);
    auto isectMembershipOneTable = dataset->CreateTable(ISECT_MEMBERSHIP_TABLE);
    
    pathMembershipOneTable->CreateRecord<float>(MEMBERSHIP);
    isectMembershipOneTable->CreateRecord<float>(MEMBERSHIP);
}

void DataController::createDataStructures(const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, ushort nrPrimitives) {
    
    currentData = new RIVDataSet<float,ushort>(DATASET_ONE);
    candidateData = new RIVDataSet<float,ushort>(DATASET_ONE);
    
    //Create the records and such for the datasets
    initDataSet(currentData,xBounds,yBounds,zBounds,nrPrimitives);
    initDataSet(candidateData,xBounds,yBounds,zBounds,nrPrimitives);
    
    trueDistributions = currentData->CreateEmptyHistogramSet(bins,histogramTables);
//    trueDistributions.Print();
    
    float xSize = xBounds[1] - xBounds[0];
    float ySize = yBounds[1] - yBounds[0];
    float zSize = zBounds[1] - zBounds[0];
    
    float cX = (xBounds[1] + xBounds[0]) / 2.F;
    float cY = (yBounds[1] + yBounds[0]) / 2.F;
    float cZ = (zBounds[1] + zBounds[0]) / 2.F;
    
    float maxSize = std::max(xSize,std::max(ySize,zSize));
    
    //Because floats are annoying with equality, make sure you over-extend a bit the size of the octree
    int maxDepth = 10;
    int maxCapacity = 1;
    energyDistribution = new Octree(maxDepth,cX,cY,cZ,1.01*maxSize,maxCapacity);
    
    resetPointers(candidateData);
}
Octree* DataController::GetEnergyDistribution() {
    return energyDistribution;
}
void DataController::resetPointers(RIVDataSet<float,ushort>* dataset) {
    currentPathTable = dataset->GetTable(PATHS_TABLE);
    currentIntersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
    currentLightsTable = dataset->GetTable(LIGHTS_TABLE);
    
    isectsToPathsRef = (RIVSingleReference*)currentIntersectionsTable->GetReferenceTo(PATHS_TABLE);
    pathsToIsectRef = (RIVMultiReference*)currentPathTable->GetReferenceTo(INTERSECTIONS_TABLE);
    isectsToLightsRef = (RIVMultiReference*)currentIntersectionsTable->GetReferenceTo(LIGHTS_TABLE);
    
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
    occluderCounts = currentIntersectionsTable->GetRecord<ushort>(OCCLUDER_COUNT);
    
    occluderIds = currentLightsTable->GetRecord<ushort>(OCCLUDER_ID);
}
RIVDataSet<float,ushort>** DataController::GetDataSet() {
    return &currentData;
}
//RIVDataSet<float,ushort>** DataController::GetDataSetTwo() {
//	return &currentDataTwo;
//}
bool DataController::ProcessNewPath(int frame, PathData* newPath) {
    
    //ALWAYS update the histograms
    ushort nrIntersections = newPath->intersectionData.size();
    
//    newPath->throughput *= newPath->intersectionData.size();
    
    trueDistributions.AddToHistogram(PIXEL_X, newPath->pixel[0]);
    trueDistributions.AddToHistogram(PIXEL_Y, newPath->pixel[1]);
    trueDistributions.AddToHistogram(PATH_R, newPath->radiance.r);
    trueDistributions.AddToHistogram(PATH_G, newPath->radiance.g);
    trueDistributions.AddToHistogram(PATH_B, newPath->radiance.b);
    trueDistributions.AddToHistogram(THROUGHPUT_R, newPath->throughput.r);
    trueDistributions.AddToHistogram(THROUGHPUT_G, newPath->throughput.g);
    trueDistributions.AddToHistogram(THROUGHPUT_B, newPath->throughput.b);
    trueDistributions.AddToHistogram(DEPTH, nrIntersections);
    
    for(ushort i = 0 ; i < nrIntersections ; ++i) {
        const IntersectData& isect = newPath->intersectionData[i];
        
        trueDistributions.AddToHistogram(BOUNCE_NR,(ushort)(i+1));
        trueDistributions.AddToHistogram(POS_X, isect.position[0]);
        trueDistributions.AddToHistogram(POS_Y, isect.position[1]);
        trueDistributions.AddToHistogram(POS_Z, isect.position[2]);
        //						trueDistributions.AddToHistogram(DIR_X, isect.dir[0]);
        //						trueDistributions.AddToHistogram(DIR_Y, isect.dir[1]);
        //						trueDistributions.AddToHistogram(DIR_Z, isect.dir[2]);
        trueDistributions.AddToHistogram(INTERSECTION_R, isect.color.r);
        trueDistributions.AddToHistogram(INTERSECTION_G, isect.color.g);
        trueDistributions.AddToHistogram(INTERSECTION_B, isect.color.b);
        trueDistributions.AddToHistogram(PRIMITIVE_ID, isect.primitiveId);
        
        //Add energy data
        energyDistribution->Add(isect.position[0],isect.position[1], isect.position[2], (isect.color.r + isect.color.g + isect.color.b) / 3.F);
    }
    
    if(currentPathTable->NumberOfRows() < maxPaths) {
        float accept = rand() / (float)RAND_MAX;
        //			accept = 0;
        //			printf("Accept prob = %f\n",accept);
        if(accept <= acceptProbability) {
            //				++pathCount;
            //			rendererId->AddValue(renderer);
            xPixels->AddValue(newPath->pixel[0]);
            yPixels->AddValue(newPath->pixel[1]);
            //				lensUs->AddValue(newPath->lensU);
            //				lensVs->AddValue(newPath->lensV);
            //				times->AddValue(newPath->timestamp);
            
            colorRs->AddValue(newPath->radiance.r);
            colorGs->AddValue(newPath->radiance.g);
            colorBs->AddValue(newPath->radiance.b);
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
                isectColorRs->AddValue(isect.color.r);
                isectColorGs->AddValue(isect.color.g);
                isectColorBs->AddValue(isect.color.b);
                primitiveIds->AddValue(isect.primitiveId);
                //					shapeIds->AddValue(isect.shapeId);
                //					interactionTypes->AddValue(isect.interactionType);
                //					lightIds->AddValue(isect.lightId);
                
                size_t isectIndex = bounceNrs->Size() - 1;
                size_t nrLights = isect.occluderIds.size();
                size_t* lightsReferenceRows = new size_t[nrLights];
                occluderCounts->AddValue(nrLights);
                for(int j = 0 ; j < nrLights ; ++j) {
//                    printf("isect to occluder : %zu --> %zu\n",j,)
                    occluderIds->AddValue(isect.occluderIds[j]);
                    lightsReferenceRows[j] = occluderIds->Size() - 1;
                }
                isectsToLightsRef->AddReferences(isectIndex, std::pair<size_t*,ushort>(lightsReferenceRows,nrLights));
                isectsToPathsRef->AddReference(isectIndex, colorRs->Size() - 1);
                indices[i] = isectIndex;
            }
            return true;
        }
    }
    else { //I AM SO FULL, I CANNOT EAT ONE MORE BYTE OF DATA
        return false;
    }
    return true;
}

void DataController::Reduce() {
    printHeader("DATA REDUCTION",100);
    const std::string taskName = "data reduction";
    reporter::startTask(taskName);
    if(firstTime) {
        //		printf("\n First time...\n");
        
//        candidateData->Print();
        std::swap(currentData, candidateData);
        //Swap current with candidate
        currentData->SetDataListeners(candidateData->GetDataListeners());
        candidateData->CopyFiltersTo(currentData);
        
        //Divide the max paths and accept_prob by two (the other half is reserved for the bootstrapped dataset)
        firstTime = false;
        maxPaths /= 2;
        acceptProbability /= 2;
        
        currentData->NotifyDataListeners();
    }
    else {
        printf("Score to beat = %f\n",bestBootstrapResult);
    }

    RIVDataSet<float,ushort>* joinedData = currentData->CloneStructure(dataTables);
    
    auto joinedPathsTable = joinedData->GetTable(PATHS_TABLE);
    auto joinedIsectsTable = joinedData->GetTable(INTERSECTIONS_TABLE);
    auto joinedLightsTable = joinedData->GetTable(LIGHTS_TABLE);
    
    //TODO: Move this reference stuff to clone structure?
    joinedPathsTable->AddReference(new RIVMultiReference(joinedPathsTable,joinedIsectsTable));
    joinedIsectsTable->AddReference(new RIVSingleReference(joinedIsectsTable,joinedPathsTable));
    joinedIsectsTable->AddReference(new RIVMultiReference(joinedIsectsTable,joinedLightsTable));
    
    //	printf("\nCURRENT DATA = \n");
    //	currentData->Print();
    //
    //	printf("\nCANDIDATE DATA = \n");
    //	candidateData->Print();
    
    //Join the two datasets
    joinedData->AddDataSet(currentData);
    joinedData->AddDataSet(candidateData);
    
//    	printf("JOINED DATA : \n");
//    	joinedData->Print();
    
    HistogramSet<float,ushort> bootstrapHistograms;
    
    bool newBootstrapFound = false;
    float bootstrapScoreAverage = 0;
    
    //Bootstrap set
    RIVDataSet<float,ushort>* bestBootstrap = NULL;
    
//    	printf("\nTRUE HISTOGRAMS = \n");
//    	trueDistributions.Print();
    
    for(int i = 0 ; i < bootstrapRepeat ; ++i) {
        //			printf("Round #%d\n",i);
        auto bootstrap = Bootstrap(joinedData, maxPaths);
        bootstrapHistograms = bootstrap->CreateHistogramSet(bins);
        
//        printf("TRUE DISTRIBUTIONS:\n");
//        trueDistributions.Print();
//        
//        printf("BOOTSTRAP HISTOGRAMS:\n");
//        bootstrapHistograms.Print();
        

        float score = trueDistributions.DistanceTo(bootstrapHistograms);
        
//        printf("Bootstrap = \n\n");
//        bootstrap->Print(100);
        
        printf("Bootstrap #%d score = %f\n",i,score);
        bootstrapScoreAverage += score;
        
        if(bestBootstrapResult < 0 || score < bestBootstrapResult) {
            
            printf("BETTER BOOTSTRAP FOUND! SCORE = %f\n",score);
            if(bestBootstrap) { //A new and better bootstrap was found, delete the old bestBootstrap
                //					printf("Delete previous best bootstrap\n");
                delete bestBootstrap;
            }
            
            bestBootstrap = bootstrap;
            
//            				printf("\nTRUE HISTOGRAMS = \n");
//            				trueDistributions.Print();
//            
//            				printf("\nBOOTSTRAP HISTOGRAMS = \n");
//            				bootstrapHistograms.Print();
            
            bestBootstrapResult = score;
            newBootstrapFound = true;
        }
        else { //This bootstrap is not a better fit than the previous one, delete it
            delete bootstrap;
        }
        bootstrap = NULL;
    }
    
    printf("Average bootstrap score = %f\n",bootstrapScoreAverage/bootstrapRepeat);
    
    if(newBootstrapFound) {
        
        printf("\n\n***** NEW BOOTSTRAP FOUND! ***** \n\n");
        
        //						printf("BOOTSTRAP WITHOUT REFERENCES : ");
        //						(bestBootstrap)->Print();
        
        //			printf("\nTRUE HISTOGRAMS = \n");
        //			trueDistributions.Print();
        //
        //					printf("\nBOOTSTRAP HISTOGRAMS = \n");
        //					bootstrapHistograms.Print();
        
        bestBootstrap->SetDataListeners(currentData->GetDataListeners());
        auto pathsTable = bestBootstrap->GetTable(PATHS_TABLE);
        auto isectsTable = bestBootstrap->GetTable(INTERSECTIONS_TABLE);
        auto lightsTable = bestBootstrap->GetTable(LIGHTS_TABLE);
        
        //Create new references as they are not created by the bootstrapping
        RIVRecord<ushort>* bootstrapDepth = pathsTable->GetRecord<ushort>(DEPTH);
        RIVRecord<ushort>* bootstrapOccluderCounts = isectsTable->GetRecord<ushort>(OCCLUDER_COUNT);
        auto bootIsectsToPathsRef = new RIVSingleReference(isectsTable,pathsTable);
        auto bootPathsToIsectRef = new RIVMultiReference(pathsTable,isectsTable);
        auto bootIsectsToLightsRef = new RIVMultiReference(isectsTable,lightsTable);
        
        //Create new references as they are not created by the bootstrapping
        pathsTable->AddReference(bootPathsToIsectRef);
        isectsTable->AddReference(bootIsectsToPathsRef);
        isectsTable->AddReference(bootIsectsToLightsRef);
        
        //Fix the new reference paths
        size_t intersectionsCount = 0;
        size_t pathsCount = 0;
        size_t occluderCount = 0;
        
        size_t pathRows = pathsTable->NumberOfRows();
        for(size_t i = 0 ; i < pathRows ; ++i) {
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
        intersectionsCount = 0;
        size_t intersectionRows = isectsTable->NumberOfRows();
        for(size_t i = 0 ; i < intersectionRows ; ++i) {
            ushort depth = bootstrapOccluderCounts->Value(i);
            std::pair<size_t*,ushort> rowsMapping;
            size_t* rows = new size_t[depth];
            for(size_t j = 0 ; j < depth ; ++j) {
                rows[j] = occluderCount;
                ++occluderCount;
            }
            rowsMapping.second = depth;
            rowsMapping.first = rows;
            bootIsectsToLightsRef->AddReferences(i, rowsMapping);
            ++intersectionsCount;
        }
//
        
//        printf("\nBOOTSTRAP RESULT = \n");
//        bestBootstrap->Print();
        
        //If we found a better bootstrap, this might mean we need to do more bootstrapping until
//        bootstrapRepeat *= 2;
//        if(bootstrapRepeat > maxBootstrapRepeat) {
//            bootstrapRepeat = maxBootstrapRepeat;
//        }
        
        //Delete the old renderer data and replace it with the bootstrap dataset,
        std::swap(currentData,bestBootstrap);
        bestBootstrap->CopyFiltersTo(currentData);
        delete bestBootstrap;
        bestBootstrap = NULL;
        
        resetPointers(candidateData);	//Reset the shortcut pointers to a new empty dataset
        currentData->NotifyDataListeners();
    }
    else {
        printf("\n Could not find a better bootstrap... \n");
        
        //Decrease the number of bootstrap repeats, to a minimum of 1
        bootstrapRepeat /= 2;
        if(!bootstrapRepeat) {
            bootstrapRepeat = 1;
        }
        //			resetPointers(candidateData);	//Reset the shortcut pointers to a new empty dataset
        //			printf("EMPTY CANDIDATE DATA: \n");
        //			candidateData->Print();
    }
    
    
    
    delete joinedData;
    
    reporter::stop(taskName);
    
}
void DataController::SetAcceptProbability(float newProb) {
    acceptProbability = newProb;
}
void DataController::Reset() {
//    printf("DataController was reset...\n");
//    trueDistributions = currentData->CreateHistogramSetFromFiltered(bins,histogramTables);
//    printf("Filtered true distributions:\n");
//    trueDistributions.Print();
    
    trueDistributions = candidateData->CreateEmptyHistogramSet(bins,histogramTables);
    //    trueDistributions.Print();
    currentData->ClearData();
    candidateData->ClearData();
    resetPointers(candidateData);
    bootstrapRepeat = maxBootstrapRepeat;
    
    firstTime = true;
    acceptProbability *= 2;
    maxPaths *= 2;
    bestBootstrapResult = std::numeric_limits<float>::max();
}
void DataController::SetMaxPaths(int maxPaths) {
    this->maxPaths = maxPaths;
}