//
//  dataconnector.cpp
//  embree
//
//  Created by Gerard Simons on 10/11/14.
//
//

#include "dataconnector.h"

#include <cstddef>
#include <stdio.h>

int DataConnector::ID_COUNTER = 0;

DataConnector::DataConnector(callback_function callback) {
	this->callback = callback;
}

void DataConnector::ProcessPath() {
	//Find callback
	callback(currentPath);
}

void DataConnector::StartPath(float x, float y, float lensU, float lensV, float time) {
	if(currentPath != NULL) {
		//Process finished path
		ProcessPath();
	}
	delete currentPath;
	
//	printf("%d Path started.\n",dc_id);
}

//	void set_callback((void) (*newCallBack)(PathData*)) {
//		callback = newCallBack;
//	}

void DataConnector::AddIntersectionData(float x, float y, float z, float r, float g, float b, int primitive_id) {
	//		printf("Instersection added.\n");
	printf("position = [%f,%f,%f]\n",x,y,z);
	printf("color = [%f,%f,%f]\n",r,g,b);
	printf("primitive ID = %d\n",primitive_id);
	
	
}