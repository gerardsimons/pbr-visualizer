//
//  Octree.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Octree.h"
#include "../helper.h"

bool Octree::IsEmpty() {
    return root == NULL;
}

void Octree::init() {
	//Find width height and depth of the root cube
	
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float minZ = std::numeric_limits<float>::max();
	
	float maxX = std::numeric_limits<float>::min();
	float maxY = std::numeric_limits<float>::min();
	float maxZ = std::numeric_limits<float>::min();
	
	if(xsOne != NULL && ysOne != NULL && zsOne != NULL) {
		for(size_t i = 0 ; i < xsOne->Size() ; ++i) {
			checkMinMax(minX, maxX, xsOne->Value(i));
			checkMinMax(minY, maxY, ysOne->Value(i));
			checkMinMax(minZ, maxZ, zsOne->Value(i));
		}
	}
	else throw "Some coordinates vectors are not set.";
    
    if(xsTwo != NULL && ysTwo != NULL && zsTwo != NULL) {
        for(size_t i = 0 ; i < xsTwo->Size() ; ++i) {
            checkMinMax(minX, maxX, xsTwo->Value(i));
            checkMinMax(minY, maxY, ysTwo->Value(i));
            checkMinMax(minZ, maxZ, zsTwo->Value(i));
        }
    }
    
	float width = maxX - minX;
	float height = maxY - minY;
	float depth = maxZ - minZ;
	
	float size = std::max(width,std::max(height,depth)); //Make sure that the node encompasses all points, so should be the largest dimension found
	
	float x = minX + width / 2.F;
	float y = minY + height / 2.F;
	float z = minZ + depth / 2.F;
	
	//Declare root node
    if(xsTwo) {
        root = new OctreeNode(xsOne,ysOne,zsOne,rsOne,gsOne,bsOne,xsTwo,ysTwo,zsTwo,rsTwo,gsTwo,bsTwo,x,y,z,size,1,&config,this);
    }
    else {
        root = new OctreeNode(xsOne,ysOne,zsOne,rsOne,gsOne,bsOne,x,y,z,size,1,&config,this);
    }
//	nrOfNodes = root->NumberOfNodes();
	
//	std::cout << *this;
}
void Octree::checkMinMax(float& currentMin, float& currentMax, float candidate) {
	if(currentMax < candidate) {
		currentMax = candidate;
	}
	if(currentMin > candidate) {
		currentMin = candidate;
	}
}
size_t Octree::NumberOfNodes() {
	if(root) {
		return root->NumberOfNodes();
	}
	return 0;
}
size_t Octree::Depth() {
	if(root) {
		if(cachedDepth == NULL) {
			cachedDepth = new size_t(root->MaxDepth());
		}
		return *cachedDepth;
	}
	else return 0;
}
Octree::~Octree() {
	delete root;
	if(cachedDepth) {
		delete cachedDepth;
	}
	
}
float Octree::MaxDensity() {
	if(cachedMaxDensity == NULL) {
		cachedMaxDensity = new float(maxDensityHelper(root));
//		printf("Max density found = %f\n",*cachedMaxDensity);
	}
	return *cachedMaxDensity;
}
size_t Octree::MaxCapacity() {
	if(!cachedMaxCapacity) {
		cachedMaxCapacity = new size_t(maxCapacityHelper(root));
	}
	return *cachedMaxCapacity;
}
float Octree::MaxEnergyOne() {
    if(!cachedMaxEnergyOne) {
        cachedMaxEnergyOne = new float(maxEnergyHelperOne(root));
    }
    return *cachedMaxEnergyOne;
}
float Octree::MaxEnergyTwo() {
    if(!cachedMaxEnergyTwo) {
        cachedMaxEnergyTwo = new float(maxEnergyHelperTwo(root));
    }
    return *cachedMaxEnergyTwo;
}
//Find the max density present in this tree
float Octree::maxDensityHelper(OctreeNode *node) {
	if(node->IsLeafNode()) { //End of the line, return the depth of this node
		return node->Density();
	}
	else {
		float densities[8];
		for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
			densities[i] = maxDensityHelper(node->GetChild(i));
		}
		return maxInArray(densities,8);
	}
}

