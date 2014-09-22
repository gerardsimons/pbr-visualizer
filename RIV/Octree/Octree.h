//
//  Octree.h
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_DO_NOT_DELETE__Octree__
#define __afstuderen_DO_NOT_DELETE__Octree__


#include "Octree.h"
#include "OctreeNode.h"
#include "OctreeConfig.h"

#include <ostream>
#include <vector>
#include <algorithm>



class OctreeNode;

class Octree {
private:
	//The outermost dimensions of this octree, each child with depth N has dimensions of d/2^N where d is either width,height or depth
	
	OctreeNode* root;
	OctreeConfig config; //Contains constants like max capacity and max tree depth, is also passed to nodes for recursion
	
	std::vector<float>* xPositions;
	std::vector<float>* yPositions;
	std::vector<float>* zPositions;
	
	float* cachedMaxDensity = NULL;
	size_t* cachedDepth = NULL;
	size_t* cachedMaxCapacity = NULL;
	
	float maxDensityHelper(OctreeNode* node);
	size_t maxCapacityHelper(OctreeNode* node);
	
	void init();
	void checkMinMax(float& currentMin, float& currentMax, float candidate);
public:
	~Octree();
	Octree(std::vector<float>* xPositions, std::vector<float>* yPositions, std::vector<float>* zPositions,OctreeConfig& configuration);
	Octree(std::vector<float>* xPositions, std::vector<float>* yPositions, std::vector<float>* zPositions, const std::vector<size_t>& indexSubset, OctreeConfig& configuration);
	//A free wheeling sandbox type test function, includes massive amounts of #YOLO type endeavours
	static bool Test();
	friend std::ostream& operator<<(std::ostream& os, const Octree& tree) {
		os << "Root node : \n" << tree.root;
		return os;
	}
	float MaxDensity();
	size_t MaxCapacity();
	size_t NumberOfPoints();
	size_t NumberOfNodes();
	size_t Depth();
	OctreeNode* GetRoot();
	OctreeConfig* GetConfiguration();
};
	



#endif /* defined(__afstuderen_DO_NOT_DELETE__Octree__) */
