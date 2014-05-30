//
//  Table.h
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __RIVDataSet__Table__
#define __RIVDataSet__Table__

#include <vector>
#include <map>
#include <stdio.h>

#include <math.h>

#include "DataView.h"
#include "Filter.h"
#include "Record.h"
#include "Reference.h"
#include "helper.h"

class RIVReference;
class RIVDataView;

class TableIterator {
protected:
    size_t index = 0;
    size_t maxIndex = 0;
public:
    TableIterator(size_t _maxIndex) { maxIndex = _maxIndex; };
    void BackToStart() { index = 0; };
    //    virtual bool HasNext() {
    ////        printf("TableIterator HasNext called.\n");
    //        return index < maxIndex;
    //    };
    virtual bool GetNext(size_t& row) {
        if(index >= maxIndex) {
            return false;
        }
        else {
            row = index;
            index++;
            return (index - 1) < maxIndex;
        }
    }
    virtual void Print() const {
        printf("TableIterator (maxIndex = %zu) \n", maxIndex);
    }
};

class FilteredTableIterator : public TableIterator {
private:
    std::map<size_t,bool>* indexPointers;
public:
    FilteredTableIterator(std::map<size_t,bool> * _indexPointers, size_t maxIndex) : TableIterator(maxIndex){
        indexPointers = _indexPointers;
        index = 0;
    };
    
    virtual bool GetNext(size_t &row) {
        if(index < maxIndex) {
            bool filtered = (*indexPointers)[index];
            while(filtered && index < maxIndex) {
                index++;
                filtered = (*indexPointers)[index];
                //                printf("index = %zu\n",index);
            }
            row = index;
            index++;
            return !filtered && index <= maxIndex;
        }
        return false;
    }
    
    void Print() const {
        printf("FilteredTableIterator index = %zu map of iterator object has %zu values :\n", index, maxIndex);
        printMap(*indexPointers);
    }
};

class RIVTable {
private:
    std::vector<RIVRecord*> records;
    std::vector<RIVReference*> references;
    std::vector<RIVDataView*> onChangeListeners;
    
    bool filtered = false;
    
    TableIterator* iterator;
    
    size_t rows = 0; //Keeps a running count of the rows (that is the length of a record)
    
    std::vector<Filter*> filters;
    
    std::string name;
    
    std::map<size_t,bool> filteredRows;
    
    void filterRecords(Filter *);
    void filterRecords(); //Filter on all filters present in the filters vector
public:
    RIVTable(std::string name);;
    RIVRecord* GetRecord(size_t index) ;
    
    void AddRecord(RIVRecord* record);
    void AddFilter(Filter *filter);
    void AddReference(RIVReference* reference);
    void AddOnChangeListeners(RIVDataView *dataview);
    
    void FilterRow(size_t,bool filter,RIVReference* reference = 0);
//    void UnfilterRow(size_t,RIVReference* reference = 0);
    void FilterRowsUnlinkedTo(RIVTable *table);
    
    void ClearFilters();
    void ClearFilter(std::string filterName);
    RIVRecord* GetRecord(std::string name) const;
    bool ContainsColumn(std::string);
    
    /* Casting help functions */
    static RIVFloatRecord* CastToFloatRecord(RIVRecord* record);
    static RIVUnsignedShortRecord* CastToUnsignedShortRecord(RIVRecord* record);
    
    bool IsFiltered() { return filtered; }; //Any filters applied?
    void FunctionOnRecords(void(*someFunction)(const RIVRecord*));

    const std::vector<RIVReference*>* GetReferences();
    TableIterator* GetIterator();
    std::string GetName() const { return name; };
    size_t GetNumRows() const { return rows; };
    const RIVReference* GetReferenceToTable(std::string tableName,std::vector<std::string> *visitedTables = 0) const;
    const RIVTable* FindTable(std::string tableName, std::vector<std::string> *visitedTables = 0);
    
    size_t NumberOfColumns(); //Columns
    size_t NumberOfRows();
    std::vector<RIVRecord*> GetRecords();
    
    //Print functions
    void Print(size_t maxPrint = 1000, bool printFiltered = true); //Print all the rows
//    void PrintUnfiltered(); //Print only those rows that are not filtered
    void PrintFilteredRowMap(); //Print the underlying filter map
    std::string RowToString(size_t row);
};

#endif /* defined(__RIVDataSet__Table__) */
