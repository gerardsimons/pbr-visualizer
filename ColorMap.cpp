//
//  ColorMap.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 08/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ColorMap.h"


const float* ColorMap::Color(float ratio) {
	for(size_t colorIndex = 0 ; colorIndex < colorTable.size() - 1; ++colorIndex) {
		
		float colorIndexRatioLeft  = colorIndex / (float)(colorTable.size() - 1);
		float colorIndexRatioRight = (colorIndex + 1) / (float)(colorTable.size() - 1);
		float color[3];
		if(colorIndexRatioLeft <= ratio && colorIndexRatioRight >= ratio) {
			//Its in between these two indices, use these to interpolate
			//                    return linearInterpolateColor(value, colorMap[colorIndex], colorMap[colorIndex+1]);
			ratio = 1.F - ratio;
			color[0] = colorTable[colorIndex][0] * ratio + (1.F - ratio) * colorTable[colorIndex + 1][0];
			color[1] = colorTable[colorIndex][1] * ratio + (1.F - ratio) * colorTable[colorIndex + 1][1],
			color[2] = colorTable[colorIndex][2] * ratio + (1.F - ratio) * colorTable[colorIndex + 1][2];
			return color;
		}
	}
	return NULL;
}

void ColorMap::AddColor(float const* newColor) {
	colorTable.push_back(newColor);
}

void ColorMap::SetRange(float minRange, float maxRange) {
	
}

