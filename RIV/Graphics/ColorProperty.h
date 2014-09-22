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



class RIVRecord;

//Interface for a color property
class RIVColorProperty {
protected:
    float alternateColor[3];
    RIVColorProperty(float* alternateColor_) {
        memcpy(alternateColor,alternateColor_,sizeof(alternateColor));
    }
    RIVColorProperty() {
        memcpy(alternateColor,colors::BLACK,sizeof(alternateColor));
    }
public:
    //DEFAULT COLORS
    
    virtual float const* Color(RIVTable*,const size_t&) = 0;
};

class RIVFixedColorProperty : public RIVColorProperty{
private:
    const float* color = NULL;
    
public:
    RIVFixedColorProperty(const float* color) {
        this->color = color;
    }
    RIVFixedColorProperty( float r,  float g,  float b) {
        const float c[] = {r,g,b};
        color = c;
    }
    float const* Color(RIVTable*,const size_t&) {
        return color;
    }
};

//enum INTERPOLATION_SCHEME {
//    DISCRETE,
//    CONTINUOUS
//};

//Color changes linearly from colorOne to colorTwo either by using a linear interpolator to create a smooth gradient,
//or by using a discrete interpolator which rounds interpolated values down to create discretized values/colors
template <typename T>
class RIVEvaluatedColorProperty : public RIVColorProperty, public RIVEvaluatedProperty<T> {
private:
    //When a row has no reference to the color reference through linkage, should a default color be returned or simply NULL?
    bool useAlternateColors = false;

    //The two color to interpolate between //TODO: better names?
//    float colorOne[3];
//    float colorTwo[3];
    
	ColorMap colorMap;

    //What to do when multiple rows (returned by reference chain) have different membmerships to different color interpolators? This function deals with this
    float const* colorForMultipleResolvedRows(const std::vector<size_t>& rows);
public:
    
//    RIVEvaluatedColorProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_) : RIVColorProperty(alternateColor_),RIVEvaluatedProperty<T>(colorReference_){
//        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//        //    clusterColorInterpolator = NULL;
//    }
    RIVEvaluatedColorProperty(RIVTable *colorReference_, float const* colorOne_, float const* colorTwo_) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_){
//        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
        
		colorMap.AddColor(colorOne_);
		colorMap.AddColor(colorTwo_);
		
//        colorMap[0] = colorOne_;
//        colorMap[1] = colorOne_;
    }
    RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, float const* colorOne_, float const* colorTwo_) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
//        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
        
//        colorMap.resize(2);
        colorMap[0] = colorOne_;
        colorMap[1] = colorTwo_;
    }
//    RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, const std::vector<const float*>& colors) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
//        //        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        //        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//        
//		for(const float* color : colors) {
//			colorMap.AddColor(color);
//		}
//    }
	RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, const ColorMap& colorMap) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
        //        memcpy(colorOne, colorOne_, sizeof(colorOne));
        //        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
        
		this->colorMap = colorMap;
    }
    
    float const* Color(RIVTable* sourceTable, const size_t& row) {
        float value; //Assuming this value will be between 0 and 1
        if(RIVEvaluatedProperty<T>::Value(sourceTable,row,value))  {
//            printf("Color value = %f\n",value);
			//
			return colorMap.Color(value);
        }
        return NULL;
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
    float const* Color(const RIVTable* table, const size_t& row) {
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
    RIVColorRGBProperty(RIVTable* referenceTable, RIVRecord *redRecord, RIVRecord* greenRecord,RIVRecord *blueRecord) {
		redColorProperty = new RIVEvaluatedProperty<T>(referenceTable,redRecord);
		greenColorProperty = new RIVEvaluatedProperty<T>(referenceTable,greenRecord);
		blueColorProperty = new RIVEvaluatedProperty<T>(referenceTable,blueRecord);
    }
    float const* Color(RIVTable* table, const size_t& row) {
		float ratioRed = 0;
		float ratioGreen = 0;
		float ratioBlue = 0;
		
		if(redColorProperty->Value(table,row,ratioRed) && blueColorProperty->Value(table,row,ratioBlue) && greenColorProperty->Value(table,row,ratioGreen)) {
//			printf("Ratio (r,g,b) = (%f,%f,%f)\n",ratioRed,ratioGreen,ratioBlue);
			static float const color[3] = {ratioRed,ratioGreen,ratioBlue};
			if(ratioRed > 0.5) {
				
			}
			return color;
		}
		else return NULL;
		

	}
};


#endif /* defined(__Afstuderen__ColorProperty__) */
