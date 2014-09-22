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

	class RIVTable {
	private:
		std::vector<RIVRecord*> records;
		std::vector<RIVReference> references;
		std::vector<RIVDataSetListener*> onChangeListeners;
		std::vector<size_t> selectedRows;
		RIVClusterSet clusterSet;
		
		bool filtered = false;
		bool isClustered = false;
		
		TableIterator* iterator;
		
		size_t rows = 0; //Keeps a running count of the rows (that is the length of a record)
		
		std::vector<riv::Filter*> filters;
		
		std::string name;
		
		std::map<size_t,bool> filteredRows;
		
		void filterRecords(riv::Filter *);
		void filterRecords(); //Filter on all filters present in the filters vector
	public:
		RIVTable(std::string name);
		RIVRecord* GetRecord(size_t index);
		RIVRecord* GetRecord(std::string recordName);
		template <typename T>
		T* GetRecord(std::string name) {
			return dynamic_cast<T*>(GetRecord(name));
		}
		
		void AddRecord(RIVRecord* record);
		void AddFilter(riv::Filter *filter);
		void AddReference(const RIVReference& reference);
		void AddOnChangeListeners(RIVDataView *dataview);
		
		void FilterRow(size_t,bool filter,RIVReference* reference = 0);
		//    void UnfilterRow(size_t,RIVReference* reference = 0);
		void FilterRowsUnlinkedTo(RIVTable *table);
		
		RIVCluster* ClusterForRow(const size_t& row) const;
		
		void ClearFilters();
		void ClearFilter(std::string filterName);
		RIVRecord* GetRecord(std::string name) const;
		bool ContainsColumn(std::string);
		
		/* Casting help functions */
		static RIVFloatRecord* CastToFloatRecord(RIVRecord* record);
		static RIVUnsignedShortRecord* CastToUnsignedShortRecord(RIVRecord* record);
		
		bool IsFiltered() { return filtered; }; //Any filters applied?
		bool IsClustered() { return isClustered; };
		bool HasRecord(RIVRecord* record);
		
		void FunctionOnRecords(void(*someFunction)(const RIVRecord*));
		
		const std::vector<RIVReference>* GetReferences();
		TableIterator* GetIterator();
		std::string GetName() const { return name; };
		size_t GetNumRows() const { return rows; };
		
		//    RIVReference* GetReferenceToTable(std::string tableName,std::vector<std::string> *visitedTables = 0); //DEPRECATED
		bool GetReferenceChainToTable(std::string tableName, RIVReferenceChain& chainToTarget, std::vector<std::string> *visitedTables = 0);
		RIVReference* GetReferenceToTable(const std::string& tableName);
		
		const RIVTable* FindTable(std::string tableName, std::vector<std::string> *visitedTables = 0);
		
		size_t NumberOfColumns(); //Columns
		size_t NumberOfRows();
		std::vector<RIVRecord*> GetRecords();
		
		RIVClusterSet& GetClusterSet() { return clusterSet; };
		void ClusterWithSize(const std::string& xRecordName, const std::string& yRecordName, const std::string& zRecordName, const size_t& clusterSize, const size_t& maxRepeat);
		RIVClusterSet* Cluster(const std::string& xRecord, const std::string& yRecord, const std::string& zRecord, const size_t& K, const size_t& maxRepeat);
		
		//Print functions
		void Print(size_t maxPrint = 1000, bool printFiltered = true); //Print all the rows
		//    void PrintUnfiltered(); //Print only those rows that are not filtered
		void PrintFilteredRowMap(); //Print the underlying filter map
		std::string RowToString(size_t row);
	};

#endif /* defined(__RIVDataSet__Table__) */
