//
//  DataSet.h
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __RIVDataSet__DataSet__
#define __RIVDataSet__DataSet__

#include <vector>
#include "Table.h"
#include "Filter.h"

class RIVDataSet {
private:
    std::vector<RIVTable*> tables;
public:
    void AddTable(RIVTable* table);
    void AddFilter(Filter* filter);
    void ClearFilters();
    void ClearFilter(std::string attributeName);
    size_t TotalNumberOfRecords();
    size_t NumberOfTables();
    std::vector<RIVTable*>* GetTables();
    void Print();
    void PrintUnfiltered();
};

#endif /* defined(__RIVDataSet__DataSet__) */
