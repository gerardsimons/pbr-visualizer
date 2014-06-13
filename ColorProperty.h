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
#include "Interpolator.h"

class RIVTable;

class RIVColorProperty {
protected:
    float alternateColor[3];
public:
    //DEFAULT COLORS
    const static float colorBlue[3];
    const static float colorYellow[3];
    const static float colorBlack[3];
    
    virtual float const* Color(RIVTable*,const size_t&) = 0;
    virtual void EnableColorByCluster() = 0;
    virtual void Setup() {};
    RIVColorProperty(float* alternateColor_) {
        memcpy(alternateColor,alternateColor_,sizeof(alternateColor));
    }
    RIVColorProperty() {
        memcpy(alternateColor,colorBlack,sizeof(alternateColor));
    }
};

enum INTERPOLATION_SCHEME {
    DISCRETE,
    CONTINUOUS
};

//Color changes linearly from colorOne to colorTwo either by using a linear interpolator to create a smooth gradient,
//or by using a discrete interpolator which rounds interpolated values down to create discretized values
class RIVColorLinearProperty : public RIVColorProperty {
private:
    //The table whose index defines the color
    RIVTable* colorReference;
    
    //The type of interpolation used
    INTERPOLATION_SCHEME interpolationMode;
    
    //If this bool is set, colorreference should contain a clusterset, and coloring is based on the cluster a row is part of (or linked to indirectly)
    //Rather than
    bool colorByClusterMode;
    //When a row has no reference to the color reference through linkage, should a default color be returned or simply NULL?
    bool useAlternateColors = false;

    //The two color to interpolate between //TODO: better names?
    float colorOne[3];
    float colorTwo[3];
    
    //Regular interpolator, linearly interpolates the color of a given row
    Interpolator<size_t>* colorInterpolator;
    //Used to determine the color for a certain cluster member
    Interpolator<size_t>* clusterColorInterpolator;
    
    void initClusterColorInterpolator();
    
    //helper function, shared by constructors to avoid duplicate code
    void init(const INTERPOLATION_SCHEME& scheme, const std::vector<size_t>& interpolationValues);
    //Find the cluster this row is part of if any, this assumes the referenceTable is a clustered table (contains a clusterset)
    float const* colorByCluster(RIVTable* sourceTable, const size_t& row);
    //This method looks to find the row linked to the given source table's row, or simply the row if the table is the color reference table.
    float const* colorByRow(RIVTable* sourceTable, const size_t& row);
public:
    RIVColorLinearProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_ ,INTERPOLATION_SCHEME mode);
    ~RIVColorLinearProperty() {
        delete colorInterpolator;
    }
    RIVColorLinearProperty(RIVTable *colorReference_);
    RIVColorLinearProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_);
    RIVColorLinearProperty(RIVTable *colorReference_,std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme);
    
    void EnableColorByCluster();
    float const* Color(RIVTable* sourceTable, const size_t& row);
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
