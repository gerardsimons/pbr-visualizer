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
//Add a filter to the table with the given name
void RIVDataSet::AddFilter(const std::string& tablename, riv::Filter *filter) {
	RIVTable* table = GetTable(tablename);
    //Find the table containing the attribute
	if(!isFiltering) {
		throw "StartFiltering must be called before doing any filter operations";
	}
    if(filter != 0) {
		table->AddFilter(filter);
		staleTables[table] = true;
		return;

    }
    else {
        throw std::string("Supplied filter is a NULL pointer.");
    }
}

//Add a filter to the table with the given name
void RIVDataSet::AddFilter(const std::string& tablename, riv::GroupFilter *filter) {
	RIVTable* table = GetTable(tablename);
    //Find the table containing the attribute
	if(!isFiltering) {
		throw "StartFiltering must be called before doing any filter operations";
	}
    if(filter != 0) {
		table->AddFilter(filter);
		staleTables[table] = true;
		return;
		
    }
    else {
        throw std::string("Supplied filter is a NULL pointer.");
    }
}


void RIVDataSet::AddFilter(riv::Filter *filter) {
	//Find the table that contains all of the filter attributes
	for(RIVTable* table : tables) {
		bool tableFound = true;
		for(const std::string& attribute : filter->GetAttributes()) {
			if(table->HasRecord(attribute) == false) {
				tableFound = false;
				break;
			}
		}
		if(tableFound) {
			table->AddFilter(filter);
			staleTables[table] = true;
			return;
		}
	}
	throw "Could not find a table that contains all of the filter's attributes";
}

void RIVDataSet::AddFilter(riv::GroupFilter *filter) {
	//Find the table that contains all of the filter attributes
	for(RIVTable* table : tables) {
		bool tableFound = true;
		for(const std::string& attribute : filter->GetAttributes()) {
			if(table->HasRecord(attribute) == false) {
				tableFound = false;
				break;
			}
		}
		if(tableFound) {
			table->AddFilter(filter);
			staleTables[table] = true;
			return;
		}
	}
	throw "Could not find a table that contains all of the filter's attributes";
}

void RIVDataSet::ClearFilters() {
    printf("Clearing all filters on the dataset.\n");
    for(RIVTable* table : tables) {
        if(table->ClearFilters()) {
			staleTables[table] = true;
		}
    }
}

void RIVDataSet::ClearFilter(size_t filterID) {
	printf("Clearing filter %d on all tables\n",filterID);
    for(RIVTable *table : tables) {
        if(table->ClearFilter(filterID)) {
			staleTables[table] = true;
		}
    }
}

void RIVDataSet::ClearFilter(std::string filterName) {
    printf("Clearing filter %s on all tables\n",filterName.c_str());
    for(RIVTable *table : tables) {
        if(table->ClearFilter(filterName)) {
			staleTables[table] = true;
		}
    }
}

size_t RIVDataSet::TotalNumberOfRecords() const {
    size_t size = 0;
    for(RIVTable *table : tables) {
        size += table->NumberOfColumns();
    }
    return size;
}

std::vector<RIVTable*>* RIVDataSet::GetTables() {
    return &tables;
}

size_t RIVDataSet::NumberOfTables() const {
    return tables.size();
}

void RIVDataSet::notifyListeners() {
    for(RIVDataSetListener *listener : onFilterListeners) {
        listener->OnDataSetChanged();
    }
}

void RIVDataSet::AddFilterListener(RIVDataSetListener* listener) {
    if(listener) {
        onFilterListeners.push_back(listener);
    }
}

void RIVDataSet::Print(size_t maxPrint, bool printFiltered) const {
    printf("Dataset containing %zu tables:\n\n",tables.size());
    for(RIVTable *table : tables) {
        table->Print(maxPrint, printFiltered);
    }
}

RIVRecord* RIVDataSet::FindRecord(std::string name) const {
    RIVRecord* record = 0;
    for(RIVTable* table : tables) {
        record = table->GetRecord(name);
        if(record) {
            return record;
        }
    }
    return record;
}

RIVTable* RIVDataSet::GetTable(const std::string& tableName) const {
    for(RIVTable *table : tables) {
        if(table->GetName() == tableName) {
            return table;
        }
    } //No such table
    throw "No such table exists";
}

bool RIVDataSet::IsSet() const {
    return tables.size() != 0;
}

RIVClusterSet* RIVDataSet::GetClusterSet() {
    return clusterSet;
}

void RIVDataSet::ClusterTable(const std::string& tableName, const std::string& columnNameX, const std::string& columnNameY, const std::string& columnNameZ, const size_t& K, const size_t& maxRepeat) {
    RIVTable *table = GetTable(tableName);
    if(!table) {
        throw "No such table.";
    }
    else {
        clusterSet = table->Cluster(columnNameX, columnNameY, columnNameZ, K, maxRepeat);
    }
}

void RIVDataSet::StartFiltering() {
	if(isFiltering) {
		throw "Invalid state, StopFiltering was never called.";
	}
	isFiltering = true;
	staleTables.clear();
}


void RIVDataSet::StopFiltering() {
	if(!isFiltering) {
		throw "Invalid state, StartFiltering was never called.";
	}
	//Finalize, do the actual filtering operations
	for(auto iter : staleTables) {
		iter.first->filterRecords();
	}
	
	isFiltering = false;
	//Notify the listeners now
	notifyListeners();
}