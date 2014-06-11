//
//  ColorProperty.h
//  Afstuderen
//
//  Created by Gerard Simons on 29/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__ColorProperty__
#define __Afstuderen__ColorProperty__

#include "Record.h"
#include "Table.h"
#include "helper.h"
#include <stdio.h>
#include <string>

class RIVTable;

enum INTERPOLATION_SCHEME {
    LINEAR,
    BILINEAR,
    TRILINEAR,
    RGB
};

class RIVColorProperty {
protected:

    float alternateColor[3];
public:
    //DEFAULT COLORS
    const static float colorBlue[3];
    const static float colorYellow[3];
    const static float colorBlack[3];
    
    virtual float const* Color(const RIVTable*,const size_t&) = 0;
    virtual void Setup() {};
    RIVColorProperty(float* alternateColor_) {
        memcpy(alternateColor,alternateColor_,sizeof(alternateColor));
    }
    RIVColorProperty() {
        memcpy(alternateColor,colorBlack,sizeof(alternateColor));
    }
};

class RIVColorLinearProperty : public RIVColorProperty {
private:
    //The table whose index defines the color
    RIVTable* colorReference;

    float colorOne[3];
    float colorTwo[3];
    
    LinearInterpolator<size_t>* colorInterpolator;
//    std::vector<float[3]> interpolatedColors;
public:
    RIVColorLinearProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_);
    ~RIVColorLinearProperty() {
        delete colorInterpolator;
    }
    RIVColorLinearProperty(RIVTable *colorReference_);
    RIVColorLinearProperty(RIVTable *colorReference_,std::vector<size_t>& interpolationValues);
    float const* Color(const RIVTable* sourceTable, const size_t& row);
};

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
