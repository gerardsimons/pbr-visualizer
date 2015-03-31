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

DataController::DataController(const int maxPaths, const int maxBootstrapRepeat, const Vec2f& xBounds, const Vec2f& yBounds, const Vec2f& zBounds, size_t nrPrimitives, ushort nrLights,ushort imageWidth, ushort imageHeight, ushort maxDepth) : maxPaths(maxPaths), maxBootstrapRepeat(maxBootstrapRepeat),bootstrapRepeat(maxBootstrapRepeat),maxNrLights(nrLights), imageWidth(imageWidth),imageHeight(imageHeight), maxDepth(maxDepth) {
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
    RIVFixedReference* refToLights = static_cast<RIVFixedReference*>(intersections->GetReferenceTo(LIGHTS_TABLE));
    
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
        std::vector<size_t> refRows = refToIntersections->GetReferenceRows(index);
        for(int i = 0 ; i < refRows.size() ; ++i) {
            size_t intersectionRow = refRows[i];
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
                std::vector<size_t> refRows = refToLights->GetReferenceRows(index);
                //                printArray(refRows.rows,refRows.size);
                for(int i = 0 ; i < refRows.size() ; ++i) {
                    bootRecord->AddValue(record->Value(refRows[i]));
                }
                
            }
        }
    });
//    bootstrap->Print();
    
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
    pathTable->CreateRecord<ushort>(DEPTH,0,maxDepth+1,true);
    
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
    interactionTypes = isectsTable->CreateRecord<ushort>(INTERACTION_TYPE,0,17,true);
    
    RIVTable<float,ushort>* lightsTable = dataset->CreateTable(LIGHTS_TABLE);
    lightsTable->CreateRecord<ushort>(LIGHT_ID,0,maxNrLights,true);
    lightsTable->CreateRecord<ushort>(OCCLUDER_ID,0,(ushort)-1,true);
    lightsTable->CreateRecord<float>(LIGHT_R,0,1,true);
    lightsTable->CreateRecord<float>(LIGHT_G,0,1,true);
    lightsTable->CreateRecord<float>(LIGHT_B,0,1,true);
    
    pathTable->AddReference(new RIVMultiReference(pathTable,isectsTable));
    isectsTable->AddReference(new RIVSingleReference(isectsTable,pathTable));
    isectsTable->AddReference(new RIVFixedReference(isectsTable,lightsTable,maxNrLights));
    lightsTable->AddReference(new RIVSingleReference(lightsTable,isectsTable));
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
    //    currentData->Print(100);
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
    energyDistribution3D = new Octree(maxDepth,cX,cY,cZ,1.01*maxSize,maxCapacity);
    float pixelsPerBins = 1;
    int xBins = imageWidth / pixelsPerBins;
    int yBins = imageHeight / (float)imageWidth * xBins;
    
    Histogram2D<float> pixelThroughput = Histogram2D<float>(IMAGE_THROUGHPUT,0,1,xBins,yBins);
    Histogram2D<float> energyDistribution2D = Histogram2D<float>(IMAGE_RADIANCE,0,1,xBins,yBins);
    Histogram2D<float> depthDistribution = Histogram2D<float>(IMAGE_DEPTH,0,1,xBins,yBins);
    
    imageDistributions.AddHistogram(IMAGE_THROUGHPUT, pixelThroughput);
    imageDistributions.AddHistogram(IMAGE_RADIANCE, energyDistribution2D);
    imageDistributions.AddHistogram(IMAGE_DEPTH, depthDistribution);
    
    resetPointers(candidateData);
}
Octree* DataController::GetEnergyDistribution3D() {
    return energyDistribution3D;
}
//Histogram2D<float>* DataController::GetEnergyDistribution2D() {
//    return &energyDistribution2D;
//}
void DataController::resetPointers(RIVDataSet<float,ushort>* dataset) {
    currentPathTable = dataset->GetTable(PATHS_TABLE);
    currentIntersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
    currentLightsTable = dataset->GetTable(LIGHTS_TABLE);
    
    isectsToPathsRef = (RIVSingleReference*)currentIntersectionsTable->GetReferenceTo(PATHS_TABLE);
    pathsToIsectRef = (RIVMultiReference*)currentPathTable->GetReferenceTo(INTERSECTIONS_TABLE);
    isectsToLightsRef = (RIVFixedReference*)currentIntersectionsTable->GetReferenceTo(LIGHTS_TABLE);
    lightsToIsectsRef = (RIVSingleReference*)currentLightsTable->GetReferenceTo(INTERSECTIONS_TABLE);
    
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
    //
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
    interactionTypes = currentIntersectionsTable->GetRecord<ushort>(INTERACTION_TYPE);
    primitiveIds = currentIntersectionsTable->GetRecord<ushort>(PRIMITIVE_ID);
    occluderCounts = currentIntersectionsTable->GetRecord<ushort>(OCCLUDER_COUNT);
    
    lightIds = currentLightsTable->GetRecord<ushort>(LIGHT_ID);
    occluderIds = currentLightsTable->GetRecord<ushort>(OCCLUDER_ID);
    lightRs = currentLightsTable->GetRecord<float>(LIGHT_R);
    lightGs = currentLightsTable->GetRecord<float>(LIGHT_G);
    lightBs = currentLightsTable->GetRecord<float>(LIGHT_B);
}
RIVDataSet<float,ushort>** DataController::GetDataSet() {
    return &currentData;
}
bool DataController::ProcessNewPath(int frame, PathData* newPath) {
    
    if(mode == NONE) {
        return false;
    }
    
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
        
        //Add energy data to octree
        energyDistribution3D->Add(isect.position[0],isect.position[1], isect.position[2], (isect.color.r + isect.color.g + isect.color.b) / 3.F);
        //        energyDistribution->Add(isect.position[0],isect.position[1], isect.position[2], (i+1));
    }
    float pixelX = newPath->pixel[0];
    float pixelY = newPath->pixel[1];
    
