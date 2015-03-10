//
//  OctreeNode.h
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_DO_NOT_DELETE__OctreeNode__
#define __afstuderen_DO_NOT_DELETE__OctreeNode__

#include <vector>

#include "Octree.h"
#include "../Data/Record.h"
#include "../Geometry/Geometry.h"
#include "OctreeConfig.h"

#define MAX_CHILDREN 8

class Octree;

class OctreeNode {
private:
	OctreeNode* children[MAX_CHILDREN];
	bool isLeaf = true; //If it has no children
    float value = 0;
    size_t addCount = 0;
    
    unsigned short maxDepth;
    unsigned int maxCapacity;
	
	unsigned short depth; //The depth of this node in the octree
	
    float aggregateHelper(ushort orginalDepth);
	bool rangeCheck(float lowerBound, float upperBound, float value);
    float computeEnergy(RIVRecord<float>* rs, RIVRecord<float>* gs,RIVRecord<float>* bs, std::vector<size_t>& indices);
public:
    float cx,cy,cz; //Center of the node
    float size; //width, height and depth
    
    OctreeNode(float cx,float cy, float cz, float cubicSize, unsigned short depth, unsigned short maxDepth, unsigned int maxCapacity);
    void Split(unsigned short splitDepth = 1);
    void Add(float x, float y, float z, float value);
    void Clear();
    float Value();
    float AggregateValue();
    OctreeNode* ChildForCoordinates(float x, float y, float z);
    
	~OctreeNode();
	
	Point3D Center();
	//Get the child at the given index, a error is thrown when the index is out of bounds
	OctreeNode* GetChild(int index);
	//The geometric size of this node
	float GetSize();
	ushort GetDepth();
	bool IsLeafNode();
	bool Contains(const Point3D& point);
	bool ContainsAnyPoints();
    friend std::ostream& operator<<(std::ostream& os, const OctreeNode& node);
};


#endif /* defined(__afstuderen_DO_NOT_DELETE__OctreeNode__) */
