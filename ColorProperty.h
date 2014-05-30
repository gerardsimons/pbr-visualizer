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
    //DEFAULT COLORS
    const static float colorBlue[3];
    const static float colorYellow[3];
    const static float colorBlack[3];
    
    float alternateColor[3];
public:
    virtual float* Color(const RIVTable*,const size_t&) = 0;
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
    std::string colorTableName;
    float colorOne[3];
    float colorTwo[3];
public:
    RIVColorLinearProperty(std::string colorTableName_, float* colorOne_, float* colorTwo_, float* alternateColor_) : RIVColorProperty(alternateColor_){
        colorTableName = colorTableName_;
        memcpy(colorOne, colorOne_, sizeof(colorOne));
        memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
    }
    RIVColorLinearProperty(std::string colorTableName_) : RIVColorProperty() {
        colorTableName = colorTableName_;
        memcpy(colorOne, colorBlue, sizeof(colorOne));
        memcpy(colorTwo, colorYellow, sizeof(colorTwo));
    }
    float* Color(const RIVTable* table, const size_t& row);
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
