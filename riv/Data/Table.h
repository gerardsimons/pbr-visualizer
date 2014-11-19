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
#include "Iterator.h"
#include "DataSetListener.h"
#include "Cluster.h"

#include "../helper.h"
#include "../reporter.h"

class RIVReference;
class RIVReferenceChain;
class RIVDataView;
class TableIterator;

namespace riv {
	class Filter;
	class GroupFilter;
}

	class RIVTable {
	private:
		std::vector<RIVRecord*> records;
		std::vector<RIVReference*> references;
		std::vector<RIVDataSetListener*> onChangeListeners;
		std::vector<size_t> selectedRows;
		RIVClusterSet clusterSet;
		
		bool filtered = false;
		bool isClustered = false;
		
		TableIterator* iterator = NULL;
		
		std::vector<riv::Filter*> filters;
		std::vector<riv::GroupFilter*> groupFilters;
		
		std::string name;
		std::vector<std::string> attributes;
		
		std::map<size_t,bool> filteredRows;
		std::vector<size_t> newlyFilteredRows;
		
//		void filterRecords(); //Filter on all filters present in the filters vector
	public:
		RIVTable(std::string name);
		RIVRecord* GetRecord(size_t index);
		RIVRecord* GetRecord(std::string recordName);
		template <typename T>
		T* GetRecord(std::string name) {
			return dynamic_cast<T*>(GetRecord(name));
		}
		//Filter this table according to the filters that are applied
		void Filter();
		void FilterReferences();
		void AddRecord(RIVRecord* record);
		bool ContainsFilter(riv::Filter* filter);
		void AddFilter(riv::Filter *filter);
		void AddFilter(riv::GroupFilter *groupFilter);
//		void AddReference(const RIVReference* reference);
		void AddReference(RIVReference* reference);
		void AddOnChangeListeners(RIVDataView *dataview);
		std::vector<std::string> GetAttributes() const;
		
		void FilterRow(size_t row);
		void SelectRow(size_t row);
		//    void UnfilterRow(size_t,RIVReference* reference = 0);
		void FilterRowsUnlinkedTo(RIVTable *table);
		
		RIVCluster* ClusterForRow(const size_t& row) const;
		
		//Clears all the filters that may be present, returns true if any filters were actually removed
		bool ClearFilters();
		//Clears all the filters with the given attribute name, returns true if any filter was actually removed
		bool ClearFilter(const std::string& filterName);
		bool ClearFilter(size_t fid);
		void ClearFilteredRows();
		RIVRecord* GetRecord(std::string name) const;
		bool ContainsColumn(std::string);
		
		/* Casting help functions */
		static RIVFloatRecord* CastToFloatRecord(RIVRecord* record);
		static RIVUnsignedShortRecord* CastToUnsignedShortRecord(RIVRecord* record);
		
		float PercentageFiltered();
		bool IsFiltered() { return filtered; }; //Any filters applied?
		bool IsClustered() { return isClustered; };
		bool HasRecord(RIVRecord* record);
		bool HasRecord(const std::string& name);
		void FunctionOnRecords(void(*someFunction)(const RIVRecord*));
		
		const std::vector<RIVReference*>* GetReferences();
		TableIterator* GetIterator();
//		TableIterator* GetPIterator();
		std::string GetName() const { return name; };
		
		size_t GetNumRows() const;
		
		//    RIVReference* GetReferenceToTable(std::string tableName,std::vector<std::string> *visitedTables = 0); //DEPRECATED
		bool GetReferenceChainToTable(std::string tableName, RIVReferenceChain& chainToTarget, std::vector<std::string> *visitedTables = 0);
		RIVReference* GetReferenceToTable(const std::string& tableName);
		
		const RIVTable* FindTable(std::string tableName, std::vector<std::string> *visitedTables = 0);
		
		size_t NumberOfColumns(); //Columns
		size_t NumberOfRows();
		std::vector<RIVRecord*> GetRecords() const;
		
		RIVClusterSet& GetClusterSet() { return clusterSet; };
		void ClusterWithSize(const std::string& xRecordName, const std::string& yRecordName, const std::string& zRecordName, const size_t& clusterSize, const size_t& maxRepeat);
		RIVClusterSet* Cluster(const std::string& xRecord, const std::string& yRecord, const std::string& zRecord, const size_t& K, const size_t& maxRepeat);
		
		//Print functions
		void PrintUnfiltered();
		void Print(size_t maxPrint = 1000, bool printFiltered = true); //Print all the rows
		//    void PrintUnfiltered(); //Print only those rows that are not filtered
		void PrintFilteredRowMap(); //Print the underlying filter map
		std::string RowToString(size_t row);
	};

#endif /* defined(__RIVDataSet__Table__) */
