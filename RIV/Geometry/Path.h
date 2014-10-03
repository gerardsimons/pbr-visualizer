//
//  Path.h
//  afstuderen_test
//
//  Created by Gerard Simons on 30/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__Path__
#define __afstuderen_test__Path__

#include <vector>
#include "ColorMap.h"

typedef unsigned short ushort;

class Path {
private:
	ushort size = 0;
	size_t* vertices = NULL;
	Color* colors = NULL;
public:
	Path(size_t* vertices, Color* colors, size_t size) {
		this->vertices = new size_t[size];
		this->colors = new Color[size];
		this->size = size;
		for(size_t i = 0 ; i < size ; ++i) {
			this->vertices[i] = vertices[i];
			this->colors[i] = colors[i];
		}
//		printf("Created #%zu\n",creationCount++);
	}
	size_t Size() const {
		return size;
	}
	size_t GetPoint(size_t index) const {
		return vertices[index];
	}
	Color GetColor(size_t index) const {
		return colors[index];
	}
	bool IsEmpty() {
		return size == 0;
	}
};

#endif /* defined(__afstuderen_test__Path__) */
