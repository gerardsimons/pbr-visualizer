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

size_t DataConnector::IdCounter = 0;

void DataConnector::FinishFrame(size_t numPaths, size_t numRays) {
	ffCallback(numPaths,numRays);
}
DataConnector::DataConnector(path_finished pfCallback, frame_finished ffCallback) : pfCallback(pfCallback), ffCallback(ffCallback) {
	id = ++IdCounter;
}
//Finish the current path with the latest data
bool DataConnector::FinishPath(unsigned short depth, const Color& color, const Color& throughput) {
	//Find callback
	if(pathSet) {
		currentPath.radiance = color;
		currentPath.throughput = throughput;
		currentPath.depth = depth;
		
//		printf("Finishing Path\n");
		return pfCallback(&currentPath);
		pathSet = false;
	}
	else {
		throw std::runtime_error("INVALID STATE: There is no path in the buffer.");
	}
}

void DataConnector::StartPath(const Vec2f& pixel,const Vec2f& lens, float time) {
	pathSet = true;
//	printf("Starting Path\n");
	currentPath = (PathData){pixel,lens,time};
}

void DataConnector::AddIntersectionData(const Vec3fa& pos, const Vec3fa& dir, const Color& color, int primitive_id, ushort type) {
//			printf("Instersection added.\n");
//	printf("position = [%f,%f,%f]\n",x,y,z);
//	printf("color = [%f,%f,%f]\n",r,g,b);
//	printf("primitive ID = %d\n",primitive_id);
	if(pathSet) {
//		printf("Add Intersection Data to Path\n");
		IntersectData isectData = IntersectData(pos, dir, color, primitive_id, 0, 0, 0);
		currentPath.intersectionData.push_back(isectData);
	}
	else throw std::runtime_error("Path is not set.");
}