//    unsigned int averageThroughput = (newPath->throughput.r + newPath->throughput.g + newPath->throughput.b) / 3.F * 100 * nrIntersections;
    unsigned int averageThroughput = (newPath->throughput.r + newPath->throughput.g + newPath->throughput.b) / 3.F * 100;
    unsigned int averageEnergy = (newPath->radiance.r + newPath->radiance.g + newPath->radiance.b) / 3.F * 100;
    //    unsigned int averageEnergy = (newPath->radiance.r * newPath->throughput.r + newPath->radiance.g * newPath->throughput.g + newPath->radiance.b * newPath->throughput.b) / 3.F * 100;
    
    imageDistributions.AddToHistogram(IMAGE_THROUGHPUT, pixelX,pixelY,averageThroughput);
    imageDistributions.AddToHistogram(IMAGE_RADIANCE, pixelX,pixelY,averageEnergy);
    imageDistributions.AddToHistogram(IMAGE_DEPTH, pixelX, pixelY,newPath->intersectionData.size());
    
//    pixelThroughput.Add(pixelX, pixelY, averageThroughput);
//    energyDistribution2D.Add(pixelX, pixelY,averageEnergy);
    
    //    printf("Addeding energy %f to %d,%d\n",averageEnergy,(int)pixelX,(int)pixelY);
    //    energyDistribution2D.PrintRaw();
    
    if(mode == ALL && currentPathTable->NumberOfRows() < maxPaths) {
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
            std::vector<size_t> indices(nrIntersections);
            
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
                interactionTypes->AddValue(isect.interactionType);
                //					lightIds->AddValue(isect.lightId);
                
                size_t isectIndex = bounceNrs->Size() - 1;
                size_t nrLights = isect.lightData.size();
                //                size_t* lightsReferenceRows = new size_t[nrLights];
                occluderCounts->AddValue(nrLights);
                for(int j = 0 ; j < nrLights ; ++j) {
                    //                    printf("isect to occluder : %zu --> %zu\n",j,)
                    const LightData& lightD = isect.lightData[j];
                    //                    printf("occluder id = %d\n",lightD.occluderId);
                    occluderIds->AddValue(lightD.occluderId);
                    lightIds->AddValue(lightD.lightId);
                    lightRs->AddValue(lightD.radiance.r);
                    lightGs->AddValue(lightD.radiance.g);
                    lightBs->AddValue(lightD.radiance.b);
                    
                    lightsToIsectsRef->AddReference(lightRs->Size() - 1, isectColorBs->Size() - 1);
                    //                    lightsReferenceRows[j] = occluderIds->Size() - 1;
                }
                //                isectsToLightsRef->AddReferences(isectIndex, std::pair<size_t*,ushort>(lightsReferenceRows,nrLights));
                isectsToPathsRef->AddReference(isectIndex, colorRs->Size() - 1);
                indices[i] = isectIndex;
            }
            pathsToIsectRef->AddReferences(colorRs->Size() - 1, indices);
            return true;
        }
    }
    else {
        return false;
    }
    return true;
}

