//
//  OctreeNode.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "OctreeNode.h"
#include "../helper.h"
OctreeNode::~OctreeNode() {
    if(!isLeaf) {
        for(int i = 0 ; i < 8 ; ++i) {
            delete children[i];
        }
    }
}
OctreeNode::OctreeNode(float cx,float cy, float cz, float cubicSize, unsigned short depth, unsigned short maxDepth, unsigned int maxCapacity) : cx(cx),cy(cy),cz(cz),size(cubicSize),depth(depth),maxCapacity(maxCapacity),maxDepth(maxDepth)  {
    
}
void OctreeNode::Split(unsigned short splitDepth) {
    if(splitDepth) {
        isLeaf = false;
        unsigned short childIndex = 0;
        
        //The center of the new children are all .25 times the dimension away from the center in either direction
        float newChildSize = .5 * size;
        float quarterSize = .25 * size;
        
        //Create 8 new nodes
        for(int xOffset = -1 ; xOffset <= 1 ; xOffset += 2 ) {
            for(int yOffset = -1 ; yOffset <= 1 ; yOffset += 2) {
                for(int zOffset = -1 ; zOffset <= 1 ; zOffset += 2) {
                    OctreeNode* newChildNode = new OctreeNode(cx + xOffset * quarterSize, cy + yOffset  * quarterSize, cz + zOffset  * quarterSize,newChildSize,depth + 1,maxDepth,maxCapacity);
                    children[childIndex++] = newChildNode;
                    newChildNode->Split(splitDepth - 1);
                }
            }
        }
    }
}
OctreeNode* OctreeNode::ChildForCoordinates(float x, float y, float z) {
//    float quarterSize = .25 * size;
    int xChild = ((x - cx) > 0) * 4;
    int yChild = ((y - cy) > 0) * 2;
    int zChild = ((z - cz) > 0) * 1;
    
    return children[xChild + yChild + zChild];
}
float OctreeNode::Value() {
    if(addCount) {
        return value * (pow(8,depth)) / addCount;
    }
    else return 0;
}
float OctreeNode::AggregateValue() {
    if(isLeaf) {
        return Value();
    }
    else {
        float sum = 0;
        for(int i = 0 ; i < 8 ; ++i) {
            sum += children[i]->AggregateValue();
        }
        return sum;
    }
}
void OctreeNode::Add(float x, float y, float z, float value) {
    ++addCount;
    if(isLeaf) {
        //Does it contain enough values and is it not too deep already?
        if(addCount > maxCapacity && depth < maxDepth) {
            value = 0;
            Split();
            OctreeNode* child = ChildForCoordinates(x, y, z);
            child->Add(x, y, z, value + this->value / 8.F);
        }
        else {
            this->value += value;
        }
    }
    else {
        ChildForCoordinates(x, y, z)->Add(x, y, z, value);
    }
}
ushort OctreeNode::GetDepth() {
    return depth;
}
OctreeNode* OctreeNode::GetChild(int index) {
    if(index > MAX_CHILDREN || isLeaf) {
        throw "Child does not exist for this node.";
    }
    return children[index];
}
Point3D OctreeNode::Center() {
    return Point3D(cx,cy,cz);
}
float OctreeNode::GetSize() {
    return size;
}
bool OctreeNode::IsLeafNode() {
    return isLeaf;
}
bool OctreeNode::rangeCheck(float lowerBound, float upperBound, float value) {
    return value >= lowerBound && value <= upperBound;
}
void OctreeNode::Clear() {
    value = 0;
}

