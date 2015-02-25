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
OctreeNode::OctreeNode(float cx,float cy, float cz, float cubicSize, unsigned short depth) : cx(cx),cy(cy),cz(cz),size(cubicSize),depth(depth)  {
    
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
                    OctreeNode* newChildNode = new OctreeNode(cx + xOffset * quarterSize, cy + yOffset  * quarterSize, cz + zOffset  * quarterSize,newChildSize,depth + 1);
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
    return value;
}
void OctreeNode::Add(float x, float y, float z, float value) {
    ++addCount;
    if(isLeaf) {
        if(addCount > 1 && depth < 10) {
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
void OctreeNode::Refine() { //Split this node into 8, by adding 8 children
    
//    isLeaf = true;
//    
//    //Or if the max depth has been reached
//    if(depth >= config->MaxDepthAllowed()) {
//        //		printf("Recursion stopped because the tree has reached its maximum depth.\n");
//        return;
//    }
//    //Or if there are less points than the minimum required for splitting
////    if(NumberOfPointsContained() <= config->MaxNodeCapacity()) {
////        //		printf("Recursion stopped because less points than MAX_NODE_CAPACITY was contained in all leaf nodes.\n");
////        return;
////    }
//    if(NumberOfPointsContained() == 0) { //When empty definitely stop
//        return;
//    }
//    
//    isLeaf = false;
//    
//    //Check if capacity is reached and depth is not >= MAX_DEPTH
//    size_t childIndex = 0;
//    
//    //The center of the new children are all .25 times the dimension away from the center in either direction
//    float newSize = .5 * size;
//    float halfSize = .25 * size;
//    
//    //Create 8 new nodes
//    for(int xOffset = -1 ; xOffset <= 1 ; xOffset += 2 ) {
//        for(int yOffset = -1 ; yOffset <= 1 ; yOffset += 2) {
//            for(int zOffset = -1 ; zOffset <= 1 ; zOffset += 2) {
//                OctreeNode* newChildNode;
//                if(xsTwo) {
////                    newChildNode = new OctreeNode(xsOne,ysOne,zsOne,rsOne,gsOne,bsOne,xsTwo,ysTwo,zsTwo,rsTwo,gsTwo,bsTwo, x + xOffset * halfSize,y + yOffset * halfSize,z + zOffset * halfSize, newSize,depth + 1,config,tree);
//                }
////                else newChildNode = new OctreeNode(xsOne,ysOne,zsOne,rsOne,gsOne,bsOne, x + xOffset * halfSize,y + yOffset * halfSize,z + zOffset * halfSize, newSize,depth + 1,config,tree);
//                children[childIndex] = newChildNode;
//                ++childIndex;
//            }
//        }
//    }
//    
//    for(size_t index : indicesOne) { //For each point
//        Point3D p = Point3D(xsOne->Value(index),ysOne->Value(index),zsOne->Value(index));
//        //			printf("Point #%zu : ",index);
//        //		std::cout << p << "\n";
//        for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) { //Check which node contains it
//            OctreeNode* node = children[i];
//            if(node->Contains(p)) { //Check if the node contains it
//                //					std::cout << "Node contains p " << p;
//                node->AssignPointOne(index); //Assign it to the node
//                break; //Go to next point, the areas are not overlapping so they will not contain it
//            }
//        }
//    }
//    for(size_t index : indicesTwo) { //For each point
//        Point3D p = Point3D(xsTwo->Value(index),ysTwo->Value(index),zsTwo->Value(index));
//        //			printf("Point #%zu : ",index);
//        //		std::cout << p << "\n";
//        for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) { //Check which node contains it
//            OctreeNode* node = children[i];
//            if(node->Contains(p)) { //Check if the node contains it
//                //					std::cout << "Node contains p " << p;
//                node->AssignPointTwo(index); //Assign it to the node
//                break; //Go to next point, the areas are not overlapping so they will not contain it
//            }
//        }
//    }
//    
//    //Recursively refine all the children
//    for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
//        children[i]->Refine();
//    }
//    
//    indicesOne.clear();
//    indicesTwo.clear();
}
size_t OctreeNode::GetDepth() {
    return depth;
}
size_t OctreeNode::NumberOfNodes() {
    size_t nrNodes = 1;
    if(!isLeaf) {
        for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
            nrNodes += children[i]->NumberOfNodes();
        }
    }
    return nrNodes;
}
//size_t OctreeNode::NumberOfPointsContained() const {
//    return indicesOne.size() + indicesTwo.size();
//}
OctreeNode* OctreeNode::GetChild(int index) {
    if(index > MAX_CHILDREN || isLeaf) {
        throw "Child does not exist for this node.";
    }
    return children[index];
}
int OctreeNode::NumberOfChildren() {
    return (isLeaf ? 0 : MAX_CHILDREN);
}
//void OctreeNode::AssignPointOne(size_t index) {
//    indicesOne.push_back(index);
//    //	printf("Point added. Node contains %zu points.\n",pointIndices.size());
//}
//void OctreeNode::AssignPointTwo(size_t index) {
//    indicesTwo.push_back(index);
//    //	printf("Point added. Node contains %zu points.\n",pointIndices.size());
//}
//float OctreeNode::computeEnergy(RIVRecord<float>* rs, RIVRecord<float>* gs,RIVRecord<float>* bs, std::vector<size_t>& indices) {
//    float sumEnergy = 0;
//    
//    for(size_t index : indices) {
//        
//        float rgbAverage = (rs->Value(index) + gs->Value(index) + bs->Value(index)) / 3.F;
//        sumEnergy += rgbAverage;
//    }
//    
//    if(indicesOne.size()) {
//        return sumEnergy / indices.size() / (depth * depth);
//    }
//    else return 0;
//}
//float OctreeNode::ComputeEnergyOne() {
//    return computeEnergy(rsOne, gsOne, bsOne, indicesOne);
//}
//float OctreeNode::ComputeEnergyTwo() {
//    return computeEnergy(rsTwo, gsTwo, bsTwo, indicesTwo);
//}
Point3D OctreeNode::Center() {
    return Point3D(cx,cy,cz);
}
//OctreeNode::~OctreeNode() {
//    if(!isLeaf) {
//        for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
//            OctreeNode* child = children[i];
//            if(child) {
////                delete child;
//            }
//        }
//    }
//}
float OctreeNode::GetSize() {
    return size;
}
bool OctreeNode::Contains(const Point3D& point) {
    
    //		std::cout << " Node center : " << Center();
    
    float halfSize = .5 * size;
    
    bool xContains = rangeCheck(cx - halfSize, cx + halfSize,point.x);
    bool yContains = rangeCheck(cy - halfSize, cy + halfSize,point.y);
    bool zContains = rangeCheck(cz - halfSize, cz + halfSize,point.z);
    
    return xContains && yContains && zContains; //True only if it is within the x, y and z bounds
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
//bool OctreeNode::ContainsAnyPoints() {
//    return (indicesOne.size() + indicesTwo.size()) > 0;
//}
//float OctreeNode::Density() {
//    //	printf("depth = %zu\n",depth);
//    //	float relativeVolume = (1.F / depth) * (1.F / depth) * (1.F / depth);
//    float relativeVolume = (1.F / depth);
//    float density = NumberOfPointsContained() / relativeVolume;
//    //	if (density>0 && density/density != density/density) {
//    //		throw "Infinity error";
//    //	}
//    //	printf("density = %f\n",density);
//    return density;
//}
//size_t OctreeNode::MaxDepth() {
//    if(isLeaf) { //End of the line, return the depth of this node
//        return depth;
//    }
//    else {
//        //return the deepest of its children's maxdepth
//        size_t depths[8];
//        for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
//            depths[i] = children[i]->MaxDepth();
//        }
//        return maxInArray(depths,8);
//    }
//}

