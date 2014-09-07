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

class RIVRecord;

//Interface for a color property
class RIVColorProperty {
protected:
    float alternateColor[3];
    RIVColorProperty(float* alternateColor_) {
        memcpy(alternateColor,alternateColor_,sizeof(alternateColor));
    }
    RIVColorProperty() {
        memcpy(alternateColor,colorBlack,sizeof(alternateColor));
    }
public:
    //DEFAULT COLORS
    const static float colorBlue[3];
    const static float colorYellow[3];
    const static float colorBlack[3];
    
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
    
    std::vector<const float*> colorMap; //basically a Nx3 color table

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
        
        colorMap.resize(2);
        colorMap[0] = colorOne_;
        colorMap[1] = colorOne_;
    }
    RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, float const* colorOne_, float const* colorTwo_) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
//        memcpy(colorOne, colorOne_, sizeof(colorOne));
//        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
        
        colorMap.resize(2);
        colorMap[0] = colorOne_;
        colorMap[1] = colorTwo_;
    }
    RIVEvaluatedColorProperty(RIVTable *colorReference_, RIVRecord* record, const std::vector<const float*>& colorMap) : RIVColorProperty(), RIVEvaluatedProperty<T>(colorReference_,record){
        //        memcpy(colorOne, colorOne_, sizeof(colorOne));
        //        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
        
        this->colorMap = colorMap;
    }
    
    float const* Color(RIVTable* sourceTable, const size_t& row) {
//        float value;
//        if(RIVEvaluatedProperty<T>::Value(sourceTable, row,value)) {
//            return linearInterpolateColor(value, colorOne, colorTwo);
//        }
//        else if(useAlternateColors) {
//            return alternateColor;
//        }
//        else return NULL;
        float value; //Assuming this value will be between 0 and 1
        float color[3] = {0,1,0}; //Red
        if(RIVEvaluatedProperty<T>::Value(sourceTable,row,value))  {
//            printf("Color value = %f\n",value);
            if(sourceTable->GetName() == "intersections" && value == 1.F) {
                
            }
            for(size_t colorIndex = 0 ; colorIndex < colorMap.size() - 1; ++colorIndex) {
                
                float colorIndexRatioLeft  = colorIndex / (float)(colorMap.size() - 1);
                float colorIndexRatioRight = (colorIndex + 1) / (float)(colorMap.size() - 1);
                float ratio = value;
                if(colorIndexRatioLeft <= value && colorIndexRatioRight >= value) {
                    //Its in between these two indices, use these to interpolate
//                    return linearInterpolateColor(value, colorMap[colorIndex], colorMap[colorIndex+1]);
                    ratio = 1.F - ratio;
                    color[0] = colorMap[colorIndex][0] * ratio + (1.F - ratio) * colorMap[colorIndex + 1][0];
                    color[1] = colorMap[colorIndex][1] * ratio + (1.F - ratio) * colorMap[colorIndex + 1][1],
                    color[2] = colorMap[colorIndex][2] * ratio + (1.F - ratio) * colorMap[colorIndex + 1][2];
                    return color;
                }
            }
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
class RIVColorRGBProperty : public RIVColorProperty {
private:
    std::string colorTableName;

    RIVRecord* blueRecord;
    RIVRecord* redRecord;
    RIVRecord* greenRecord;
public:
    RIVColorRGBProperty(std::string tableName_, RIVRecord *redRecord_, RIVRecord* greenRecord_,RIVRecord *blueRecord_) {
        colorTableName = tableName_;
        redRecord = redRecord_;
        blueRecord = blueRecord_;
        greenRecord = greenRecord_;
    }
    float *Color(const RIVTable* table, const size_t& row);
};

#endif /* defined(__Afstuderen__ColorProperty__) */
