//
//  ParallelCoordsAxis.h
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef _PARALLEL_COORDS_AXIS_H
#define _PARALLEL_COORDS_AXIS_H

#include <string>
#include "Record.h"
#include "Geometry.h"

class ParallelCoordsAxis {
public:
    ParallelCoordsAxis(int x,int y,int height,float min, float max, const std::string& name);
    ParallelCoordsAxis();
	void ComputeScale(int n);
	
	//Returns the Y position of a value along the scale
    float PositionOnScaleForViewY(int);
	float PositionOnScaleForValue(float value);
    float PositionOnScaleForScalar(float scalar);
	float ValueOnScale(float value);
    float ScaleValueForY(int yPos);

	//properties
	std::vector<float> scale;
    int x,y;
    float minValue, maxValue; //TODO: template
    int height;
    
    std::string name; //Usually points to a record's name, acts as unique ID!
    RIVRecord* RecordPointer;
    
    bool HasSelectionBox;
    Area selection;
};

#endif