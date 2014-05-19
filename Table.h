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

#include "Filter.h"
#include "Record.h"
#include "Reference.h"
#include "helper.h"

class RIVReference;

class TableIterator {
protected:
    size_t index = 0;
    size_t maxIndex = 0;
public:
    TableIterator(size_t _maxIndex) { maxIndex = _maxIndex; };
    void BackToStart() { index = 0; };
    virtual bool HasNext() {
//        printf("TableIterator HasNext called.\n");
        return index < maxIndex;
    };
    virtual size_t GetNext() {
        size_t temp = index;
        index++;
        return temp;
    }
    virtual void Print() const {
        printf("TableIterator (maxIndex = %zu) \n", maxIndex);
    }
};

class FilteredTableIterator : public TableIterator {
private:
    std::map<size_t,bool>* indexPointers;
public:
    FilteredTableIterator(std::map<size_t,bool> * _indexPointers) : TableIterator(_indexPointers->size()){
        indexPointers = _indexPointers;
        index = 0;
    };
    
    bool HasNext() {
        if(index >= maxIndex) {
            return false;
//            printf("End of iterator!\n");
        }
        else {
            bool filtered = (*indexPointers)[index];
            if(!filtered) {
                return true;
//                printf("Unfiltered value found at %hz!\n",index);
            }
            else {
                ++index;
                return HasNext();
            }
        }
    }
    
    size_t GetNext() {
        bool filtered = (*indexPointers)[index];
        while(filtered) {
            filtered = (*indexPointers)[index];
            index++;
        }
//        printf("return next unfiltered index = %hz\n",index);
        index++;
        return index - 1;
    }
    
    void Print() const {
        printf("FilteredTableIterator index = %hz map of iterator object has %zu values :\n", index, indexPointers->size());
        printMap(*indexPointers);
    }
};

class RIVTable {
private:
    std::vector<RIVRecord*> records;
    std::vector<RIVReference*> references;
    
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
    
    void FilterRow(size_t,RIVReference* reference = 0);
    
    void ClearFilters();
    void ClearFilter(std::string filterName);
    
    bool ContainsColumn(std::string);
    
    /* Casting help functions */
    static RIVFloatRecord* CastToFloatRecord(RIVRecord* record);
    static RIVUnsignedShortRecord* CastToUnsignedShortRecord(RIVRecord* record);
    
    bool IsFiltered() { return filteredRows.size() > 0; }; //Any filters applied?

    TableIterator* GetIterator();
    void FunctionOnRecords(void(*someFunction)(const RIVRecord*));
    
    std::string GetName() { return name; };
    
    size_t NumberOfColumns(); //Columns
    size_t NumberOfRows();
    
    //Print functions
    void PrintAll(); //Print all the rows
    void PrintUnfiltered(); //Print only those rows that are not filtered
    void PrintFilteredRowMap(); //Print the underlying filter map
    std::string RowToString(size_t row);
};

#endif /* defined(__RIVDataSet__Table__) */
