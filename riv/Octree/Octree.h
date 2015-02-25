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
	
    OctreeNode* root = NULL;
	OctreeConfig config; //Contains constants like max capacity and max tree depth, is also passed to nodes for recursion
    
    std::vector<std::vector<std::vector<OctreeNode*>>> leafNodes;
    
    size_t addCount = 0;
    size_t nDim; //How many nodes in 1 dimension, e.g. 2 ^ depth
    size_t N; //How many nodes in total = 8 ^ depth
    unsigned short depth;
    float minX,maxX;
    float minY,maxY;
    float minZ,maxZ;
	
//	float* cachedMaxDensity = NULL;
//	size_t* cachedDepth = NULL;
//	size_t* cachedMaxCapacity = NULL;
//    float* cachedMaxEnergyOne = NULL;
//    float* cachedMaxEnergyTwo = NULL;
	
//	float maxDensityHelper(OctreeNode* node);
//	size_t maxCapacityHelper(OctreeNode* node);
//    
//    float maxEnergyHelperOne(OctreeNode* node);
//    float maxEnergyHelperTwo(OctreeNode* node);
	
    unsigned short binForValue(float value, float min, float max);
	void init(float cx, float cy, float cz, float size);
	void checkMinMax(float& currentMin, float& currentMax, float candidate);
    void createIndex();
    void createIndexHelper(OctreeNode* node);
    
    float maxValueHelper(OctreeNode* node);
public:
    ~Octree() { delete root; };
    Octree() { };
    //Create a octree of depthxdepthxdepth nodes with center cx,cy,cz and a cubic size of cubicSize
    Octree(unsigned int depth,float cx, float cy, float cz, float cubicSize);
    Octree& operator=(Octree&& other) {
//        std::cout << "copy assignment of A\n";
        std::swap(root, other.root);
        std::swap(depth, other.depth);
        std::swap(minX, other.minX);
        std::swap(maxX, other.maxX);
        std::swap(minY, other.minY);
        std::swap(maxY, other.maxY);
        std::swap(minZ, other.minZ);
        std::swap(maxZ, other.maxZ);
        std::swap(leafNodes, other.leafNodes);
        return *this;
    }
    float MaxValue();
    size_t NodesPerDimension();
    //Find the octree node with this value and add the value to it
    void Add(float x, float y, float z, float value);
    OctreeNode* GetLeafNode(float x, float y, float z);
    OctreeNode* LeafNodeForCoordinates(float x, float y, float z);
//	static bool Test();
//	friend std::ostream& operator<<(std::ostream& os, const Octree& tree) {
//		os << "Root node : \n" << tree.root;
//		return os;
//	}
//	float MaxDensity();
//	size_t MaxCapacity();
//	size_t NumberOfPoints();
//	size_t NumberOfNodes();
//    float MaxEnergyOne();
//    float MaxEnergyTwo();
	ushort Depth();
    bool IsEmpty();
	OctreeNode* GetRoot();
	OctreeConfig* GetConfiguration();
    void Clear();
};
	



#endif /* defined(__afstuderen_DO_NOT_DELETE__Octree__) */
