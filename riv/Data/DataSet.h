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
#include <tuple>

#include "Histogram.h"
#include "TupleIterator.h"
#include "Table.h"
#include "Filter.h"
#include "DataSetListener.h"

//Forward declaration
class RIVDataView;

template <typename ...Ts>
class RIVDataSet {
private:
	
//	std::tuple<std::vector<RIVTable<Ts...>>> tables;
//	std::vector<RIVTable<Ts...>> tables;
	std::vector<RIVTable<Ts...>*> tables;
	std::map<RIVTable<Ts...>*,bool> staleTables;
	
    std::vector<RIVDataSetListener*> dataListeners;
	void notifyFilterListeners() {
		for(RIVDataSetListener *listener : dataListeners) {
			listener->OnFiltersChanged();
		}
	}

	//What tables are changed
//	std::map<RIVTable*,bool> staleTables;
//	std::map<std::string,RIVTable*> tableRegister;

	
	bool isFiltering = false;
	

public:
//	RIVDataSet* Bootstrap(size_t N);
	~RIVDataSet() {
		deletePointerVector(tables);
	}
	std::vector<RIVDataSetListener*> GetDataListeners() {
		return dataListeners;
	}
	void SetDataListeners(const std::vector<RIVDataSetListener*>& listeners) {
		dataListeners = listeners;
	}
	void NotifyDataListeners() {
		for(RIVDataSetListener* listener : dataListeners) {
			listener->OnDataChanged();
		}
	}
	void AddTable(RIVTable<Ts...>* table) {
		auto tables = GetTables();
		tables->push_back(table);
	}
	template<typename U>
	std::vector<RIVTable<U>>* GetTables() {
		return &std::get<std::vector<RIVTable<U>>>(tables);
	}
	std::vector<RIVTable<Ts...>*>* GetTables() {
		return &tables;
	}
	RIVTable<Ts...>* GetTable(const std::string& name) {
		for(auto table : tables) {
			if(table->name == name) {
				return table;
			}
		}
		return NULL;
	}
	RIVTable<Ts...>* CreateTable(const std::string& tableName) {
		RIVTable<Ts...>* newTable = new RIVTable<Ts...>(tableName);
		AddTable(newTable);
		return newTable;
	}
//	template<typename T>
//	void AddFilter(const std::string& tableName, riv::GroupFilter<T> *filter) {
//		
//	}
	template<typename T>
	void AddFilter(riv::SingularFilter<T>* filter) {
		//Find the table that contains all of the filter attributes
		for(auto table : tables) {
			if(filter->AppliesToTable(table)) {
				table->AddFilter(filter);
				staleTables[table] = true;
				return;
			}
		}
		throw std::runtime_error("Filter applies to no table in this dataset.");
	}
	template<typename... Us>
	void UpdateFilter(riv::GroupFilter<Us...>* filter) {
		for(auto table : tables) {
			if(table->HasFilter(filter)) {
				staleTables[table] = true;
			}
		}
	}
	template<typename... Us>
	void AddFilter(riv::GroupFilter<Us...>* filter) {
		//Find the table that contains all of the filter attributes
		for(auto table : tables) {
			if(filter->AppliesToTable(table)) {
				table->AddFilter(filter);
				staleTables[table] = true;
				return;
			}
		}
		throw std::runtime_error("Filter applies to no table in this dataset.");
	}
	//Automatically find the table this should be filtered on, the one containing all of the filters attributes
//	void AddFilter(riv::Filter* filter);
//	void AddFilter(riv::GroupFilter *filter);
	void AddFilterListener(RIVDataSetListener* listener) {
		dataListeners.push_back(listener);
	}
//	void UpdateFilter(riv::Filter* filter);
	
	void StartFiltering() {
		if(isFiltering) {
			throw std::runtime_error("Invalid state, StopFiltering was never called.");
		}
		isFiltering = true;
		staleTables.clear();
	}
	
