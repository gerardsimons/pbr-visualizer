//
//  File.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Reference.h"


RIVReference::RIVReference(RIVTable *_sourceTable, RIVTable *_targetTable) {
    
    sourceTable = _sourceTable;
    targetTable = _targetTable;
}

void RIVReference::AddReference(size_t sourceIndex, size_t targetIndex) {
    sourceIndices.push_back(sourceIndex);
    targetIndices.push_back(targetIndex);
}