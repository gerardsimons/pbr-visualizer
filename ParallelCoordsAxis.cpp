//
//  ParallelCoordsAxis.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ParallelCoordsAxis.h"

ParallelCoordsAxis::ParallelCoordsAxis(int x,int y, int height, RIVRecord* record) {
    this->x = x;
    this->y = y;
    this->height = height;
    this->record = record;
}

//Returns the Y position of a value along the scale
float ParallelCoordsAxis::PositionOnScale(float value) {
	if(value >= 0.F && value <= 1.F) {
		return y + value * height;
	}
}

//Returns the value 
float ParallelCoordsAxis::ValueOnScale(float value) {
	std::pair<float,float> *min_max = record->MinMax();
	if(value >= 0.F && value <= 1.F) {
		return (1 - value) * min_max->first + value * min_max->second;
	}
}

void ParallelCoordsAxis::ComputeScale(int n) {
	scale.push_back(0.F);
	//TWhat scale is best to use?
	for(size_t i = 1 ; i < n ; i++) {
		float value = (i / (float)n);
		//Round to 1 decimal
		scale.push_back(value);
	}
	scale.push_back(1.F);
}