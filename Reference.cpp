//
//  File.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Reference.h"


RIVReference::RIVReference(RIVTable* _sourceTable, RIVTable *_targetTable) {
    sourceTable = _sourceTable;
    targetTable = _targetTable;
}

void RIVReference::AddReference(size_t sourceIndex, size_t targetIndex) {
    (*indexReferences)[sourceIndex] = targetIndex;
}

void RIVReference::SetReferences(std::map<size_t,size_t>* references) {
    this->indexReferences = references;
}

size_t* RIVReference::GetIndexReference(size_t sourceIndex) {
    if(HasReference(sourceIndex)) {
        return &(*indexReferences)[sourceIndex];
    }
    else return 0;
}

bool RIVReference::HasReference(size_t sourceIndex) {
    return indexReferences->count(sourceIndex) != 0;
}
