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

//#define DEBUG_STR

size_t DataConnector::IdCounter = 0;

void DataConnector::FinishFrame(size_t numPaths, size_t numRays) {
	ffCallback(numPaths,numRays);
}
DataConnector::DataConnector(path_finished pfCallback, frame_finished ffCallback) : pfCallback(pfCallback), ffCallback(ffCallback) {
	id = ++IdCounter;
}
//Finish the current path with the latest data
bool DataConnector::FinishPath(const Color& color, const Color& throughput) {
	//Find callback
#ifdef DEBUG_STR
    printf("Finish path\n\n");
#endif
	if(pathSet) {
        currentLightData.clear();
        //Some preprocessing
        currentPath.radiance = color;
//        currentPath.radiance.r = std::min(color.r,1.F);
//        currentPath.radiance.g = std::min(color.g,1.F);
//        currentPath.radiance.b = std::min(color.b,1.F);
        
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
    #ifdef DEBUG_STR
	printf("Starting Path\n");
    #endif
	currentPath = (PathData){pixel,lens,time};
//    printf("Start path\n");
}

void DataConnector::AddIntersectionData(const Vec3fa& pos, const Vec3fa& dir,const Color& color, int primitive_id, ushort type) {
    #ifdef DEBUG_STR
    printf("Intersection added.\n");
#endif
//	printf("position = [%f,%f,%f]\n",x,y,z);
//	printf("color = [%f,%f,%f]\n",r,g,b);
//	printf("primitive ID = %d\n",primitive_id);
//    printf("Add intersection data\n");
    
    
	if(pathSet) {
//		printf("Add Intersection Data to Path\n");
//        color.r = std::min(color.r,1.F);
//        color.g = std::min(color.g,1.F);
//        color.b = std::min(color.b,1.F);
//        if(color.r + color.g + color.b > 5) {
//            
//        }
		currentIntersect = IntersectData(pos, dir, color, primitive_id, 0, 0, type);
        FinishIntersection();
//        currentLightData.clear();
	}
	else throw std::runtime_error("Path is not set.");
}
void DataConnector::AddLightData(ushort lightId, ushort occluderId, const embree::Color &color) {
//    currentPath.intersectionData.back().lightData.push_back((LightData){lightId,occluderId,color});
    #ifdef DEBUG_STR
    printf("Add light data\n");
    #endif
    currentLightData.push_back((LightData){lightId,occluderId,color});
}
void DataConnector::FinishIntersection() {
//    printf("Finish intersection\n");
//    currentLightData.reserve(nrLights);
//    ushort intersectionNumber = currentPath.intersectionData.size();
//    ushort endLight = currentLightData.size() - nrLights * intersectionNumber;
//    ushort startLight = currentLightData.size() - nrLights * (intersectionNumber + 1);
//    for(int i = startLight ; i < endLight ; ++i) {
//        currentIntersect.lightData.push_back(currentLightData[i]);
//    }
    currentIntersect.lightData = currentLightData;
    currentPath.intersectionData.insert(currentPath.intersectionData.begin(),currentIntersect);
}