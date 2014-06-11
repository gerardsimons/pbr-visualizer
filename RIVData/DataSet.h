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
#include "DataSetListener.h"

//Forward declaration
class RIVDataView;
class RIVTable;
class RIVRecord;

class RIVDataSet {
private:
    std::vector<RIVTable*> tables;
    std::vector<RIVDataSetListener*> onFilterListeners;
    void notifyListeners();
public:
    void AddTable(RIVTable* table);
    void AddFilter(Filter* filter);
    void AddFilterListener(RIVDataSetListener* listener);
    void ClearFilters();
    void ClearFilter(std::string attributeName);
    RIVRecord* FindRecord(std::string name) const;
    size_t TotalNumberOfRecords() const;
    size_t NumberOfTables() const;
    std::vector<RIVTable*>* GetTables();
    RIVTable* GetTable(const std::string&) const;
    void Print(size_t maxPrint = 1000, bool printFiltered = true) const;
    bool IsSet() const;
//    void PrintUnfiltered();
};

#endif /* defined(__RIVDataSet__DataSet__) */
