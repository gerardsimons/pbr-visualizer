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
#include "../Data/Record.h"
#include "../Geometry/Geometry.h"

	//Interface class for parallel coords axis
class ParallelCoordsAxisInterface {
public:
	int height;
	int x;
	int y;
	std::string name; //Usually points to a record's name, acts as unique ID!
	
	bool HasSelectionBox;
	Area selection;
protected:
	ParallelCoordsAxisInterface(int x, int y, int height, const std::string& name) : x(x), y(y), height(height), name(name) {

	}
};

template <typename T>
class ParallelCoordsAxis : public ParallelCoordsAxisInterface {
public:
	std::vector<T> scale;
	T minValue;
	T maxValue;
	RIVRecord<T>* recordPointer;
	ParallelCoordsAxis(int x,int y, int height, T minValue, T maxValue, const std::string& name, RIVRecord<T>* recordPointer, unsigned int scaleDivision)
	: ParallelCoordsAxisInterface(x,y,height,name),minValue(minValue),maxValue(maxValue),recordPointer(recordPointer) {
			ComputeScale(scaleDivision);
		}
		
		ParallelCoordsAxis() {
			
		}
		
		//A ratio value indicating where an arbitrary Y position is according the axis (0 = bottom, 1 = top)
		float ScaleValueForY(int yPos) {
			float value = (yPos - y) / (float)(height);
			//    printf("scaleValue = %f\n",value);
			return value;
		}
		
		float PositionOnScaleForViewY(int viewY) {
			
			//Bound the viewY to axes boundaries
			viewY = std::min(std::max(viewY,y),y + height);
			
			return PositionOnScaleForScalar(ScaleValueForY(viewY));
		}
		
		//Returns the Y position of a value along the scale indicated by a ratio of low and high
		float PositionOnScaleForScalar(float scalar) {
			if(scalar >= 0.F && scalar <= 1.F) {
				return y + scalar * height;
			}
			else {
				throw new std::string("scalar out of bounds : %f \n",scalar);
				return std::numeric_limits<float>::quiet_NaN();
			}
		}
		
		float PositionOnScaleForValue(float value) {
			float scalar = (value - minValue) / (maxValue - minValue);
			if(minValue == maxValue) {
				scalar = 0.5F;
			}
			return PositionOnScaleForScalar(scalar);
		}
		
		//Returns the value
		float ValueOnScale(T value) {
			if(value >= 0.F && value <= 1.F) {
				return (1 - value) * minValue + value * maxValue;
			}
			else return std::numeric_limits<T>::quiet_NaN();
		}
		
		void ComputeScale(int n) {
			scale.push_back(0.F);
			//What scale is best to use?
			for(size_t i = 1 ; i < n ; i++) {
				float value = (i / (float)n);
				//Round to 1 decimal
				scale.push_back(value);
			}
			scale.push_back(1.F);
		}
	};

#endif