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
#include <map>

#include "Table.h"

class RIVTable;

class RIVReference {
public:
    //The source table is implicit as a reference is supplied to a table
    RIVTable *targetTable;
    RIVTable *sourceTable;
    
    ~RIVReference() {
        delete indexReferences; //Very important as index references is created as new in reverse function
    }
    
    std::map<size_t,std::vector<size_t>>* indexReferences;
    
    RIVReference(RIVTable *_sourceTable, RIVTable *_targetTable);
    bool HasReference(size_t) const; //Does it have a reference from a given source index
//    void AddReference(size_t sourceIndex, size_t targetIndex);
    void SetReferences(std::map<size_t,std::vector<size_t>>*);
    std::vector<size_t>* GetIndexReferences(size_t) const;
    RIVReference* ReverseReference();
};

#endif
