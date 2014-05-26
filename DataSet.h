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
#include "DataView.h"

//Forward declaration
class RIVDataView;
class RIVTable;

class RIVDataSet {
private:
    std::vector<RIVTable*> tables;
    std::vector<RIVDataView*> onFilterListeners;
    void notifyListeners();
public:
    void AddTable(RIVTable* table);
    void AddFilter(Filter* filter);
    void AddFilterListener(RIVDataView* view);
    void ClearFilters();
    void ClearFilter(std::string attributeName);
    size_t TotalNumberOfRecords();
    size_t NumberOfTables();
    std::vector<RIVTable*>* GetTables();
    void Print();
    void PrintUnfiltered();
};

#endif /* defined(__RIVDataSet__DataSet__) */
