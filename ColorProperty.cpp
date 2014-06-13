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

void RIVColorLinearProperty::init(const INTERPOLATION_SCHEME& scheme, const std::vector<size_t>& interpolationValues) {
    switch(scheme) {
        case CONTINUOUS:
        {
            colorInterpolator = new LinearInterpolator<size_t>(interpolationValues);
        }
        case DISCRETE:
        {
            colorInterpolator = new DiscreteInterpolator<size_t>(interpolationValues);
        }
            
    }
}

RIVColorLinearProperty::RIVColorLinearProperty(RIVTable *colorReference_, float* colorOne_, float* colorTwo_, float* alternateColor_) : RIVColorProperty(alternateColor_){
    //        colorTableName = colorTableName_;
    colorReference = colorReference_;
    memcpy(colorOne, colorOne_, sizeof(colorOne));
    memcpy(colorTwo, colorTwo_, sizeof(colorTwo));
    size_t lower = 0;
    size_t upper = colorReference->GetNumRows();
    std::vector<size_t> interpolationValues;
    interpolationValues.push_back(lower);
    interpolationValues.push_back(upper);
    init(CONTINUOUS,interpolationValues);
    clusterColorInterpolator = NULL;
}

RIVColorLinearProperty::RIVColorLinearProperty(RIVTable* colorReference_) : RIVColorProperty() {
    colorReference = colorReference_;
    memcpy(colorOne, colorBlue, sizeof(colorOne));
    memcpy(colorTwo, colorYellow, sizeof(colorTwo));
    size_t lower = 0;
    size_t upper = colorReference->GetNumRows();
    std::vector<size_t> interpolationValues;
    interpolationValues.push_back(lower);
    interpolationValues.push_back(upper);
    init(CONTINUOUS,interpolationValues);
    clusterColorInterpolator = NULL;
}

RIVColorLinearProperty::RIVColorLinearProperty(RIVTable* colorReference_, std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme) : RIVColorProperty() {
    colorReference = colorReference_;
    memcpy(colorOne, colorBlue, sizeof(colorOne));
    memcpy(colorTwo, colorYellow, sizeof(colorTwo));
    init(scheme,interpolationValues);
    clusterColorInterpolator = NULL;
}

//This is the public function usually called by views that base their color on some table (the reference table).
//Color is either based on the row (or the cluster of the row) of the reference table. If a link exists between the given table & row
//and the reference table. Additionally information about the cluster may be given, in order to avoid unnecessary look ups when the given
//table is the color reference table.
float const* RIVColorLinearProperty::Color(RIVTable* table, const size_t& row) {
    if(colorByClusterMode) {
        return colorByCluster(table,row);
    }
    else {
        return colorByRow(table, row);
    }
}

float const* RIVColorLinearProperty::colorByRow(RIVTable* sourceTable, const size_t& row) {
    if(sourceTable->GetName() == colorReference->GetName()) {
        float ratio = colorInterpolator->Interpolate(row);
        return linearInterpolateColor(ratio, colorOne, colorTwo);
    }
    else {
        //Find the table through its references
        RIVReferenceChain chainToColorTable;
        if(sourceTable->GetReferenceChainToTable(colorReference->GetName(),chainToColorTable)) {
            //Find target index
            const std::vector<size_t>& targetRange = chainToColorTable.ResolveRow(row);
            if(targetRange.size() > 0){
                size_t colorIndex = (targetRange)[0];
                //Repeat using the new row and table
                return Color(colorReference,colorIndex);
            }
        }
    }
    if(useAlternateColors)
        return alternateColor;
    else
        return NULL;
}

float const* RIVColorLinearProperty::colorByCluster(RIVTable* sourceTable,const size_t& row) {
    if(clusterColorInterpolator == NULL) {
        initClusterColorInterpolator();
    }
    if(sourceTable->GetName() == colorReference->GetName()) {
        RIVCluster* cluster = sourceTable->ClusterForRow(row);
        size_t mIndex = cluster->GetMedoidIndex();
        float interpolatedValue = clusterColorInterpolator->Interpolate(mIndex);
        return linearInterpolateColor(interpolatedValue,colorOne,colorTwo);
    }
    else {
        //Find cluster in color reference table, assuming it has one
        RIVReferenceChain chainToColorTable;
        if(sourceTable->GetReferenceChainToTable(colorReference->GetName(),chainToColorTable)) {
            //Find target index
            const std::vector<size_t>& targetRange = chainToColorTable.ResolveRow(row);
            if(targetRange.size() > 0){
                size_t colorIndex = (targetRange)[0];
                //Repeat using the new row and table
                return Color(colorReference,colorIndex);
            }
        }
//        RIVReference* reference = table->GetReferenceToTable(colorReference->GetName());
//        if(colorReference) {
//            std::vector<size_t>* targetRange = reference->GetIndexReferences(row);
//            if(targetRange && targetRange->size() > 0){
//                //Ask the clusterset what cluster the target index belongs to
//                return colorByCluster(reference->targetTable, (*targetRange)[0]);
//            }
//        }
    }
    if(useAlternateColors)
        return alternateColor;
    else
        return NULL;
}

void RIVColorLinearProperty::initClusterColorInterpolator() {
    RIVClusterSet* clusterSet = colorReference->GetClusterSet();
    const std::vector<size_t>& medoids = clusterSet->GetMedoidIndices();
    clusterColorInterpolator = new DiscreteInterpolator<size_t>(medoids);
}

void RIVColorLinearProperty::EnableColorByCluster() {
    if(colorReference->IsClustered()) {
        if(!colorByClusterMode) {
            initClusterColorInterpolator();
            colorByClusterMode = true;
        }
    }
    else throw "Table is not a clustered table.";
}
