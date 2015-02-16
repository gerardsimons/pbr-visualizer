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
private:
	std::vector<int> pointsOne; //Only Y needs to defined for a axiss
	std::vector<int> pointsTwo;
public:
	int x;
	int y;
	int width;
	int height;
	
	std::string name; //Usually points to a record's name, acts as unique ID!

	std::vector<int>& GetPoints(int index) {
		if(index == 0) {
			return pointsOne;
		}
		else
			return pointsTwo;
	}
	
	//If the axis is selected it may update other views such as the slider view to change it focus to include this data dimension
	bool isSelected = false;
	//Does it have a selection box used to filter data according to its dimensions
	bool HasSelectionBox = false;
	RIVRectangle selection;
protected:
	ParallelCoordsAxisInterface(int x, int y, int width, int height, const std::string& name) : x(x), y(y), width(width), height(height), name(name) {
		
	}
};

template <typename T>
class ParallelCoordsAxis : public ParallelCoordsAxisInterface {
public:
	std::vector<T> scale;
	
	Histogram<T> densityHistogramOne;
	Histogram<T> densityHistogramTwo;
	
	//Ease of access using index
//	std::vector<Histogram<T>*> histograms;
	
	Histogram<T> differenceDensity;
	bool differenceDensityComputed = false;
	
	T minValue;
	T maxValue;
	RIVRecord<T>* recordPointer;
	
	ParallelCoordsAxis(int x,int y,int width, int height, T minValue, T maxValue, const std::string& name, RIVRecord<T>* recordPointer, unsigned int scaleDivision, const Histogram<T>& histogramOne)
	: ParallelCoordsAxisInterface(x,y,width,height,name),minValue(minValue),maxValue(maxValue),recordPointer(recordPointer), densityHistogramOne(histogramOne) {
		ComputeScale(scaleDivision);
		
//		densityHistogramOne = Histogram<T>(name,minValue,maxValue,bins);
//		densityHistogramTwo = Histogram<T>(name,minValue,maxValue,bins);
//		
//		histograms.push_back(&densityHistogramOne);
//		histograms.push_back(&densityHistogramTwo);
	}
	
	ParallelCoordsAxis(int x,int y,int width, int height, T minValue, T maxValue, const std::string& name, RIVRecord<T>* recordPointer, unsigned int scaleDivision,const Histogram<T>& histogramOne, const Histogram<T>& histogramTwo)
	: ParallelCoordsAxisInterface(x,y,width,height,name),minValue(minValue),maxValue(maxValue),recordPointer(recordPointer),densityHistogramOne(histogramOne), densityHistogramTwo(histogramTwo) {
		ComputeScale(scaleDivision);
		
		//		densityHistogramOne = Histogram<T>(name,minValue,maxValue,bins);
		//		densityHistogramTwo = Histogram<T>(name,minValue,maxValue,bins);
		//
		//		histograms.push_back(&densityHistogramOne);
		//		histograms.push_back(&densityHistogramTwo);
	}
	
	ParallelCoordsAxis() {
		
	}
	
	void ResetDensities() {
//		densityHistogramOne->Clear();
//		densityHistogramTwo->Clear();
		differenceDensityComputed = false;
	}
	
	Histogram<T>* GetHistogram(int i) {
		if(i == 0) {
			return &densityHistogramOne;
		}
		else return &densityHistogramTwo;
	}
	
	Histogram<T>* GetHistogramOne() {
		return &densityHistogramOne;
	}
	
	Histogram<T> GetHistogramTwo() {
		return &densityHistogramTwo;
	}
	
//	void SetHistogram(Histogram<T>& densityHistogram) {
//		this->densityHistogram = densityHistogram;
//	}
	
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
			// throw new std::string("scalar out of bounds : %f \n",scalar);
			// return std::numeric_limits<float>::quiet_NaN();
			return y;
		}
	}
	//TODO: Return as int?
	float PositionOnScaleForValue(float value) {
		float scalar = (value - minValue) / (maxValue - minValue);
		if(minValue == maxValue) {
			scalar = 0.5F;
		}
		return PositionOnScaleForScalar(scalar);
	}
	
	//Returns the value
	T ValueOnScale(float value) {
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