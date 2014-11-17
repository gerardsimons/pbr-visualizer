//
//  DataController.cpp
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#include "DataController.h"

DataController::DataController(RIVDataSet* dataset) : dataset(dataset) {
	createDataSet();
}

void DataController::createDataSet() {
	
	//Path table
	pathTable = new RIVTable("path");
	
	RIVFloatRecord* xPixels = new RIVFloatRecord("x");
	RIVFloatRecord* yPixels = new RIVFloatRecord("y");
	RIVFloatRecord* lensUs = new RIVFloatRecord("lens U");
	RIVFloatRecord* lensVs = new RIVFloatRecord("lens V");
	RIVFloatRecord* time = new RIVFloatRecord("time");
	RIVFloatRecord* colorR = new RIVFloatRecord("R");
	RIVFloatRecord* colorG = new RIVFloatRecord("G");
	RIVFloatRecord* colorB = new RIVFloatRecord("B");
	
	//Throughput?
	
	isectsTable = new RIVTable("intersections");
}