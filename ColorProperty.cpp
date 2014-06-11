//
//  ColorProperty.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 29/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ColorProperty.h"

const float RIVColorProperty::colorBlue[3] = {0,0,1};
const float RIVColorProperty::colorYellow[3] = {1,1,0};
const float RIVColorProperty::colorBlack[3] = {0,0,0};

RIVColorLinearProperty::RIVColorLinearProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_) : RIVColorProperty(alternateColor_){
    //        colorTableName = colorTableName_;
    colorReference = colorReference_;
    memcpy(colorOne, colorOne_, sizeof(colorOne));
    memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
    size_t lower = 0;
    size_t upper = colorReference->GetNumRows();
    colorInterpolator = new LinearInterpolator<size_t>(lower,upper);
}

RIVColorLinearProperty::RIVColorLinearProperty(RIVTable* colorReference_) : RIVColorProperty() {
    colorReference = colorReference_;
    memcpy(colorOne, colorBlue, sizeof(colorOne));
    memcpy(colorTwo, colorYellow, sizeof(colorTwo));
    size_t lower = 0;
    size_t upper = colorReference->GetNumRows();
    colorInterpolator = new LinearInterpolator<size_t>(lower,upper);
}

RIVColorLinearProperty::RIVColorLinearProperty(RIVTable* colorReference_, std::vector<size_t>& interpolationValues) : RIVColorProperty() {
    colorReference = colorReference_;
    memcpy(colorOne, colorBlue, sizeof(colorOne));
    memcpy(colorTwo, colorYellow, sizeof(colorTwo));
    colorInterpolator = new LinearInterpolator<size_t>(interpolationValues);
}

float const* RIVColorLinearProperty::Color(const RIVTable* table, const size_t& row) {
    float ratio = 0.F;
    if(table->GetName() == colorReference->GetName()) {
        ratio = row / (float) table->GetNumRows();
        return linearInterpolateColor(ratio, colorOne, colorTwo);
    }
    else {
        //Find the table
        const RIVReference* reference = table->GetReferenceToTable(colorReference->GetName());
        if(table->GetName() == "intersections") {
            
        }
        if(colorReference) {
            std::vector<size_t>* targetRange = reference->GetIndexReferences(row);
            if(targetRange){
                size_t colorIndex = (*targetRange)[0];
//                ratio = colorIndex / (float)reference->targetTable->GetNumRows();
                
//                return linearInterpolateColor(ratio, colorOne, colorTwo);
                float interpolated = colorInterpolator->Interpolate(colorIndex);
                float R = interpolated * colorOne[0] + (1-interpolated) * colorTwo[0];
                float G = interpolated * colorOne[1] + (1-interpolated) * colorTwo[1];
                float B = interpolated * colorOne[2] + (1-interpolated) * colorTwo[2];
                float color[3] = {R,G,B};
                return color;
            }
        }
    }
    return alternateColor;
}

float* RIVColorRGBProperty::Color(const RIVTable* table, const size_t& row) {
    size_t tableRow = row;
    //Find correct table    
    if(table->GetName() != colorTableName) {
        const RIVReference* ref = table->GetReferenceToTable(colorTableName);
        if(ref) {
            std::vector<size_t> *indices = ref->GetIndexReferences(row);
            if(indices && indices->size() != 0) {
                tableRow = indices->at(0);
            }
            else {
                //No reference!
                return alternateColor;
            }
        }
        else {
            throw "Color reference could not be resolved.";
        }
    }
    float R = redRecord->ScaleValue(tableRow);
    float G = greenRecord->ScaleValue(tableRow);
    float B = blueRecord->ScaleValue(tableRow);

//    printf("[R,G,B] row %zu = [%d,%d,%d]\n",tableRow,(int)(R * 255),(int)(G * 255), (int)(B * 255));
    float color[3] = {R,G,B};
    
    return color;
}