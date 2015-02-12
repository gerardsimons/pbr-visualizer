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

#include "../Data/Record.h"
#include "../Data/DataSet.h"

#include <ostream>
#include <vector>
#include <algorithm>

class OctreeNode;

class Octree {
private:
	//The outermost dimensions of this octree, each child with depth N has dimensions of d/2^N where d is either width,height or depth
	
	OctreeNode* root;
	OctreeConfig config; //Contains constants like max capacity and max tree depth, is also passed to nodes for recursion
	
    std::vector<size_t> indicesOne;
    RIVRecord<float>* xsOne;
    RIVRecord<float>* ysOne;
    RIVRecord<float>* zsOne;
    RIVRecord<float>* rsOne;
    RIVRecord<float>* gsOne;
    RIVRecord<float>* bsOne;

    std::vector<size_t> indicesTwo;
    RIVRecord<float>* xsTwo;
    RIVRecord<float>* ysTwo;
    RIVRecord<float>* zsTwo;
    RIVRecord<float>* rsTwo;
    RIVRecord<float>* gsTwo;
    RIVRecord<float>* bsTwo;
	
	float* cachedMaxDensity = NULL;
	size_t* cachedDepth = NULL;
	size_t* cachedMaxCapacity = NULL;
    float* cachedMaxEnergyOne = NULL;
    float* cachedMaxEnergyTwo = NULL;
	
	float maxDensityHelper(OctreeNode* node);
	size_t maxCapacityHelper(OctreeNode* node);
    
    float maxEnergyHelperOne(OctreeNode* node);
    float maxEnergyHelperTwo(OctreeNode* node);
	
	void init();
	void checkMinMax(float& currentMin, float& currentMax, float candidate);
public:
	~Octree();
    Octree();
	Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zsOne, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, OctreeConfig& configuration);
	Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zsOne, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, const std::vector<size_t>& indexSubset,  OctreeConfig& configuration);
    
    Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zsOne, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, RIVRecord<float>* xsTwo, RIVRecord<float>* ysTwo, RIVRecord<float>* zsTwo, RIVRecord<float>* rsTwo, RIVRecord<float>* gsTwo, RIVRecord<float>* bsTwo, OctreeConfig& configuration);
    Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zPositions, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, RIVRecord<float>* xsTwo, RIVRecord<float>* ysTwo, RIVRecord<float>* zsTwo, RIVRecord<float>* rsTwo, RIVRecord<float>* gsTwo, RIVRecord<float>* bsTwo, const std::vector<size_t>& indexSubsetOne, const std::vector<size_t>& indexSubsetTwo, OctreeConfig& configuration);

	static bool Test();
	friend std::ostream& operator<<(std::ostream& os, const Octree& tree) {
		os << "Root node : \n" << tree.root;
		return os;
	}
	float MaxDensity();
	size_t MaxCapacity();
	size_t NumberOfPoints();
	size_t NumberOfNodes();
    float MaxEnergyOne();
    float MaxEnergyTwo();
	size_t Depth();
    bool IsEmpty();
	OctreeNode* GetRoot();
	OctreeConfig* GetConfiguration();
};
	



#endif /* defined(__afstuderen_DO_NOT_DELETE__Octree__) */
