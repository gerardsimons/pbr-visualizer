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
#include <set>

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
    
    static int ID_COUNTER;
    
    std::vector<RIVTable<Ts...>*> tables;
    std::map<RIVTable<Ts...>*,bool> staleTables;
    
    std::vector<RIVDataSetListener*> dataListeners;
    void notifyFilterListeners() {
        for(RIVDataSetListener *listener : dataListeners) {
            listener->OnFiltersChanged(this);
        }
    }
    
    bool isFiltering = false;
    std::string name;
public:
    int datasetID;
    RIVDataSet(const std::string& name) : name(name) {
        datasetID = ID_COUNTER++;
    }
    ~RIVDataSet() {
        deletePointerVector(tables);
        //		printf("delete pointer vector\n");
    }
    std::string& GetName() {
        return name;
    }
    std::vector<RIVDataSetListener*> GetDataListeners() {
        return dataListeners;
    }
    void SetDataListeners(const std::vector<RIVDataSetListener*>& listeners) {
        dataListeners = listeners;
    }
    void NotifyDataListeners() {
        for(RIVDataSetListener* listener : dataListeners) {
            listener->OnDataChanged(this);
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
    bool HasTable(const std::string& tableName) {
        return GetTable(tableName) != NULL;
    }
    RIVTable<Ts...>* CreateTable(const std::string& tableName) {
        if(HasTable(tableName)) {
            std::string error = tableName + " already exists";
            throw std::runtime_error(error);
        }
        else {
            RIVTable<Ts...>* newTable = new RIVTable<Ts...>(tableName);
            AddTable(newTable);
            return newTable;
        }
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
    void AddFilter(riv::RowFilter* rowFilter) {
        for(auto table : tables) {
            if(rowFilter->AppliesToTable(table)) {
                table->AddFilter(rowFilter);
                staleTables[table] = true;
                return;
            }
        }
    }
    template<typename T>
    void AddDisjunctiveFilter(riv::CompoundFilter<T>* disjunctiveFilter) {
        for(auto table : tables) {
            if(disjunctiveFilter->AppliesToTable(table)) {
                table->AddDisjunctiveFilter(disjunctiveFilter);
                staleTables[table] = true;
                return;
            }
        }
    }
    template<typename T>
    void AddConjunctiveFilter(riv::CompoundFilter<T>* conjunctiveFilter) {
        for(auto table : tables) {
            if(conjunctiveFilter->AppliesToTable(table)) {
                table->AddConjunctiveFilter(conjunctiveFilter);
                staleTables[table] = true;
                return;
            }
        }
    }
    //Automatically find the table this should be filtered on, the one containing all of the filters attributes
    //	void AddFilter(riv::Filter* filter);
    //	void AddFilter(riv::GroupFilter *filter);
    void AddDataListener(RIVDataSetListener* listener) {
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
        if(staleTables.size()) {
            //TODO: Fix this
            //Finalize, do the actual filtering operations
            for(auto iter : staleTables) {
                iter.first->Filter();
            }
            
            //		Make sure linked tables are filtered accordingly
            for(auto iter : staleTables) {
                iter.first->FilterReferences();
            }
            
            //Notify the listeners now
            notifyFilterListeners();
        }
        isFiltering = false;
    }
    bool IsFiltered() {
        for(auto table : tables) {
            if(table->IsFiltered()) {
                return true;
            }
        }
        return false;
    }
    void DeleteTable(const std::string& tableName) {
        for(size_t i = 0 ; i < tables.size() ; ++i) {
            auto table = tables[i];
            if(table->name == tableName) {
                delete table;
                tables.erase(tables.begin() + i);
            }
        }
    }
    template<typename T>
    void ClearFilter(const std::string& filterName) {
        //		printf("Clearing filter %s on all tables\n",filterName.c_str());
        if(isFiltering) {
            for(RIVTable<Ts...>* table : tables) {
                if(table->template ClearFilter<T>(filterName)) {
                    staleTables[table] = true;
                    for(RIVReference* reference : table->references) {
                        if(reference)
                            staleTables[(RIVTable<Ts...>*)reference->targetTable] = true;
                    }
                }
            }
        }
        else {
            throw std::runtime_error("Need to start filtering first");
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
            numRecords += table->NumberOfRecords();
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
    void ClearAllFilters() {
        if(isFiltering) {
            for(auto table : tables) {
                table->ClearFilters();
                staleTables[table] = true;
            }
        }
        else {
            throw std::runtime_error("Need to start filtering first");
        }
    }
    void Print(size_t maxPrint = 0, bool printFiltered = true) {
        printf("Dataset containing %zu tables:\n\n",tables.size());
        for(auto table : tables) {
            table->Print(maxPrint, printFiltered);
        }
    }
    void PrintFilteredRows(size_t maxPrint = 0) {
        printf("Dataset containing %zu tables:\n\n",tables.size());
        for(auto table : tables) {
            table->PrintFilteredRows(maxPrint);
        }
    }
    void AddDataSet(RIVDataSet* otherDataset) {
        for(RIVTable<Ts...>* table : tables) {
            RIVTable<Ts...>* otherTable = otherDataset->GetTable(table->name);
            table->JoinTable(otherTable);
        }
    }
    void ClearRowFilters() {
        if(isFiltering) {
            for(auto table : tables) {
                table->ClearRowFilters();
                staleTables[table] = true;
            }
        }
        else {
            throw std::runtime_error("Need to start filtering first");
        }
    }
    void ClearRowFilter(riv::RowFilter* existingFilter) {
        if(isFiltering) {
            if(existingFilter) {
                for(auto table : tables) {
                    if(table->ClearRowFilter(existingFilter)) {
                        for(RIVReference* reference : table->references) {
                            staleTables[table] = true;
                            if(reference) {
                                staleTables[(RIVTable<Ts...>*)reference->targetTable] = true;
                            }
                        }
                    }
                }
            }
        }
        else {
            throw std::runtime_error("Need to start filtering first");
        }
    }
    RIVDataSet* CloneStructure() {
        RIVDataSet<Ts...>* clone = new RIVDataSet<Ts...>(name);
        for(RIVTable<Ts...>* table : tables) {
            clone->AddTable(table->CloneStructure());
        }
        return clone;
    }
    RIVDataSet* CloneStructure(const std::set<std::string>& tablesToClone) {
        RIVDataSet<Ts...>* clone = new RIVDataSet<Ts...>(name);
        for(RIVTable<Ts...>* table : tables) {
            if(tablesToClone.find(table->name) != tablesToClone.end()) {
                clone->AddTable(table->CloneStructure());
            }
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
    HistogramSet<Ts...> CreateHistogramSetFromFiltered(int bins, const std::set<std::string>& tableNamesToUse) {
        HistogramSet<Ts...> histograms;
        for(auto table : tables) {
            if(tableNamesToUse.find(table->name) != tableNamesToUse.end()) {
                auto histogramset = table->CreateHistogramSetFromFiltered(bins);
                //			histogramset.Print();
                histograms.Join(histogramset);
            }
        }
        return histograms;
    }
    HistogramSet<Ts...> CreateHistogramSet(int bins, const std::set<std::string>& tableNamesToUse) {
        HistogramSet<Ts...> histograms;
        for(auto table : tables) {
            if(tableNamesToUse.find(table->name) != tableNamesToUse.end()) {
                auto histogramset = table->CreateHistogramSet(bins);
                //			histogramset.Print();
                histograms.Join(histogramset);
            }
        }
        return histograms;
    }
    HistogramSet<Ts...> CreateEmptyHistogramSet(int bins, const std::set<std::string>& tableNamesToUse) {
        HistogramSet<Ts...> histograms;
        for(auto table : tables) {
            if(tableNamesToUse.find(table->name) != tableNamesToUse.end()) {
                auto histogramset = table->CreateEmptyHistogramSet(bins);
                //			histogramset.Print();
                histograms.Join(histogramset);
            }
        }
        return histograms;
    }
    void MarkTableAsStale(RIVTable<Ts...>* table) {
        staleTables[table] = true;
    }
    void CopyFiltersTo(RIVDataSet<Ts...>* dataset) {
        dataset->StartFiltering();
        for(auto table : tables) {
            //            auto otherTable = dataset->GetTable(table->name);
            //            dataset->MarkTableAsStale(otherTable);
            auto& allFilters = *table->GetAllFilters();
            tuple_for_each(allFilters, [&](auto tFilters) {
                for(auto filter : tFilters) {
                    dataset->AddFilter(filter);
                }
            });
        }
        dataset->StopFiltering();
    }
};

template<typename ...Ts>
int RIVDataSet<Ts...>::ID_COUNTER = 0;

#endif /* defined(__RIVDataSet__DataSet__) */
