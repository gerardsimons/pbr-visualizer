//
//  Octree.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Octree.h"
#include "../helper.h"

Octree::Octree(unsigned int depth, float cx, float cy, float cz, float size, unsigned int maxCapacity) : depth(depth), maxCapacity(maxCapacity) {
    init(cx, cy, cz, size);
    
    cachedMax = new float(0);
}
void Octree::init(float cx, float cy, float cz, float size) {
    float halfSize = size / 2.F;
    
    minX = cx - halfSize;
    maxX = cx + halfSize;
    minY = cy - halfSize;
    maxY = cy + halfSize;
    minZ = cz - halfSize;
    maxZ = cz + halfSize;
    
    if(depth) {
        root = new OctreeNode(cx,cy,cz,size,1,depth,maxCapacity);
    }
    else {
        throw std::runtime_error("Depth must be a positive number");
    }
}
void Octree::Add(float x, float y, float z, float value) {
    //Find the
    ++addCount;
    
    if(cachedMax) {
        delete cachedMax;
        cachedMax = NULL;
    }
    
    
//    OctreeNode* node = LeafNodeForCoordinates(x, y, z);
    root->Add(x,y,z,value);
}
size_t Octree::NodesPerDimension() {
    return nDim;
}
bool Octree::IsEmpty() {
    return root == NULL;
}
ushort Octree::Depth() {
    return depth;
}
OctreeNode* Octree::GetRoot() {
    return root;
}
float Octree::maxValueHelper(OctreeNode* node) {
    if(node->IsLeafNode()) {
        return node->Value();
    }
    else {
        float maxValues[8];
        for(ushort i = 0 ; i < 8 ; ++i) {
            maxValues[i] = maxValueHelper(node->GetChild(i));
        }
        return maxInArray(maxValues, 8);
    }
}
float Octree::maxValueHelper(OctreeNode* node,ushort maxDepth) {
    if(node->IsLeafNode() || node->GetDepth() >= maxDepth) {
        return node->AggregateValue();
    }
    else {
        float maxValues[8];
        for(ushort i = 0 ; i < 8 ; ++i) {
            maxValues[i] = maxValueHelper(node->GetChild(i),maxDepth);
        }
        return maxInArray(maxValues, 8);
    }
}
float Octree::MaxValue() {
    if(!cachedMax) {
        cachedMax = new float(maxValueHelper(root));
    }
    return *cachedMax;
}
float Octree::MaxValue(ushort maxDepth) {
    return maxValueHelper(root,maxDepth);
}
void Octree::checkMinMax(float& currentMin, float& currentMax, float candidate) {
	if(currentMax < candidate) {
		currentMax = candidate;
	}
	if(currentMin > candidate) {
		currentMin = candidate;
	}
}
void Octree::Clear() {
    init(root->cx, root->cy, root->cz, root->size);
}