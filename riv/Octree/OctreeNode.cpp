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
        
//        printf("Node -- Split!\n");
        
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
//                    newChildNode->value = value / 8.F;
//                    newChildNode->addCount = 1;
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
    int index = xChild + yChild + zChild;
    OctreeNode* child = children[index];
    
//    float childDistance = std::sqrt(std::pow(child->cx - x,2) + std::pow(child->cy - y,2) + std::pow(child->cz - z,2));
//    //Check
//    for(int i = 0 ; i < 8 ; i++) {
//        if(i != index) {
//            float distance = std::sqrt(std::pow(children[i]->cx - x,2) + std::pow(children[i]->cy - y,2) + std::pow(children[i]->cz - z,2));
//            if(distance < childDistance - 0.0001) {
//                
//            }
//        }
//    }
    
    return child;
}
float OctreeNode::Value() {
    if(addCount) {
//        return value / addCount * 8 * depth;
        return value / addCount * std::pow(depth,8);
    }
    else return 0;
}
float OctreeNode::aggregateHelper(ushort originalDepth) {
    if(isLeaf) {
        if(addCount) {
            return value / addCount * (8*(depth - originalDepth));
        }
        else return 0;
    }
    else {
        float sum = 0;
        for(int i = 0 ; i < 8 ; ++i) {
            sum += children[i]->aggregateHelper(originalDepth);
        }
        return sum;
    }
}
float OctreeNode::AggregateValue() {
    
    return Value();
//    if(isLeaf) {
//        return Value();
//    }
//    else {
//        return aggregateHelper(depth);
//    }
}
void OctreeNode::Add(float x, float y, float z, float value) {
    ++addCount;
//    this->value += value;
    if(isLeaf) {

        //Split if necessary
        if(addCount >= maxCapacity && depth < maxDepth) {
            Split();
            OctreeNode* child = ChildForCoordinates(x, y, z);
//            printf("Add part of old value = %f\n",this->value / 8.F + value);
            child->Add(x, y, z, value);
//            this->value = 0;
//            ++addCount;
        }
        else {
//            ++addCount;
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

