//
//  DataController.cpp
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#include "DataController.h"
#include <algorithm>

DataController::DataController() {
	if(updateThrottle == 0) {
		updateThrottle = maxPaths;
	}
	createDataSet();
}

void DataController::createDataSet() {
	
	dataset = new RIVDataSet();
	
	//Path table
	pathTable = new RIVTable("paths");
	
	rendererId = new RIVUnsignedShortRecord("renderer");
	xPixels = new RIVFloatRecord("x");
	yPixels = new RIVFloatRecord("y");
	lensUs = new RIVFloatRecord("lens U");
	lensVs = new RIVFloatRecord("lens V");
	times = new RIVFloatRecord("time");
	colorRs = new RIVFloatRecord("R");
	colorGs = new RIVFloatRecord("G");
	colorBs = new RIVFloatRecord("B");
	throughputRs = new RIVFloatRecord("throughput R");
	throughputGs = new RIVFloatRecord("throughput G");
	throughputBs = new RIVFloatRecord("throughput B");
	depths = new RIVUnsignedShortRecord("depth");
	
	//Throughput?
	pathTable->AddRecord(rendererId);
	pathTable->AddRecord(xPixels);
	pathTable->AddRecord(yPixels);
	pathTable->AddRecord(lensUs);
	pathTable->AddRecord(lensVs);
	pathTable->AddRecord(times);
	pathTable->AddRecord(colorRs);
	pathTable->AddRecord(colorGs);
	pathTable->AddRecord(colorBs);
	pathTable->AddRecord(throughputRs);
	pathTable->AddRecord(throughputGs);
	pathTable->AddRecord(throughputBs);
	pathTable->AddRecord(depths);
	
	isectsTable = new RIVTable("intersections");

	bounceNrs = new RIVUnsignedShortRecord("bounce_nr");
	xs = new RIVFloatRecord("x");
	ys = new RIVFloatRecord("y");
	zs = new RIVFloatRecord("z");
	isectColorRs = new RIVFloatRecord("R");
	isectColorGs = new RIVFloatRecord("G");
	isectColorBs = new RIVFloatRecord("B");
	primitiveIds = new RIVUnsignedShortRecord("primitive ID");
	shapeIds = new RIVUnsignedShortRecord("shape ID");
	interactionTypes = new RIVUnsignedShortRecord("interaction type");
	lightIds = new RIVUnsignedShortRecord("light ID");
	
	isectsTable->AddRecord(bounceNrs);
	isectsTable->AddRecord(xs);
	isectsTable->AddRecord(ys);
	isectsTable->AddRecord(zs);
	isectsTable->AddRecord(isectColorRs);
	isectsTable->AddRecord(isectColorGs);
	isectsTable->AddRecord(isectColorBs);
	isectsTable->AddRecord(primitiveIds);
	isectsTable->AddRecord(shapeIds);
	isectsTable->AddRecord(interactionTypes);
	isectsTable->AddRecord(lightIds);
	
	pathsToIsectRef = new RIVMultiReference(pathTable,isectsTable);
	pathTable->AddReference(pathsToIsectRef);
	isectToPathsRef = new RIVSingleReference(isectsTable,pathTable);
	isectsTable->AddReference(isectToPathsRef);
	
	dataset->AddTable(pathTable);
	dataset->AddTable(isectsTable);
}

RIVDataSet* DataController::GetDataSet() {
	return dataset;
}

void DataController::ProcessNewPath(ushort renderer, PathData* newPath) {
//	printf("New path from renderer #1 received!\n");
	if(!paused) {
		if(rendererId->Size() < maxPaths) {
			size_t nrIntersections = newPath->intersectionData.size();
			if(nrIntersections > 0) {
				
//				printf("Adding new path.\n");
				rendererId->AddValue(renderer);
				xPixels->AddValue(newPath->imageX);
				yPixels->AddValue(newPath->imageY);
				lensUs->AddValue(newPath->lensU);
				lensVs->AddValue(newPath->lensV);
				times->AddValue(newPath->timestamp);
				colorRs->AddValue(std::min(newPath->radiance[0],1.F));
				colorGs->AddValue(std::min(newPath->radiance[1],1.F));
				colorBs->AddValue(std::min(newPath->radiance[2],1.F));
				throughputRs->AddValue(newPath->throughput[0]);
				throughputGs->AddValue(newPath->throughput[1]);
				throughputBs->AddValue(newPath->throughput[2]);
				depths->AddValue((ushort)newPath->intersectionData.size());
				
				size_t* indices = new size_t[nrIntersections];
				
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
					shapeIds->AddValue(isect.shapeId);
					interactionTypes->AddValue(isect.interactionType);
					lightIds->AddValue(isect.lightId);
					
					isectToPathsRef->AddReference(xs->Size() - 1, rendererId->Size() - 1);
					indices[i] = xs->Size() - 1;
				}

				pathsToIsectRef->AddReferences(rendererId->Size() - 1, std::pair<size_t*,ushort>(indices,nrIntersections));
				
//				//Should I notify the dataset listeners?
				if(rendererId->Size() % updateThrottle == 0) {
					dataset->NotifyDataListeners();
				}
			}
		}
		else { //I AM SO FULL, I CANNOT EAT ONE MORE BYTE OF DATA
			paused = true;
			//How to resolve the pause, what do we throw out to create more space?
			dataset->Print(250);
			dataset->NotifyDataListeners();
		}
	}
}

bool DataController::IsPaused() {
	return paused;
}