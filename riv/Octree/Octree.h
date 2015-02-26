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
    OctreeNode* root = NULL;
	OctreeConfig config;
    
    float* cachedMax = NULL;
    
    size_t addCount = 0;
    size_t nDim; //How many nodes in 1 dimension, e.g. 2 ^ depth
    size_t N; //How many nodes in total = 8 ^ depth
    float minX,maxX;
    float minY,maxY;
    float minZ,maxZ;
    unsigned short depth;
    unsigned int maxCapacity;
	
    unsigned short binForValue(float value, float min, float max);
	void init(float cx, float cy, float cz, float size);
	void checkMinMax(float& currentMin, float& currentMax, float candidate);
    void createIndex();
    void createIndexHelper(OctreeNode* node);
    
    float maxValueHelper(OctreeNode* node);
public:
    ~Octree() { delete root; };
    Octree() : depth(0), maxCapacity(0) { };
    //Create a octree of depthxdepthxdepth nodes with center cx,cy,cz and a cubic size of cubicSize
    Octree(unsigned int depth, float cx, float cy, float cz, float size, unsigned int maxCapacity);
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
        return *this;
    }
    float MaxValue();
    size_t NodesPerDimension();
    void Add(float x, float y, float z, float value);
    OctreeNode* GetLeafNode(float x, float y, float z);
    OctreeNode* LeafNodeForCoordinates(float x, float y, float z);
	ushort Depth();
    bool IsEmpty();
	OctreeNode* GetRoot();
	OctreeConfig* GetConfiguration();
    void Clear();
};
	



#endif /* defined(__afstuderen_DO_NOT_DELETE__Octree__) */