size_t Octree::maxCapacityHelper(OctreeNode *node) {
	if(node->IsLeafNode()) { //End of the line, return the depth of this node
		return node->NumberOfPointsContained();
	}
	else {
		size_t pointsInChildren[8];
		for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
			pointsInChildren[i] = maxCapacityHelper(node->GetChild(i));
		}
		return maxInArray(pointsInChildren,8);
	}
}
float Octree::maxEnergyHelperOne(OctreeNode* node) {
    if(node->IsLeafNode()) { //End of the line, return the depth of this node
        return node->ComputeEnergyOne();
    }
    else {
        float energyInChildren[8];
        for(ushort i = 0 ; i < MAX_CHILDREN ; ++i) {
            energyInChildren[i] = maxEnergyHelperOne(node->GetChild(i));
        }
        return maxInArray(energyInChildren,8);
    }
}
float Octree::maxEnergyHelperTwo(OctreeNode* node) {
    if(node->IsLeafNode()) { //End of the line, return the depth of this node
        return node->ComputeEnergyTwo();
    }
    else {
        float energyInChildren[8];
        for(ushort i = 0 ; i < MAX_CHILDREN ; ++i) {
            energyInChildren[i] = maxEnergyHelperTwo(node->GetChild(i));
        }
        return maxInArray(energyInChildren,8);
    }
}
Octree::Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zsOne, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, OctreeConfig& configuration) :  config(configuration) {
	
	this->xsOne = xsOne;
	this->ysOne = ysOne;
	this->zsOne = zsOne;
    
    this->rsOne = rsOne;
    this->gsOne = gsOne;
    this->bsOne = bsOne;
	
	init();
	
	//Assign all the points to the root node
	for(size_t i = 0 ; i < xsOne->Size() ; ++i) {
		root->AssignPointOne(i);
	}
	
	//Assign all children to the node
	root->Refine();
};
Octree::Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zsOne, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, const std::vector<size_t>& indexSubset, OctreeConfig& configuration) :  config(configuration) {
	
	this->xsOne = xsOne;
	this->ysOne = ysOne;
	this->zsOne = zsOne;
    
    this->rsOne = rsOne;
    this->gsOne = gsOne;
    this->bsOne = bsOne;
	
	init();
	
	//Assign all the points to the root node
	for(size_t i = 0 ; i < indexSubset.size() ; ++i) {
		root->AssignPointOne(indexSubset[i]);
	}
	
	root->Refine();
}
Octree::Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zsOne, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, RIVRecord<float>* xsTwo, RIVRecord<float>* ysTwo, RIVRecord<float>* zsTwo, RIVRecord<float>* rsTwo, RIVRecord<float>* gsTwo, RIVRecord<float>* bsTwo, OctreeConfig& configuration) : config(configuration) {
    
    this->xsOne = xsOne;
    this->ysOne = ysOne;
    this->zsOne = zsOne;
    
    this->rsOne = rsOne;
    this->gsOne = gsOne;
    this->bsOne = bsOne;
    
    this->xsTwo = xsTwo;
    this->ysTwo = ysTwo;
    this->zsTwo = zsTwo;
    
    this->rsTwo = rsTwo;
    this->gsTwo = gsTwo;
    this->bsTwo = bsTwo;
    
    init();
    
    //Assign all the points to the root node
    for(size_t i = 0 ; i < xsOne->Size() ; ++i) {
        root->AssignPointOne(i);
    }
    for(size_t i = 0 ; i < xsTwo->Size() ; ++i) {
        root->AssignPointTwo(i);
    }
    
    root->Refine();
}
Octree::Octree(RIVRecord<float>* xsOne, RIVRecord<float>* ysOne, RIVRecord<float>* zsOne, RIVRecord<float>* rsOne, RIVRecord<float>* gsOne, RIVRecord<float>* bsOne, RIVRecord<float>* xsTwo, RIVRecord<float>* ysTwo, RIVRecord<float>* zsTwo, RIVRecord<float>* rsTwo, RIVRecord<float>* gsTwo, RIVRecord<float>* bsTwo, const std::vector<size_t>& indexSubsetOne, const std::vector<size_t>& indexSubsetTwo, OctreeConfig& configuration) : config(configuration) {
    
    this->xsOne = xsOne;
    this->ysOne = ysOne;
    this->zsOne = zsOne;
    
    this->rsOne = rsOne;
    this->gsOne = gsOne;
    this->bsOne = bsOne;
    
    this->xsTwo = xsTwo;
    this->ysTwo = ysTwo;
    this->zsTwo = zsTwo;
    
    this->rsTwo = rsTwo;
    this->gsTwo = gsTwo;
    this->bsTwo = bsTwo;
    
    init();
    
    //Assign all the points to the root node
    for(size_t i = 0 ; i < indexSubsetOne.size() ; ++i) {
        root->AssignPointOne(indexSubsetOne[i]);
    }
    for(size_t i = 0 ; i < indexSubsetTwo.size() ; ++i) {
        root->AssignPointTwo(indexSubsetTwo[i]);
    }
    
    root->Refine();
}
Octree::Octree() {
    root = NULL;
}
//TODO: This ain't right when we use a filtered dataset
size_t Octree::NumberOfPoints() {
	if(xsOne != NULL) {
		return xsOne->Size();
	}
	return 0;
}
OctreeNode* Octree::GetRoot() {
	return root;
}
OctreeConfig* Octree::GetConfiguration() {
	return &config;
}
//A free wheeling sandbox type test function
bool Octree::Test() {
	
	OctreeConfig config = OctreeConfig(3,1,.01F);
	
	//Mock position data
	std::vector<float>* xPositions = new std::vector<float>();
	std::vector<float>* yPositions = new std::vector<float>();
	std::vector<float>* zPositions = new std::vector<float>();
	
	float width = 2;
	
	//8 points in a cube fashion, plus one point extra to incite one extra division
	xPositions->push_back(-width);
	xPositions->push_back(-width);
	xPositions->push_back(-width);
	xPositions->push_back(-width);
	xPositions->push_back(width);
	xPositions->push_back(width);
	xPositions->push_back(width);
	xPositions->push_back(width);
	xPositions->push_back(width*.9F);
	
	float height = 2;
	
	yPositions->push_back(height);
	yPositions->push_back(height);
	yPositions->push_back(2*height);
	yPositions->push_back(2*height);
	yPositions->push_back(height);
	yPositions->push_back(height);
	yPositions->push_back(2*height);
	yPositions->push_back(2*height);
	yPositions->push_back(1.9*height);
	
	float depth = 1;
	zPositions->push_back(0);
	zPositions->push_back(depth);
	zPositions->push_back(0);
	zPositions->push_back(depth);
	zPositions->push_back(0);
	zPositions->push_back(depth);
	zPositions->push_back(0);
	zPositions->push_back(depth);
	zPositions->push_back(2*depth);
	
	std::vector<size_t> subset = std::vector<size_t>(2);
	subset[0] = 0;
	subset[1] = 1;
//	Octree* testTree = new Octree(xPositions,yPositions,zPositions,subset,config);
	
//	printf("TestTree has %zu children.\n",testTree->NumberOfNodes());
//	printf("TestTree has depth %zu\n",testTree->Depth());
	
//	std::cout << *testTree;
	
	return false;
}

