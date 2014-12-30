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

#include "../Data/DataSet.h"
#include "../Data/Record.h"
#include "ColorPalette.h"
#include "ColorMap.h"

//Interface for a color property
class RIVColorProperty {
protected:
    riv::Color alternateColor;
    RIVColorProperty(const riv::Color& alternateColor_) {
        alternateColor = alternateColor_;
    }
    RIVColorProperty() {
        alternateColor = colors::BLACK;
    }
public:
	//Determine the color for the row of a given table assuming there is a connection between the given table and the reference table
    virtual bool ComputeColor(RIVTableInterface* table, size_t row, riv::Color& color) = 0;
	//Recompute the evaulators, this should happen when the reference table changed, leave unimplemented when not needed
	virtual void Reset(RIVDataSet<float,ushort>* newDataSource) { };
};

class RIVFixedColorProperty : public RIVColorProperty{
private:
     riv::Color fixedColor;
    
public:
    RIVFixedColorProperty(riv::Color color) {
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
    bool ComputeColor(RIVTableInterface* table, size_t row, riv::Color& color) {
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
    
	riv::ColorMap colorMap;

    //What to do when multiple rows (returned by reference chain) have different membmerships to different color interpolators? This function deals with this
    float const* colorForMultipleResolvedRows(const std::vector<size_t>& rows);
public:
    bool ComputeColor(RIVTableInterface* table, size_t row, riv::Color& color) {
        float value; //Assuming this value will be between 0 and 1
        if(RIVEvaluatedProperty<T>::Value(table,row,value))  {
			color = colorMap.ComputeColor(value);
			return true;
        }
        return false;
    }
	
	void Reset(RIVDataSet<float,ushort>* newDataSource) {
		RIVEvaluatedProperty<T>::Reset(newDataSource);
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
    bool ComputeColor(RIVTableInterface* table, size_t row, riv::Color& color) {
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
	RIVColorRGBProperty(RIVTableInterface* referenceTable, RIVRecord<T>* redRecord, RIVRecord<T>* greenRecord, RIVRecord<T>* blueRecord) {
		redColorProperty = new RIVEvaluatedProperty<T>(referenceTable,redRecord);
		greenColorProperty = new RIVEvaluatedProperty<T>(referenceTable,greenRecord);
		blueColorProperty = new RIVEvaluatedProperty<T>(referenceTable,blueRecord);
    }
    bool ComputeColor(RIVTableInterface* table, size_t row, riv::Color& color) {
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
	void Reset(RIVDataSet<float,ushort>* newDataSource) {
		redColorProperty->Reset(newDataSource);
		greenColorProperty->Reset(newDataSource);
		blueColorProperty->Reset(newDataSource);
	}
};


#endif /* defined(__Afstuderen__ColorProperty__) */
