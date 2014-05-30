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

float* RIVColorLinearProperty::Color(const RIVTable* table, const size_t& row) {
    float ratio = 0.F;
    if(table->GetName() == colorTableName) {
        ratio = row / (float) table->GetNumRows();
        return linearInterpolateColor(ratio, colorOne, colorTwo);
    }
    else {
        //Find the table
        const RIVReference* colorReference = table->GetReferenceToTable(colorTableName);
        if(colorReference) {
            std::vector<size_t>* targetRange = colorReference->GetIndexReferences(row);
            if(targetRange){
                size_t colorIndex = (*targetRange)[0];
                ratio = colorIndex / (float)colorReference->targetTable->GetNumRows();
                return linearInterpolateColor(ratio, colorOne, colorTwo);
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