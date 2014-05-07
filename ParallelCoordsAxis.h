//
//  ParallelCoordsAxis.h
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include <string>
#include "Record.h"

class ParallelCoordsAxis {
public:
    ParallelCoordsAxis(int x,int y,int height,RIVRecord<float>* record);
	void ComputeScale(int n);
	
	//Returns the Y position of a value along the scale
	float PositionOnScale(float value);
	float ValueOnScale(float value);

	//properties
	std::vector<float> scale;
    int x,y;
    int height;
    RIVRecord<float>* record;
};