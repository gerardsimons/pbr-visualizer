//
//  Reference.h
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef RIVDataSet_Reference_h
#define RIVDataSet_Reference_h

#include <stdio.h>
#include <vector>

#include "Table.h"

class RIVTable;

class RIVReference {
public:
    RIVTable *sourceTable;
    RIVTable *targetTable;
    
    std::vector<size_t> sourceIndices;
    std::vector<size_t> targetIndices;
    
    RIVReference(RIVTable *_sourceTable, RIVTable *_targetTable);
    void AddReference(size_t sourceIndex, size_t targetIndex);
};

#endif
