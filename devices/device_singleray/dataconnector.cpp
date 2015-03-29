//
//  dataconnector.cpp
//  embree
//
//  Created by Gerard Simons on 10/11/14.
//
//

#include "dataconnector.h"

#include <algorithm>
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
bool DataConnector::FinishPath(Color& color, Color& throughput) {
	//Find callback
//    printf("Finish path\n\n");
	if(pathSet) {
        currentLightData.clear();
        //Some preprocessing
        currentPath.radiance.r = std::min(color.r,1.F);
        currentPath.radiance.g = std::min(color.g,1.F);
        currentPath.radiance.b = std::min(color.b,1.F);
        
        currentPath.throughput.r = std::min(throughput.r,1.F);
        currentPath.throughput.g = std::min(throughput.g,1.F);
        currentPath.throughput.b = std::min(throughput.b,1.F);
        
		return pfCallback(&currentPath);
	}
	else {
		throw std::runtime_error("INVALID STATE: There is no path in the buffer.");
	}
}
void DataConnector::StartPath(const Vec2f& pixel,const Vec2f& lens, float time) {
	pathSet = true;
//	printf("Starting Path\n");
	currentPath = (PathData){pixel,lens,time};
//    printf("Start path\n");
}

void DataConnector::AddIntersectionData(const Vec3fa& pos, const Vec3fa& dir, Color& color, int primitive_id, ushort type) {
//			printf("Instersection added.\n");
//	printf("position = [%f,%f,%f]\n",x,y,z);
//	printf("color = [%f,%f,%f]\n",r,g,b);
//	printf("primitive ID = %d\n",primitive_id);
//    printf("Add intersection data\n");
	if(pathSet) {
//		printf("Add Intersection Data to Path\n");
        color.r = std::min(color.r,1.F);
        color.g = std::min(color.g,1.F);
        color.b = std::min(color.b,1.F);
		IntersectData isectData = IntersectData(pos, dir, color, primitive_id, 0, 0, 0);
        isectData.lightData.reserve(nrLights);
        ushort intersectionNumber = currentPath.intersectionData.size();
        ushort endLight = currentLightData.size() - nrLights * intersectionNumber;
        ushort startLight = currentLightData.size() - nrLights * (intersectionNumber + 1);
        for(int i = startLight ; i < endLight ; ++i) {
            isectData.lightData.push_back(currentLightData[i]);
        }
        if(isectData.lightData.size() != nrLights) {
            
        }
		currentPath.intersectionData.insert(currentPath.intersectionData.begin(),isectData);
//        currentLightData.clear();
	}
	else throw std::runtime_error("Path is not set.");
}
void DataConnector::AddLightData(ushort lightId, ushort occluderId, const embree::Color &color) {
//    currentPath.intersectionData.back().lightData.push_back((LightData){lightId,occluderId,color});
//    printf("Add light data\n");
    currentLightData.push_back((LightData){lightId,occluderId,color});
}