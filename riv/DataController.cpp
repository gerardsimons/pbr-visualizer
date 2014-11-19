//
//  DataController.cpp
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#include "DataController.h"

DataController::DataController() {
	createDataSet();
}

void DataController::createDataSet() {
	
	dataset = new RIVDataSet();
	
	//Path table
	pathTable = new RIVTable("path");
	
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
				colorRs->AddValue(newPath->radiance[0]);
				colorGs->AddValue(newPath->radiance[1]);
				colorBs->AddValue(newPath->radiance[2]);
				throughputRs->AddValue(newPath->throughput[0]);
				throughputGs->AddValue(newPath->throughput[1]);
				throughputBs->AddValue(newPath->throughput[2]);
				depths->AddValue((ushort)newPath->intersectionData.size());
				
				size_t* indices = new size_t[nrIntersections];
				
				for(int i = 0 ; i < nrIntersections ; ++i) {
					IntersectData& isect = newPath->intersectionData[i];
					
					xs->AddValue(isect.position[0]);
					ys->AddValue(isect.position[1]);
					zs->AddValue(isect.position[2]);
					isectColorRs->AddValue(isect.color[0]);
					isectColorGs->AddValue(isect.color[1]);
					isectColorBs->AddValue(isect.color[2]);
					primitiveIds->AddValue(isect.primitiveId);
					shapeIds->AddValue(isect.shapeId);
					interactionTypes->AddValue(isect.interactionType);
					lightIds->AddValue(isect.lightId);
					
					isectToPathsRef->AddReference(xs->Size() - 1, rendererId->Size() - 1);
					indices[i] = rendererId->Size();
				}

				pathsToIsectRef->AddReferences(rendererId->Size() - 1, std::pair<size_t*,ushort>(indices,nrIntersections));
				
//				//Should I notify the dataset listeners?
//				if(rendererId->Size() % updateThrottle == 0) {
//					dataset->NotifyDataListeners();
//				}
			}
		}
		else { //I AM SO FULL, I CANNOT EAT ONE MORE BYTE OF DATA
			paused = true;
			//How to resolve the pause, what do we throw out to create more space?
			dataset->Print(100);
			dataset->NotifyDataListeners();
		}
	}
}