	bool IsEmpty() {
		for(auto table : tables) {
			if(!table->IsEmpty()) {
				return false;
			}
		}
		return true;
	}
	void StopFiltering() {
		if(!isFiltering) {
			throw "Invalid state, StartFiltering was never called.";
		}
		//TODO: Fix this
		//Finalize, do the actual filtering operations
		for(auto iter : staleTables) {
			iter.first->Filter();
		}
	
//		Make sure linked tables are filtered accordingly
		for(auto iter : staleTables) {
			iter.first->FilterReferences();
		}
	
		isFiltering = false;
		//Notify the listeners now
		notifyFilterListeners();
	}
	
//	void ClearFilters() {
//		printf("Clearing all filters on the dataset.\n");
//		for(RIVTable* table : tables) {
//			if(table->ClearFilters()) {
//				staleTables[table] = true;
//				const std::vector<RIVReference*>* refs = table->GetReferences();
//				for(size_t i = 0 ; i < refs->size() ; ++i) {
//					RIVReference* ref = refs->at(i);
//					staleTables[ref->targetTable] = true;
//				}
//			}
//		}
//	}
	
//	void ClearFilter(size_t filterID) {
//		printf("Clearing filter %zu on all tables\n",filterID);
//		for(RIVTable *table : tables) {
//			if(table->ClearFilter(filterID)) {
//				staleTables[table] = true;
//				const std::vector<RIVReference*>* refs = table->GetReferences();
//				for(size_t i = 0 ; i < refs->size() ; ++i) {
//					RIVReference* ref = refs->at(i);
//					staleTables[ref->targetTable] = true;
//				}
//			}
//		}
//	}
	
	template<typename T>
	void ClearFilter(const std::string& filterName) {
		printf("Clearing filter %s on all tables\n",filterName.c_str());
		
		for(RIVTable<Ts...>* table : tables) {
			if(table->template ClearFilter<T>(filterName)) {
				staleTables[table] = true;
				RIVReference* reference = table->reference;
				staleTables[(RIVTable<Ts...>*)reference->targetTable] = true;
			}
		}
	}
	
	void ClearData() {
		for(RIVTable<Ts...>* table : tables) {
			table->ClearData();
		}
	}
//    RIVRecord* FindRecord(std::string name) const;
	size_t TotalNumberOfRecords() const {
		size_t numRecords = 0;
		for(auto* table : tables) {
			numRecords += table->NumberOfColumns();
		}
		return numRecords;
	}
    size_t NumberOfTables() const;
//	template<typename U>
//    std::vector<RIVTable*>* GetTables();
	template<typename T>
	RIVTable<T>* GetTable(const std::string& tableName) {
		std::vector<RIVTable<T>>* tables = GetTables<T>();
		for(size_t j = 0 ; j < tables->size() ; ++j) {
			RIVTable<T>* table = &tables->at(j);
			if(table->name == tableName) {
				return table;
			}
		}
		return NULL;
	}
	void Print(size_t maxPrint = 0, bool printFiltered = true) {
		printf("Dataset containing %zu tables:\n\n",tables.size());
		for(auto table : tables) {
			table->Print(maxPrint, printFiltered);
		}
	}
	void AddDataSet(RIVDataSet* otherDataset) {
		for(RIVTable<Ts...>* table : tables) {
			RIVTable<Ts...>* otherTable = otherDataset->GetTable(table->name);
			table->JoinTable(otherTable);
		}
	}
	RIVDataSet* CloneStructure() {
		RIVDataSet<Ts...>* clone = new RIVDataSet<Ts...>();
		for(RIVTable<Ts...>* table : tables) {
			clone->AddTable(table->CloneStructure());
		}
		return clone;
	}
	
	HistogramSet<Ts...> CreateHistogramSet(int bins) {
		HistogramSet<Ts...> histograms;
		for(auto table : tables) {
			auto histogramset = table->CreateHistogramSet(bins);
//			histogramset.Print();
			histograms.Join(histogramset);
		}
		return histograms;
	}
//    void PrintUnfiltered();
};

#endif /* defined(__RIVDataSet__DataSet__) */
