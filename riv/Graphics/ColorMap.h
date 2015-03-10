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

class Color {
public:
	float R;
	float G;
	float B;
    float A;
    
    Color() {};
    Color(float R, float G, float B) : R(R), G(G), B(B), A(1) {
    }
    Color(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {
    }
};

//A linearly interpolated color map
class ColorMap {
private:
	std::vector<Color> colorTable;
    
    float min = 0;
    float max = 1;
public:
	ColorMap() {
		
	}
    ColorMap(std::vector<Color>& colors);
    ColorMap(std::vector<Color>& colors, float minRange, float maxRange);
    ColorMap(float minRange, float maxRange);
	
	//Methods
	void SetRange(float minRange,float maxRange);
	Color ComputeColor(float ratio);
	void AddColor(const Color& newColor);
};
}

#endif /* defined(__afstuderen_DO_NOT_DELETE__ColorMap__) */