void DataController::Reduce() {
    
    if(mode == ALL) {
        
        printHeader("DATA REDUCTION",100);
        const std::string taskName = "data reduction";
        reporter::startTask(taskName);
        if(firstTime) {
            //		printf("\n First time...\n");
            
            //        candidateData->Print(100);
            std::swap(currentData, candidateData);
            //Swap current with candidate
            currentData->SetDataListeners(candidateData->GetDataListeners());
            candidateData->CopyFiltersTo(currentData);
            
            //Divide the max paths and accept_prob by two (the other half is reserved for the bootstrapped dataset)
            firstTime = false;
            maxPaths /= 2;
            acceptProbability /= 2;
            
            currentData->NotifyDataListeners();
//            currentData->Print();
            
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
        joinedIsectsTable->AddReference(new RIVFixedReference(joinedIsectsTable,joinedLightsTable,maxNrLights));
        joinedLightsTable->AddReference(new RIVSingleReference(joinedLightsTable,joinedIsectsTable));
        
        //    	printf("\nCURRENT DATA = \n");
        //    	currentData->Print();
        //
        //	printf("\nCANDIDATE DATA = \n");
        //	candidateData->Print();
        
        //Join the two datasets
        joinedData->AddDataSet(currentData);
        joinedData->AddDataSet(candidateData);
        
        //    printf("JOINED DATA : \n");
        //    joinedData->Print();
        
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
            //        RIVRecord<ushort>* bootstrapOccluderCounts = isectsTable->GetRecord<ushort>(OCCLUDER_COUNT);
            auto bootIsectsToPathsRef = new RIVSingleReference(isectsTable,pathsTable);
            auto bootPathsToIsectRef = new RIVMultiReference(pathsTable,isectsTable);
            auto bootIsectsToLightsRef = new RIVFixedReference(isectsTable,lightsTable,maxNrLights);
            auto bootLightsToIsectsRef = new RIVSingleReference(lightsTable,isectsTable);
            
            //Create new references as they are not created by the bootstrapping
            pathsTable->AddReference(bootPathsToIsectRef);
            isectsTable->AddReference(bootIsectsToPathsRef);
            isectsTable->AddReference(bootIsectsToLightsRef);
            lightsTable->AddReference(bootLightsToIsectsRef);
            
            //Fix the new reference paths
            size_t intersectionsCount = 0;
            size_t pathsCount = 0;
            size_t lightsCount = 0;
            
            size_t pathRows = pathsTable->NumberOfRows();
            for(size_t i = 0 ; i < pathRows ; ++i) {
                ushort depth = bootstrapDepth->Value(i);
                std::vector<size_t> rows(depth);
                for(size_t j = 0 ; j < depth ; ++j) {
                    rows[j] = intersectionsCount;
                    bootIsectsToPathsRef->AddReference(intersectionsCount, pathsCount);
                    ++intersectionsCount;
                }
                bootPathsToIsectRef->AddReferences(i,rows);
                ++pathsCount;
            }
            
            intersectionsCount = 0;
            size_t lightRows = lightsTable->NumberOfRows();
            for(size_t i = 0 ; i < lightRows ; ++i) {
                //            for(int j = 0 ; j < maxNrLights ; ++j) {
                bootLightsToIsectsRef->AddReference(i, intersectionsCount);
                if(i % maxNrLights == (maxNrLights - 1)) {
                    ++intersectionsCount;
                }
                //            }
            }
            
            /*
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
             */
            //
            
            //        printf("\nBOOTSTRAP RESULT = \n");
            //        bestBootstrap->Print();
            
            
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
    
}
//Histogram2D<float>* DataController::GetPixelThroughputDistribution() {
//    return &pixelThroughput;
//}
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
void DataController::CycleDataCollectionMode() {
    printf("DataController will update ");
    switch (mode) {
        case ALL:
            mode = DISTRIBUTIONS;
            printf("DISTRIBUTIONS");
            break;
        case DISTRIBUTIONS:
            mode = NONE;
            printf("NONE");
            break;
        case NONE:
            mode = ALL;
            printf("ALL");
            break;
    }
    printf("\n");
}