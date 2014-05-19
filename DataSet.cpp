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
    for(RIVTable *table : tables) {
        if(table->ContainsColumn(filter->attributeName)) {
            //Apply the filter to the table)
            table->AddFilter(filter);
        }
    }
}

void RIVDataSet::ClearFilters() {
    for(RIVTable* table : tables) {
        table->ClearFilters();
    }
}

void RIVDataSet::ClearFilter(std::string filterName) {
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

void RIVDataSet::Print() {
    printf("Dataset containing %zu tables:\n\n",tables.size());
    for(RIVTable *table : tables) {
        table->PrintAll();
    }
}

void RIVDataSet::PrintUnfiltered() {
    printf("Dataset containing %zu tables:\n\n",tables.size());
    for(RIVTable *table : tables) {
        table->PrintUnfiltered();
    }
}