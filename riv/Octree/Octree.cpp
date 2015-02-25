//
//  Octree.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Octree.h"
#include "../helper.h"

Octree::Octree(unsigned int depth, float cx, float cy, float cz, float size) : depth(depth) {
    init(cx, cy, cz, size);
}
void Octree::init(float cx, float cy, float cz, float size) {
    nDim = std::pow(2,depth);
    N = std::pow(8, depth);
    printf("Creating an octree with depth %d and containing %zu x %zu x %zu = %zu leaf nodes\n",depth, nDim,nDim,nDim,N);
    
    float halfSize = size / 2.F;
    
    minX = cx - halfSize;
    maxX = cx + halfSize;
    minY = cy - halfSize;
    maxY = cy + halfSize;
    minZ = cz - halfSize;
    maxZ = cz + halfSize;
    
    if(depth) {
        root = new OctreeNode(cx,cy,cz,size,1);
        
        for(unsigned short i = 1 ; i < depth ; ++i) {
            root->Split(depth);
        }
    }
    else {
        throw std::runtime_error("Depth must be a positive number");
    }
    
    //Create the leafnodes index
    createIndex();
}
unsigned short Octree::binForValue(float value, float min, float max) {
    if(value >= min && value <= max) {
        return (value - min) / (max - min) * nDim;
    }
    throw std::runtime_error("Invalid value");
}
void Octree::createIndex() {
    //Resize the vector
    size_t nDim = pow(2,depth);
    leafNodes.resize(nDim);
    for(int x = 0 ; x < nDim ; ++x) {
        leafNodes[x].resize(nDim);
        for(int y = 0 ; y < nDim ; ++y) {
            leafNodes[x][y].resize(nDim);
        }
    }
    createIndexHelper(root);
}
void Octree::createIndexHelper(OctreeNode* node) {
    if(node->IsLeafNode()) { //End of the line
        ushort xBin = binForValue(node->cx, minX, maxX);
        ushort yBin = binForValue(node->cy, minY, maxY);
        ushort zBin = binForValue(node->cz, minZ, maxZ);
        
//        printf("xBin = %d\n",xBin);
//        printf("yBin = %d\n",yBin);
//        printf("zBin = %d\n",zBin);

        leafNodes[xBin][yBin][zBin] = node;
    }
    else {
        for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
            createIndexHelper(node->GetChild(i));
        }
    }
}
OctreeNode* Octree::GetLeafNode(float xBin, float yBin, float zBin) {
    return leafNodes[xBin][yBin][zBin];
}
OctreeNode* Octree::LeafNodeForCoordinates(float x, float y, float z) {
    size_t xBin = binForValue(x, minX, maxX);
    size_t yBin = binForValue(y, minY, maxY);
    size_t zBin = binForValue(z, minZ, maxZ);
    
    OctreeNode* node = leafNodes[xBin][yBin][zBin];
    if(node == NULL) {
        
    }
    return node;
}
void Octree::Add(float x, float y, float z, float value) {
    //Find the
    ++addCount;
    
//    OctreeNode* node = LeafNodeForCoordinates(x, y, z);
    root->Add(x,y,z,value);
}
size_t Octree::NodesPerDimension() {
    return nDim;
}
bool Octree::IsEmpty() {
    return root == NULL;
}

//void Octree::init() {
	//Find width height and depth of the root cube
	
//	float minX = std::numeric_limits<float>::max();
//	float minY = std::numeric_limits<float>::max();
//	float minZ = std::numeric_limits<float>::max();
//	
//	float maxX = std::numeric_limits<float>::min();
//	float maxY = std::numeric_limits<float>::min();
//	float maxZ = std::numeric_limits<float>::min();
//	
//	if(xsOne != NULL && ysOne != NULL && zsOne != NULL) {
//		for(size_t i = 0 ; i < xsOne->Size() ; ++i) {
//			checkMinMax(minX, maxX, xsOne->Value(i));
//			checkMinMax(minY, maxY, ysOne->Value(i));
//			checkMinMax(minZ, maxZ, zsOne->Value(i));
//		}
//	}
//	else throw "Some coordinates vectors are not set.";
//    
//    if(xsTwo != NULL && ysTwo != NULL && zsTwo != NULL) {
//        for(size_t i = 0 ; i < xsTwo->Size() ; ++i) {
//            checkMinMax(minX, maxX, xsTwo->Value(i));
//            checkMinMax(minY, maxY, ysTwo->Value(i));
//            checkMinMax(minZ, maxZ, zsTwo->Value(i));
//        }
//    }
//    
//	float width = maxX - minX;
//	float height = maxY - minY;
//	float depth = maxZ - minZ;
//	
//	float size = std::max(width,std::max(height,depth)); //Make sure that the node encompasses all points, so should be the largest dimension found
//	
//	float x = minX + width / 2.F;
//	float y = minY + height / 2.F;
//	float z = minZ + depth / 2.F;
//	
//	//Declare root node
//    if(xsTwo) {
//        root = new OctreeNode(xsOne,ysOne,zsOne,rsOne,gsOne,bsOne,xsTwo,ysTwo,zsTwo,rsTwo,gsTwo,bsTwo,x,y,z,size,1,&config,this);
//    }
//    else {
//        root = new OctreeNode(xsOne,ysOne,zsOne,rsOne,gsOne,bsOne,x,y,z,size,1,&config,this);
//    }
//	nrOfNodes = root->NumberOfNodes();
	
//	std::cout << *this;
//}
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
        for(ushort i = 0 ; i < node->NumberOfChildren() ; ++i) {
            maxValues[i] = maxValueHelper(node->GetChild(i));
        }
        return maxInArray(maxValues, 8);
    }
}
float Octree::MaxValue() {
    return maxValueHelper(root);
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
//    for(auto& nodes : leafNodes) {
//        for(auto& moreNodes : nodes) {
//            for(OctreeNode* leafNode : moreNodes) {
//                leafNode->Clear();
//            }
//        }
//    }
}