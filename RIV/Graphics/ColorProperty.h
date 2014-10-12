//
//  ColorProperty.h
//  Afstuderen
//
//  Created by Gerard Simons on 29/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__ColorProperty__
#define __Afstuderen__ColorProperty__

#include "Property.h"

#include <stdio.h>
#include <string.h>
#include "ColorPalette.h"
#include "ColorMap.h"

#include "SQLDataView.h"

class RIVRecord;


class RIVColorProperty {
public:
	virtual Color ComputeColor() = 0;
	virtual void Start(sqlite::DataView* view) = 0;
//	virtual void Stop() = 0;
	
};

class RIVFixedColorProperty : public RIVColorProperty{
private:
     ::Color fixedColor;
    
public:
    RIVFixedColorProperty(::Color color) {
        fixedColor = color;
    }
	RIVFixedColorProperty() {
		fixedColor.R = 0;
		fixedColor.G = 0;
		fixedColor.B = 0;
	}
    RIVFixedColorProperty( float r,  float g,  float b) {
		fixedColor.R = r;
		fixedColor.G = g;
		fixedColor.B = b;
    }
    Color ComputeColor() {
		return fixedColor;
//        return true; //Always possible
    }
	void Start(sqlite::DataView* view) {};
	void Stop() {};
};

//enum INTERPOLATION_SCHEME {
//    DISCRETE,
//    CONTINUOUS
//};

//Color changes linearly from colorOne to colorTwo either by using a linear interpolator to create a smooth gradient,
//or by using a discrete interpolator which rounds interpolated values down to create discretized values/colors
template <typename T>
class RIVEvaluatedColorProperty : public RIVEvaluatedProperty<T>, public RIVColorProperty {
private:
    //When a row has no reference to the color reference through linkage, should a default color be returned or simply NULL?
    bool useAlternateColors = false;
	
	ColorMap colorMap;

    //What to do when multiple rows (returned by reference chain) have different membmerships to different color interpolators? This function deals with this
//    float const* colorForMultipleResolvedRows(const std::vector<size_t>& rows);
public:
    
//    RIVEvaluatedColorProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_) : RIVColorProperty(alternateColor_),RIVEvaluatedProperty<T>(colorReference_){
//        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//        //    clusterColorInterpolator = NULL;
////    }
//    RIVEvaluatedColorProperty(RIVTable *colorReference_, float const* colorOne_, float const* colorTwo_) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_){
////        memcpy(colorOne, colorOne_, sizeof(colorOne));
////        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//        
//		colorMap.AddColor(colorOne_);
//		colorMap.AddColor(colorTwo_);
	
//        colorMap[0] = colorOne_;
//        colorMap[1] = colorOne_;
//    }
//    RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, float const* colorOne_, float const* colorTwo_) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
////        memcpy(colorOne, colorOne_, sizeof(colorOne));
////        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//        
////        colorMap.resize(2);
//        colorMap[0] = colorOne_;
//        colorMap[1] = colorTwo_;
//    }
//    RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, const std::vector<const float*>& colors) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
//        //        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        //        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//        
//		for(const float* color : colors) {
//			colorMap.AddColor(color);
//		}
//    }
//	RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, const ColorMap& colorMap) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
//        //        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        //        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//        
//		this->colorMap = colorMap;
//    }
//	
//	RIVEvaluatedColorProperty(RIVTable *colorReference_, const std::string& recordName, const ColorMap& colorMap) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,recordName){
//		//        memcpy(colorOne, colorOne_, sizeof(colorOne));
//		//        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//		
//		this->colorMap = colorMap;
//	}
	
	RIVEvaluatedColorProperty(sqlite::DataView* referralView, sqlite::Column* referralColumn, DataController* dc, const ColorMap& colorMap) : RIVEvaluatedProperty<T>(referralView,referralColumn,dc) {
		this->colorMap = colorMap;
	}
	void Start(sqlite::DataView* view) {
		RIVEvaluatedProperty<T>::Start(view);
	}
    Color ComputeColor() {
        float value; //Assuming this value will be between 0 and 1
		value = RIVEvaluatedProperty<T>::Value();
//            printf("Color value = %f\n",value);
		
			return colorMap.ComputeColor(value);
		
	}
	virtual void Stop() {
		RIVEvaluatedProperty<T>::Stop();
	}
};
//Returns color by cycling through a fixed pre-determined set of colors
class RIVColorDiscreteProperty : public RIVColorProperty {
private:
    std::vector<float const*> colors;
    size_t colorsToUse;
    size_t colorPointer;
public:
    RIVColorDiscreteProperty(std::vector<float const*> colors_, const size_t& colorsToUse_) {
        colorsToUse = colorsToUse_;
        colors = colors_;
    }
    bool ComputeColor(sqlite::DataView* view, const size_t& row, ::Color& color) {
        size_t index = colorPointer % colorsToUse;
        colorPointer++;
        return colors[index];
    }
};
//Each R,G and B channel is interpolated independently according to three given
template <typename T>
class RIVColorRGBProperty : public RIVColorProperty {
private:
	//Use three distinct color properties for each RGB channel
	RIVEvaluatedProperty<T>* redColorProperty;
	RIVEvaluatedProperty<T>* greenColorProperty;
	RIVEvaluatedProperty<T>* blueColorProperty;
	

public:
	~RIVColorRGBProperty() {
		delete redColorProperty;
		delete greenColorProperty;
		delete blueColorProperty;
	}
//    RIVColorRGBProperty(RIVTable* referenceTable, RIVRecord *redRecord, RIVRecord* greenRecord,RIVRecord *blueRecord) {
//		redColorProperty = new RIVEvaluatedProperty<T>(referenceTable,redRecord);
//		greenColorProperty = new RIVEvaluatedProperty<T>(referenceTable,greenRecord);
//		blueColorProperty = new RIVEvaluatedProperty<T>(referenceTable,blueRecord);
//    }
	RIVColorRGBProperty(sqlite::DataView* referralView, sqlite::Column* redColumn,sqlite::Column* greenColumn,sqlite::Column* blueColumn, DataController* dc) {
		redColorProperty = new RIVEvaluatedProperty<T>(referralView,redColumn,dc);
		greenColorProperty = new RIVEvaluatedProperty<T>(referralView,greenColumn,dc);
		blueColorProperty = new RIVEvaluatedProperty<T>(referralView,blueColumn,dc);
    }
    Color ComputeColor() {
		float ratioRed = redColorProperty->Value();
		float ratioGreen = greenColorProperty->Value();
		float ratioBlue = blueColorProperty->Value();
		Color color;

		color.R = ratioRed;
		color.G = ratioGreen;
		color.B = ratioBlue;
		
		return color;
	}
	virtual void Start(sqlite::DataView* view) {
		redColorProperty->Start(view);
		greenColorProperty->Start(view);
		blueColorProperty->Start(view);
	}
};


#endif /* defined(__Afstuderen__ColorProperty__) */
