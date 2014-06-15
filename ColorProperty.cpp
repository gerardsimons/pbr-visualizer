//
//  ColorProperty.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 29/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ColorProperty.h"
#include "Property.h"

const float RIVColorProperty::colorBlue[3] = {0,0,1};
const float RIVColorProperty::colorYellow[3] = {1,1,0};
const float RIVColorProperty::colorBlack[3] = {0,0,0};


RIVInterpolatedColorProperty::RIVInterpolatedColorProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_) : RIVColorProperty(alternateColor_), RIVInterpolatedProperty(colorReference_){
    memcpy(colorOne, colorOne_, sizeof(colorOne));
    memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
//    clusterColorInterpolator = NULL;
}
RIVInterpolatedColorProperty::RIVInterpolatedColorProperty(RIVTable *colorReference_, float const* colorOne_, float const* colorTwo_) : RIVColorProperty(), RIVInterpolatedProperty(colorReference_){
    memcpy(colorOne, colorOne_, sizeof(colorOne));
    memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
}
//This constructor is unnecessary, explicitly call other constructor with blue and yellow from color palette instead
RIVInterpolatedColorProperty::RIVInterpolatedColorProperty(RIVTable* colorReference_) : RIVColorProperty(), RIVInterpolatedProperty(colorReference_) {
    memcpy(colorOne, colorBlue, sizeof(colorOne));
    memcpy(colorTwo, colorYellow, sizeof(colorTwo));
}
//Same goes for this one
RIVInterpolatedColorProperty::RIVInterpolatedColorProperty(RIVTable* colorReference_, std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme) : RIVColorProperty(), RIVInterpolatedProperty(colorReference_, interpolationValues) {
    memcpy(colorOne, colorBlue, sizeof(colorOne));
    memcpy(colorTwo, colorYellow, sizeof(colorTwo));
}

//float const* RIVInterpolatedColorProperty::Color(RIVTable* sourceTable, const size_t& row) {
//    //Determine what interpolator we should use to compute the color
//    if(sourceTable->GetName() == colorReference->GetName()) {
//        Interpolator<size_t>* colorInterpolator = interpolatorMapping[row];
//        if(colorInterpolator == NULL) {
//            colorInterpolator = defaultColorInterpolator;
//        }
//        float ratio = colorInterpolator->Interpolate(row);
//        return linearInterpolateColor(ratio, colorOne, colorTwo);
//    }
//    else {
//        //Find the table through its chain of references
//        RIVReferenceChain chainToColorTable;
//        //If found
//        if(sourceTable->GetReferenceChainToTable(colorReference->GetName(),chainToColorTable)) {
//            //Find target index
//            std::vector<size_t> targetRange = chainToColorTable.ResolveRow(row);
//            
//            //
//            
//            
//            if(targetRange.size() > 0){
//                size_t colorIndex = (targetRange)[0]; //Very arbitrary, pass all and average?
//                //Repeat using the new row and table
//                return Color(colorReference,colorIndex);
//            }
//        }
//    }
//    //Fall through, the reference table was not found for the original source table
//    if(useAlternateColors)
//        return alternateColor;
//    else
//        return NULL;
//
//}

float const* RIVInterpolatedColorProperty::Color(RIVTable* sourceTable, const size_t& row) {
    float value;
    if(RIVInterpolatedProperty::Value(sourceTable, row,value)) {
        return linearInterpolateColor(value, colorOne, colorTwo);
    }
    else if(useAlternateColors) {
            return alternateColor;
    }
    else return NULL;
}


//float const* RIVInterpolatedColorProperty::colorForMultipleResolvedRows(const std::vector<size_t>& rows) {
////    float const* overallColor = NULL;
//    Interpolator<size_t>* colorInterpolator = NULL;
//    size_t rowFound;
//    for(size_t i = 0 ; i < rows.size() ; ++i) {
//        rowFound = rows[i];
//        //Give preference to special interpolators
//        colorInterpolator = specialInterpolators[rowFound];
//        if(colorInterpolator != NULL) break;
//    }
//    float interpolatedValue;
//    if(colorInterpolator == NULL) {
//        interpolatedValue = defaultColorInterpolator->Interpolate(rowFound);
//    }
//    else {
//        interpolatedValue = colorInterpolator->Interpolate(rowFound);
//    }
//    float ratio = colorInterpolator->Interpolate(rowFound);
//    return linearInterpolateColor(ratio, colorOne, colorTwo);
//}
//
//
////Move this, this should not be here.
//void RIVInterpolatedColorProperty::initClusterColorInterpolator() {
//    RIVClusterSet* clusterSet = propertyReference->GetClusterSet();
//    std::vector<size_t> medoids = clusterSet->GetMedoidIndices();
//    
//    AddInterpolationScheme(medoids, new DiscreteInterpolator<size_t>(medoids));
//}
