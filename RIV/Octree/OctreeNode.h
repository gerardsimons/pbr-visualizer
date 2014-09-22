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
#include "../Geometry/Geometry.h"
#include "OctreeConfig.h"

#define MAX_CHILDREN 8



class Octree;

class OctreeNode {
private:
	OctreeConfig *config;
	
	//The tree it is part of
	Octree* tree;
	//All the children
	OctreeNode* children[MAX_CHILDREN];
	bool isLeaf; //If it has no children
	
	//The geometric values determine what points fall within a node and more importantly what nodes fall in in which of its children (if any)
	float x,y,z; //Center of the node
	float size; //width, height and depth
	
	
	std::vector<float>* xPositions;
	std::vector<float>* yPositions;
	std::vector<float>* zPositions;
	
	std::vector<size_t> pointIndices;
	
	size_t depth; //The depth of this node in the octree
	
	bool rangeCheck(float lowerBound, float upperBound, float value);
public:
	OctreeNode(std::vector<float>* xPositions,std::vector<float>* yPositions,std::vector<float>* zPositions, float x, float y, float z, float size, size_t depth, OctreeConfig* config, Octree* tree);
	~OctreeNode();
	
	//Check if according to the octree config parameters, this OctreeNode should be split up further and recursively call this function on the new function until recurson stops
	void Refine();
	//Assign a point to this node, the index refers to the point of which its coordinates are given at the index place in the xPositions,yPositions and zPositions
	void AssignPoint(size_t index);
	Point3D Center();
	//Treats the node and all of its descendants as a subtree and counts the number of nodes it contains
	size_t NumberOfNodes();
	//The number of DIRECT children (no grandchildren etc.) so this is either 8 or 0
	int NumberOfChildren();
	//Number of points in this node
	size_t NumberOfPointsContained();
	//Get the child at the given index, a error is thrown when the index is out of bounds
	OctreeNode* GetChild(int index);
	//Search children for max depth
	size_t MaxDepth();
	//The geometric size of this node
	float GetSize();
	//Get a density measure of this node, this is the number of points in the node multiplied by its relative size
	float Density();
	size_t GetDepth();
	bool IsLeafNode();
	bool Contains(const Point3D& point);
	bool ContainsAnyPoints();
	friend std::ostream& operator<<(std::ostream& os, const OctreeNode& node) {
		
		std::string tabSequence;
		//Generate indentation
		for(int i = 0 ; i < node.depth ; ++i) {
			tabSequence += "\t";
		}
		
		os << tabSequence << "Node (" << node.x << "," << node.y << "," << node.z << ")" << " contains " << node.pointIndices.size() << " points: ";
		for(size_t index : node.pointIndices) {
			os << "(" << node.xPositions->at(index) << "," << node.yPositions->at(index) << "," << node.zPositions->at(index) << ")";
		}
		std::cout << "\n";
		if(!node.isLeaf) {
			os << tabSequence << " Has children : \n";
			for(size_t i = 0 ; i < MAX_CHILDREN ; ++i) {
				os << *node.children[i];
			}
		}
		return os;
	}
};


#endif /* defined(__afstuderen_DO_NOT_DELETE__OctreeNode__) */
