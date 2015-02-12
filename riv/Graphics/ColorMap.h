//
//  ColorMap.h
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 08/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_DO_NOT_DELETE__ColorMap__
#define __afstuderen_DO_NOT_DELETE__ColorMap__

//#include <stdio.h>
#include <vector>

namespace riv {

typedef struct Color {
	float R;
	float G;
	float B;
    float A;
} Color;

//A linearly interpolated color map
class ColorMap {
private:
	std::vector<Color> colorTable;
public:
	ColorMap() {
		
	}
	ColorMap(std::vector<Color>& colors) {
		colorTable = colors;
	}
	
	//Methods
	void SetRange(float minRange,float maxRange);
	Color ComputeColor(float ratio);
	void AddColor(const Color& newColor);
};
}

#endif /* defined(__afstuderen_DO_NOT_DELETE__ColorMap__) */
