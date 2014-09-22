//
//  ParallelCoordsAxis.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ParallelCoordsAxis.h"
#include <algorithm>

namespace riv {

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

float ParallelCoordsAxis::PositionOnScaleForViewY(int viewY) {
    
    //Bound the viewY to axes boundaries
    viewY = std::min(std::max(viewY,y),y + height);
    
    return PositionOnScaleForScalar(ScaleValueForY(viewY));
}

//Returns the Y position of a value along the scale indicated by a ratio of low and high
float ParallelCoordsAxis::PositionOnScaleForScalar(float scalar) {
	if(scalar >= 0.F && scalar <= 1.F) {
		return y + scalar * height;
	}
	else {
        throw new std::string("scalar out of bounds : %f \n",scalar);
        return std::numeric_limits<float>::quiet_NaN();
    }
}

float ParallelCoordsAxis::PositionOnScaleForValue(float value) {
    float scalar = (value - minValue) / (maxValue - minValue);
    if(minValue == maxValue) {
        scalar = 0.5F;
    }
    return PositionOnScaleForScalar(scalar);
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
}