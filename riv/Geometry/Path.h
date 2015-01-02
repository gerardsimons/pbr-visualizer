//
//  Path.h
//  afstuderen_test
//
//  Created by Gerard Simons on 30/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__Path__
#define __afstuderen_test__Path__

#include <cstdlib>
#include <vector>
#include "Graphics/ColorMap.h"

typedef unsigned short ushort;

typedef struct PathPoint {
	size_t rowIndex;
	ushort bounceNr;
	riv::Color color; //3x float
} PathPoint;

class Path {
private:
	PathPoint* points = NULL;
	ushort size;
	Path() {
		
	}
public:
	~Path() {
		if(points) {
			delete[] points;
		}
	}
	Path& operator=( const Path& newPath) {
		
		points = new PathPoint[newPath.size];
		for(int i = 0 ; i < newPath.size ; ++i) {
			points[i] = newPath.points[i];
		}
		return *this;
	}
	Path(const Path& copy) {
		points = new PathPoint[copy.size];
		for(int i = 0 ; i < copy.size ; ++i) {
			points[i] = copy.points[i];
		}
		size = copy.size;
	}
	Path(const std::vector<PathPoint>& points_) {
		size = points_.size();
		points = new PathPoint[size];
		int i = 0;
		for(const PathPoint& p : points_) {
			points[i++] = p;
		}
		//		printf("Path created\n");
	}
	
	size_t Size() const {
		return size;
	}
	PathPoint* GetPoint(size_t index) const {
		return &points[index];
	}
	bool IsEmpty() {
		return size == 0;
	}
	PathPoint* GetPointWithBounce(ushort bounceNr) const {
		for(ushort i = 0 ; i < size ; ++i) {
			if(points[i].bounceNr == bounceNr) {
				return &points[i];
			}
		}
		return NULL;
	}
};

#endif /* defined(__afstuderen_test__Path__) */