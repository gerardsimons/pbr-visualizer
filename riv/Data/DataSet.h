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
#include "ClusterSet.h"



//Forward declaration
class RIVDataView;
class RIVTable;
class RIVRecord;

class RIVDataSet {
private:
    RIVClusterSet* clusterSet = NULL;
    std::vector<RIVTable*> tables;
    std::vector<RIVDataSetListener*> dataListeners;
	
	//What tables are changed
	std::map<RIVTable*,bool> staleTables;
	
	
	void notifyFilterListeners();
	void notifyDataListeners();
	
	bool isFiltering = false;
public:
    void AddTable(RIVTable* table);
	void AddFilter(const std::string& tableName, riv::GroupFilter *filter);
    void AddFilter(const std::string& tableName, riv::Filter* filter);
	//Automatically find the table this should be filtered on, the one containing all of the filters attributes
	void AddFilter(riv::Filter* filter);
	void AddFilter(riv::GroupFilter *filter);
    void AddFilterListener(RIVDataSetListener* listener);
	void UpdateFilter(riv::Filter* filter);
    void ClearFilters();
	void ClearFilter(size_t filterID);
    void ClearFilter(const std::string& attributeName);
    RIVRecord* FindRecord(std::string name) const;
    size_t TotalNumberOfRecords() const;
    size_t NumberOfTables() const;
    std::vector<RIVTable*>* GetTables();
    RIVTable* GetTable(const std::string&) const;
    void Print(size_t maxPrint = -1, bool printFiltered = true) const;
    void ClusterTable(const std::string& tableName, const std::string& columnNameX, const std::string& columnNameY, const std::string& columnNameZ, const size_t& K, const size_t& maxRepeat);
    RIVClusterSet* GetClusterSet();
    bool IsSet() const;
	
	//In order to coalesce several filter operations into one notification to its listeners, its necessary to indicate the start and stop of a filteirng operation
	void StartFiltering();
	void StopFiltering();
//    void PrintUnfiltered();
};

#endif /* defined(__RIVDataSet__DataSet__) */
