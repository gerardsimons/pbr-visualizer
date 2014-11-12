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
    Color alternateColor;
    RIVColorProperty(const Color& alternateColor_) {
        alternateColor = alternateColor_;
    }
    RIVColorProperty() {
        alternateColor = colors::BLACK;
    }
public:
    //DEFAULT COLORS
    virtual bool ComputeColor(RIVTable* table, const size_t& row, ::Color& color) = 0;
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
    bool ComputeColor(RIVTable* table, const size_t& row, ::Color& color) {
		color = fixedColor;
        return true; //Always possible
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
	
	RIVEvaluatedColorProperty(RIVTable *colorReference_, const std::string& recordName, const ColorMap& colorMap) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,recordName){
		//        memcpy(colorOne, colorOne_, sizeof(colorOne));
		//        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
		
		this->colorMap = colorMap;
	}
	
    bool ComputeColor(RIVTable* table, const size_t& row, ::Color& color) {
        float value; //Assuming this value will be between 0 and 1
        if(RIVEvaluatedProperty<T>::Value(table,row,value))  {
//            printf("Color value = %f\n",value);
			//
			color = colorMap.ComputeColor(value);
			return true;
        }
        return false;
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
    bool ComputeColor(RIVTable* table, const size_t& row, ::Color& color) {
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
	RIVColorRGBProperty(RIVTable* referenceTable, const std::string& redRecord, const std::string& greenRecord,const std::string& blueRecord) {
		redColorProperty = new RIVEvaluatedProperty<T>(referenceTable,redRecord);
		greenColorProperty = new RIVEvaluatedProperty<T>(referenceTable,greenRecord);
		blueColorProperty = new RIVEvaluatedProperty<T>(referenceTable,blueRecord);
    }
    bool ComputeColor(RIVTable* table, const size_t& row, ::Color& color) {
		float ratioRed;
		float ratioGreen;
		float ratioBlue;
		if(redColorProperty->Value(table,row,ratioRed) && blueColorProperty->Value(table,row,ratioBlue) && greenColorProperty->Value(table,row,ratioGreen)) {
//			printf("Ratio (r,g,b) = (%f,%f,%f)\n",ratioRed,ratioGreen,ratioBlue);
//			printf("color (r,g,b) = (%f,%f,%f)\n",color[0],color[1],color[2]);
			color.R = ratioRed;
			color.G = ratioGreen;
			color.B = ratioBlue;
			return true;
		}
		return false;
	}
};


#endif /* defined(__Afstuderen__ColorProperty__) */
