//
//  dataconnector.cpp
//  embree
//
//  Created by Gerard Simons on 10/11/14.
//
//

#include "dataconnector.h"

#include <exception>
#include <cstddef>
#include <stdexcept>
#include <stdio.h>

DataConnector::DataConnector(callback_function callback) {
	this->callback = callback;
}
//Finish the current path with the latest data
void DataConnector::FinishPath(unsigned short depth, float r, float g, float b, float throughput_r, float throughput_g, float throughput_b) {
	//Find callback
	if(pathSet) {
		currentPath.radiance[0] = r;
		currentPath.radiance[1] = g;
		currentPath.radiance[2] = b;
		
		currentPath.throughput[0] = throughput_r;
		currentPath.throughput[1] = throughput_g;
		currentPath.throughput[2] = throughput_b;
		
//		printf("Finishing Path\n");
		callback(&currentPath);
		pathSet = false;
	}
	else {
		throw std::runtime_error("INVALID STATE: There is no path in the buffer.");
	}
}

void DataConnector::StartPath(float x, float y, float lensU, float lensV, float time) {
	pathSet = true;
//	printf("Starting Path\n");
	currentPath = (PathData){x,y,lensU,lensV,time};
}

void DataConnector::AddIntersectionData(float x, float y, float z, float r, float g, float b, int primitive_id, ushort type) {
//			printf("Instersection added.\n");
//	printf("position = [%f,%f,%f]\n",x,y,z);
//	printf("color = [%f,%f,%f]\n",r,g,b);
//	printf("primitive ID = %d\n",primitive_id);
	if(pathSet) {
//		printf("Add Intersection Data to Path\n");
		IntersectData isectData = IntersectData(x, y, z, r, g, b, primitive_id, 0, 0, 0);
		currentPath.intersectionData.push_back(isectData);
	}
	else throw std::runtime_error("Path is not set.");
}