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

//void RIVReference::AddReference(size_t sourceIndex, size_t targetIndex) {
//    (*indexReferences)[sourceIndex] = targetIndex;
//}

void RIVReference::SetReferences(std::map<size_t,std::vector<size_t>>* references) {
    this->indexReferences = references;
}

std::vector<size_t>* RIVReference::GetIndexReferences(size_t sourceIndex) const {
    if(HasReference(sourceIndex)) {
        return &(*indexReferences)[sourceIndex];
    }
    else return 0;
}

bool RIVReference::HasReference(size_t sourceIndex) const {
    return indexReferences != 0 && indexReferences->count(sourceIndex) != 0;
}

RIVReference* RIVReference::ReverseReference() {
    RIVReference* reverse = new RIVReference(targetTable,sourceTable);
    
    std::map<size_t,std::vector<size_t>> *reverseIndexReferences = new std::map<size_t,std::vector<size_t>>();
    
    for(auto it = indexReferences->begin() ; it != indexReferences->end() ; ++it) {
        std::vector<size_t> indexRange = it->second;
        bool firstRun = true;
        for(size_t i : indexRange)  {
            firstRun = false;
//            printf("indexRange i = %d\n",i);
//            printf("indexRange.second = %d\n",indexRange.second);
            std::vector<size_t> range;
            range.push_back(it->first);
            (*reverseIndexReferences)[i] = range;
        }
    }
//
    reverse->SetReferences(reverseIndexReferences);
    
    return reverse;
}