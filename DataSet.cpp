//
//  DataSet.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "DataSet.h"
#include "Table.h"

void RIVDataSet::AddTable(RIVTable* table) {
    tables.push_back(table);
}

void RIVDataSet::AddFilter(Filter *filter) {
    //Find the table containing the attribute
    if(filter != 0) {
        for(RIVTable *table : tables) {
            if(table->ContainsColumn(filter->attributeName)) {
                //Apply the filter to the table)
                table->AddFilter(filter);
            }
        }
        notifyListeners();
    }
    else {
        throw std::string("Supplied filter was a NULL pointer.");
    }
}

void RIVDataSet::ClearFilters() {
    printf("Clearing all filters on the dataset.\n");
    for(RIVTable* table : tables) {
        table->ClearFilters();
    }
}

void RIVDataSet::ClearFilter(std::string filterName) {
    printf("Clearing filter %s on all tables\n",filterName.c_str());
    for(RIVTable *table : tables) {
        table->ClearFilter(filterName);
    }

}

size_t RIVDataSet::TotalNumberOfRecords() {
    size_t size = 0;
    for(RIVTable *table : tables) {
        size += table->NumberOfColumns();
    }
    return size;
}

std::vector<RIVTable*>* RIVDataSet::GetTables() {
    return &tables;
}

size_t RIVDataSet::NumberOfTables() {
    return tables.size();
}

void RIVDataSet::notifyListeners() {
    for(RIVDataView *dataview : onFilterListeners) {
        dataview->OnFilterChange();
    }
}

void RIVDataSet::AddFilterListener(RIVDataView* view) {
    onFilterListeners.push_back(view);
}

void RIVDataSet::Print(size_t maxPrint, bool printFiltered) {
    printf("Dataset containing %zu tables:\n\n",tables.size());
    for(RIVTable *table : tables) {
        table->Print(maxPrint, printFiltered);
    }
}

RIVRecord* RIVDataSet::FindRecord(std::string name) {
    for(RIVTable* table : tables) {
        RIVRecord* record = table->GetRecord(name);
        if(record) return record;
    }
    return 0;
}
/*
void RIVDataSet::PrintUnfiltered() {
    printf("Dataset containing %zu tables:\n\n",tables.size());
    for(RIVTable *table : tables) {
        table->PrintUnfiltered();
    }
}
*/