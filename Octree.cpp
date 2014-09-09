//
//  Octree.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Octree.h"

void Octree::init() {
	//Find width height and depth of the root cube
	
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float minZ = std::numeric_limits<float>::max();
	
	float maxX = std::numeric_limits<float>::min();
	float maxY = std::numeric_limits<float>::min();
	float maxZ = std::numeric_limits<float>::min();
	
	if(xPositions != NULL && yPositions != NULL && zPositions != NULL) {
		for(size_t i = 0 ; i < xPositions->size() ; ++i) {
			checkMinMax(minX, maxX, (*xPositions)[i]);
			checkMinMax(minY, maxY, (*yPositions)[i]);
			checkMinMax(minZ, maxZ, (*zPositions)[i]);
		}
	}
	else throw "Some coordinates vectors are not set.";
	
	float width = maxX - minX;
	float height = maxY - minY;
	float depth = maxZ - minZ;
	
	float size = std::max(width,std::max(height,depth)); //Make sure that the node encompasses all points, so should be the largest dimension found
	
	float x = minX + width / 2.F;
	float y = minY + height / 2.F;
	float z = minZ + depth / 2.F;
	
	//Declare root node
	root = new OctreeNode(xPositions,yPositions,zPositions,x,y,z,size,1,&config);
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
Octree::Octree(std::vector<float>* xPositions, std::vector<float>* yPositions, std::vector<float>* zPositions, OctreeConfig& configuration) :  config(configuration) {
	
	this->xPositions = xPositions;
	this->yPositions = yPositions;
	this->zPositions = zPositions;
	
	init();
	
	//Assign all the points to the root node
	for(size_t i = 0 ; i < xPositions->size() ; ++i) {
		root->AssignPoint(i);
	}
	
	//Assign all children to the node
	root->Refine();
};
Octree::Octree(std::vector<float>* xPositions, std::vector<float>* yPositions, std::vector<float>* zPositions, const std::vector<size_t>& indexSubset, OctreeConfig& configuration) :  config(configuration) {
	
	this->xPositions = xPositions;
	this->yPositions = yPositions;
	this->zPositions = zPositions;
	
	init();
	
	//Assign all the points to the root node
	for(size_t i = 0 ; i < indexSubset.size() ; ++i) {
		root->AssignPoint(indexSubset[i]);
	}
	
	root->Refine();
}
size_t Octree::NumberOfPoints() {
	if(xPositions != NULL) {
		return xPositions->size();
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
	Octree* testTree = new Octree(xPositions,yPositions,zPositions,subset,config);
	
	printf("TestTree has %zu children.\n",testTree->NumberOfNodes());
	printf("TestTree has depth %zu\n",testTree->Depth());
	
	std::cout << *testTree;
	
	return false;
}

