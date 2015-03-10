//
//  ColorMap.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 08/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ColorMap.h"
#include <cstdlib>

namespace riv {
    ColorMap::ColorMap(std::vector<Color>& colors) : colorTable(colors){

    }
    ColorMap::ColorMap(std::vector<Color>& colors, float minRange, float maxRange) : colorTable(colors), min(minRange), max(maxRange) {
    }
    ColorMap::ColorMap(float minRange, float maxRange) : min(minRange), max(maxRange){

    }
	Color ColorMap::ComputeColor(float value) {
        value -= min;
        value /= (max - min);
		if(value < 0) {
			return colorTable[0];
		}
		for(size_t colorIndex = 0 ; colorIndex < colorTable.size() - 1; ++colorIndex) {
			
			float colorIndexRatioLeft  = colorIndex / (float)(colorTable.size() - 1);
			float colorIndexRatioRight = (colorIndex + 1) / (float)(colorTable.size() - 1);
			Color color;
			
			if(colorIndexRatioLeft <= value && colorIndexRatioRight >= value) {
				//Its in between these two indices, use these to interpolate
				float ratio = (value - colorIndexRatioLeft) / (colorIndexRatioRight - colorIndexRatioLeft);
				
				//                    printf("ratio = %f\n",ratio);
				
				//                    float H = colors[colorIndex][0] * ratio + (1.F - ratio) * colors[colorIndex + 1][0];
				//                    float S = colors[colorIndex][1] * ratio + (1.F - ratio) * colors[colorIndex + 1][1];
				//                    float V = colors[colorIndex][2] * ratio + (1.F - ratio) * colors[colorIndex + 1][2];
				//                    HSVtoRGB(&color[0],&color[1],&color[2],H,S,V);
				
				ratio = 1.F - ratio;
				color.R = colorTable[colorIndex].R * ratio + (1.F - ratio) * colorTable[colorIndex + 1].R;
				color.G = colorTable[colorIndex].G * ratio + (1.F - ratio) * colorTable[colorIndex + 1].G,
				color.B = colorTable[colorIndex].B * ratio + (1.F - ratio) * colorTable[colorIndex + 1].B;
				
				return color;
				
				//			break; //We are done, this is the one
			}
		}
		return colorTable[colorTable.size() - 1];
	}
	
	void ColorMap::AddColor(const Color& newColor) {
		colorTable.push_back(newColor);
	}
}
