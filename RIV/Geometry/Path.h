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

class Path {
private:
	std::vector<size_t> vertexIndices;
	std::vector<Color> colors;
public:
	void AddPoint(const Color& c, size_t vIndex) {
		colors.push_back(c);
		vertexIndices.push_back(vIndex);
	}
	size_t Size() const {
		return vertexIndices.size();
	}
	size_t GetPoint(size_t index) const {
		return vertexIndices[index];
	}
	Color GetColor(size_t index) const {
		return colors[index];
	}
	bool IsEmpty() {
		return vertexIndices.size() == 0;
	}
};

#endif /* defined(__afstuderen_test__Path__) */
