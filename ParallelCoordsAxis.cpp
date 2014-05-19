//
//  ParallelCoordsAxis.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ParallelCoordsAxis.h"
#include <algorithm>

ParallelCoordsAxis::ParallelCoordsAxis(int x,int y, int height, float minValue, float maxValue, const std::string& name) {
    this->x = x;
    this->y = y;
    this->height = height;
    this->name = name;
    this->minValue = minValue;
    this->maxValue = maxValue;
}

ParallelCoordsAxis::ParallelCoordsAxis() {
    
}

//A ratio value indicating where an arbitrary Y position is according the axis (0 = bottom, 1 = top)
float ParallelCoordsAxis::ScaleValueForY(int yPos) {
    float value = (yPos - y) / (float)(height);
//    printf("scaleValue = %f\n",value);
    return value;
}

//Returns the Y position of a value along the scale
float ParallelCoordsAxis::PositionOnScale(float value) {
    value = std::max(0.F,std::min(value,1.F));
	if(value >= 0.F && value <= 1.F) {
		return y + value * height;
	}
	else return std::numeric_limits<float>::quiet_NaN();
}

//Returns the value 
float ParallelCoordsAxis::ValueOnScale(float value) {
	if(value >= 0.F && value <= 1.F) {
		return (1 - value) * minValue + value * maxValue;
	}
    else return std::numeric_limits<float>::quiet_NaN();
}

void ParallelCoordsAxis::ComputeScale(int n) {
	scale.push_back(0.F);
	//What scale is best to use?
	for(size_t i = 1 ; i < n ; i++) {
		float value = (i / (float)n);
		//Round to 1 decimal
		scale.push_back(value);
	}
	scale.push_back(1.F);
}