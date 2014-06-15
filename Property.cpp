//
//  GraphicsProperty.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 15/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Property.h"

bool RIVInterpolatedProperty::Value(RIVTable* sourceTable, const size_t& row, float& computedValue) {
    //Determine what interpolator we should use to compute the color
    if(sourceTable->GetName() == propertyReference->GetName()) {
        Interpolator<size_t>* colorInterpolator = interpolatorMapping[row];
        if(colorInterpolator == NULL) {
            colorInterpolator = defaultInterpolator;
        }
        computedValue = colorInterpolator->Interpolate(row);
        return true;
    }
    else {
        //Find the table through its chain of references
        RIVReferenceChain chainToColorTable;
        //If found
        if(sourceTable->GetReferenceChainToTable(propertyReference->GetName(),chainToColorTable)) {
            //Find target index
            std::vector<size_t> targetRange = chainToColorTable.ResolveRow(row);
            if(targetRange.size() > 0){
                size_t propertyIndex = (targetRange)[0]; //Very arbitrary, pass all and average?
                //Repeat using the new row and table
                return Value(propertyReference,targetRange,computedValue);
            }
        }
    }
    return false;
}
bool RIVInterpolatedProperty::Value(RIVTable* sourceTable, const std::vector<size_t>& rows,float& value) {
    //    float const* overallColor = NULL;
    Interpolator<size_t>* propertyInterpolator = NULL;
    size_t rowFound;
    for(size_t i = 0 ; i < rows.size() ; ++i) {
        rowFound = rows[i];
        //Give preference to special interpolators
        propertyInterpolator = interpolatorMapping[rowFound];
        if(propertyInterpolator != NULL) break;
    }
    float interpolatedValue;
    if(propertyInterpolator == NULL) {
        value = defaultInterpolator->Interpolate(rowFound);
    }
    else {
        value = propertyInterpolator->Interpolate(rowFound);
    }
    return true;
}
void RIVInterpolatedProperty::AddInterpolationScheme(std::vector<size_t>& indices, Interpolator<size_t>* newInterpolator) {
    specialInterpolators.push_back(newInterpolator);
    for(size_t i : indices) {
        interpolatorMapping[i] = newInterpolator;
    }
}
void RIVInterpolatedProperty::init(RIVTable* propertyReference_, const INTERPOLATION_SCHEME &scheme, const std::vector<size_t> &interpolationValues) {
    propertyReference = propertyReference_;
    switch(scheme) {
        case CONTINUOUS:
        {
            defaultInterpolator = new LinearInterpolator<size_t>(interpolationValues);
            break;
        }
        case DISCRETE:
        {
            defaultInterpolator = new DiscreteInterpolator<size_t>(interpolationValues);
            break;
        }
            
    }
}
RIVInterpolatedProperty::RIVInterpolatedProperty(RIVTable *propertyReference_,std::vector<size_t>& interpolationValues) {
    init(propertyReference_, defaultInterpolationMode, interpolationValues);
}
RIVInterpolatedProperty::RIVInterpolatedProperty(RIVTable *propertyReference_) {
    size_t lower = 0;
    size_t upper = propertyReference_->GetNumRows();
    std::vector<size_t> interpolationValues;
    interpolationValues.push_back(lower);
    interpolationValues.push_back(upper);
    init(propertyReference_, defaultInterpolationMode, interpolationValues);
}
RIVInterpolatedProperty::RIVInterpolatedProperty(RIVTable *propertyReference_,std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme) {
    init(propertyReference_, scheme, interpolationValues);
}

float const* Value(RIVTable* sourceTable, std::vector<size_t>& row);