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
#include <tuple>

#include <math.h>

#include "TupleIterator.h"

#include "Filter.h"
#include "Record.h"
#include "Reference.h"
#include "Iterator.h"
#include "DataSetListener.h"

#include "../helper.h"
#include "../reporter.h"

//class RIVReferenceChain;
//class RIVDataView;
class TableIterator;

template<template<typename...> class C,typename T>
struct apply_args;

template<template<typename...> class C,typename R,typename... Args>
struct apply_args<C, R(Args...) >
{
	typedef C<Args...> type;
};


class RIVTableInterface {
public:
	const std::string name;
	RIVReference* reference = NULL;
//	size_t ID = ++IDCOUNTER;
	
	virtual bool HasRecord(const std::string& name) = 0;
	virtual void FilterRow(size_t row) = 0;
	virtual size_t NumberOfRows() const = 0;
protected:
	RIVTableInterface(const std::string& name) : name(name) {
		
	}
};

template <typename... Ts>
class RIVTable : public RIVTableInterface {
private:
	std::tuple<std::vector<RIVRecord<Ts>*>...> records;
	
	const size_t tupleSize = sizeof...(Ts);
	
	std::vector<RIVDataSetListener*> onChangeListeners;
	std::vector<size_t> selectedRows;
	
	bool filtered = false;
	bool isClustered = false;
	
	TableIterator* iterator = NULL;
	
	std::tuple<std::vector<riv::Filter<Ts>*>...> filters; //Use pointers because Filter is an abstract class
	std::tuple<std::vector<riv::GroupFilter<Ts>>...> groupFilters;

	std::vector<std::string> attributes;
	
	std::map<size_t,bool> filteredRows;
	std::vector<size_t> newlyFilteredRows;
	
public:
	RIVTable(std::string name) : RIVTableInterface(name) {
		
	}
	~RIVTable() {
		TupleForEach(filters, [&](auto tFilters) {
			deletePointerVector(tFilters);
		});
	}
	RIVTable* CloneStructure() {
		RIVTable* clone = new RIVTable(name);
		TupleForEach(records, [&](auto records) {
			for(auto& record : records) {
				clone->AddRecord(record);
			}
		});
		return clone;
	}
	std::tuple<std::vector<RIVRecord<Ts>*>...>& GetAllRecords() {
		return records;
	}
	//Helper function
	template<typename U>
	std::vector<RIVRecord<U>*>* GetRecords() {
		return &std::get<std::vector<RIVRecord<U>*>>(records);
	}
	template<typename U>
	RIVRecord<U>* GetRecord(size_t index) {
		std::vector<RIVRecord<U>*>* records = GetRecord<U>();
		if(records->size() > index) {
			return &(records)[index];
		}
		return NULL;
	}
	template<typename T>
	RIVRecord<T>* GetRecord(const std::string& name) {
		std::vector<RIVRecord<T>*>* records = GetRecords<T>();
		for(size_t i = 0 ; i < records->size() ; ++i) {
			RIVRecord<T>* record = records->at(i);
			if(record->name == name) {
				return record;
			}
		}
		//No record with that name was found
		return NULL;
	}
	template<typename T>
	void AddRecord(RIVRecord<T>* record) {
		std::get<std::vector<RIVRecord<T>*>>(records).push_back(record);
		size_t numRows = NumberOfRows();
		if((numRows != 0 && numRows != record->Size()) || record->Size() == 0) {
			throw std::runtime_error("RIVRecord is of invalid size");
		}
	}
	template<typename T>
	RIVRecord<T>* CreateRecord(const std::string& name) {
//		AddRecord<float>(RIVRecord<float>(name));
		std::vector<RIVRecord<T>*>* tRecords = GetRecords<T>();
		RIVRecord<T>* newRecord = new RIVRecord<T>(name);
		tRecords->push_back(newRecord);
		return newRecord;
	}
	template<typename T>
	RIVRecord<T>* CreateRecord(const std::string& name, T min, T max,bool clampOutliers = false) {
		//		AddRecord<float>(RIVRecord<float>(name));
		std::vector<RIVRecord<T>*>* tRecords = GetRecords<T>();
		RIVRecord<T>* newRecord = new RIVRecord<T>(name,min,max,clampOutliers);
		tRecords->push_back(newRecord);
		return newRecord;
	}
	bool IsEmpty() {
		bool empty = true;
		TupleForEach(records, [&](auto tRecords) {
			if(tRecords.size()) {
				empty = false;
				return;
			}
		});
		return empty;
	}
	//Filter this table according to the filters that are applied
	void Filter() {
		filteredRows.clear();
		newlyFilteredRows.clear();
		std::string task = "Filter " + name;
		reporter::startTask(task);
			printf("Filtering table %s with filter:\n",name.c_str());
//			for(riv::Filter* f : filters) {
//				printf("\t");
//				f->Print();
//			}
//			printf("\n");
			size_t rows = NumberOfRows();
			for(size_t row = 0 ; row < rows ; row++) {
				if(filteredRows[row]) {
					//				printf("row %zu was already filtered\n",row);
					continue; //Already filtered
				}
				bool filterSourceRow = false;
				//Group filters
//				for(riv::GroupFilter* groupFilter : groupFilters) {
//					//				printf("Group filtering : ");
//					//				groupFilter->Print();
//					if(!groupFilter->PassesFilter(this, row)) {
//						//					printf("row = %zu FILTERED\n",row);
//						filterSourceRow = true;
//						break;
//					}
//					else {
//						//					printf("row = %zu SELECTED\n",row);
//					}
//				}
	
				if(!filterSourceRow) {
					TupleForEach(filters, [&](auto tFilters) {
						for(auto filter : tFilters) {
							//If the filter applies to this table, filter according to
							if(filter->AppliesToTable(this)) {
								auto recordForFilter = GetRecord<<#typename T#>>(<#const std::string &name#>)
								filterSourceRow = !filter->PassesFilter(this, row);
								if(filterSourceRow) {
									break;
								}
							}
						}
					});
				}
	
				if(filterSourceRow) {
					//				printf("row = %zu FILTERED\n",row);
					FilterRow(row);
	
					newlyFilteredRows.push_back(row);
				}
				else {
					//				printf("row = %zu SELECTED\n",row);
				}
			}
		
		reporter::stop(task);
	//	printf("After filtering : ");
	//	PrintUnfiltered();
	
	//	printf("References : ");
	//	for(RIVReference* ref : references) {
	//		ref->targetTable->PrintUnfiltered();
	//	}
	}
	void FilterReferences();
	template<typename T>
	std::vector<riv::Filter<T>*>* GetFilters() {
		return &std::get<std::vector<riv::Filter<T>*>>(filters);
	}
	template<typename T>
	bool ContainsFilter(riv::Filter<T>* filter);
	template<typename T>
	void AddFilter(riv::Filter<T> *filter) {
		auto tFilters = GetFilters<T>();
		tFilters->push_back(filter);
	}
	template<typename T>
	void AddFilter(riv::GroupFilter<T> *groupFilter);
//	void AddRow(const std::tuple<std::vector<Ts>...>) {
//		
//	}
	
//	template <typename ...Ts>
//	static void CopyRow(RIVTable<Ts...>* table, RIVTable<Ts...>* otherTable, size_t row) {
//		size_t recordIndex = 0;
//		TupleForEach(records, [&](auto records) {
//			typedef apply_args<records,decltype(myFunc)>::type MyConcrete;
//			++recordIndex;
//		});
//	}
	//		void AddOnChangeListeners(RIVDataView *dataview);
	
	std::vector<std::string> GetAttributes() const;
	
	void FilterRow(size_t row) {
		filteredRows[row] = true;
		filtered = true;
		
	}
	void SelectRow(size_t row);
	//    void UnfilterRow(size_t,RIVReference* reference = 0);
	void FilterRowsUnlinkedTo(RIVTable *table);
	
	//Clears all the filters that may be present, returns true if any filters were actually removed
	bool ClearFilters() {
		TupleForEach(filters, [&](auto filters) {
			filters.clear();
		});
		TupleForEach(groupFilters, [&](auto groupFilters) {
			groupFilters.clear();
		});
	}
	//Clears all the filters with the given attribute name, returns true if any filter was actually removed
	bool ClearFilter(const std::string& filterName) {
		TupleForEach(filters, [&](auto filters) {
			size_t i;
			bool filterFound = false;
			for(i = 0 ; i < filters.size() ; ++i) {
				if(filters[i]->AppliesToAttribute(filterName)) {
					filterFound = true;
					break;
				}
			}
			if(!filterFound) {
				printf("No such filter found.\n");
			}
			else {
				filters.erase(filters.begin() + 1);
			}
		});
	}
	bool ClearFilter(size_t fid);
	void ClearFilteredRows();
	bool ContainsColumn(std::string);
	
	float PercentageFiltered();
	bool IsFiltered() { return filtered; }; //Any filters applied?
	bool IsClustered() { return isClustered; };
	
	bool HasRecord(const std::string& name) {
		TupleForEach(records,[&](auto tRecords) {
			for(auto record : tRecords) {
				if(record->name == name) {
					return true;
				}
			}
		});
		return false;
	}
	
	TableIterator GetIterator() {
		if(IsFiltered()) {
			return FilteredTableIterator(&filteredRows,NumberOfRows(), reference);
		}
		else {
			return TableIterator(NumberOfRows(), reference);
		}
	}
	//		TableIterator* GetPIterator();
	std::string GetName() const { return name; };

	void SetReference(RIVReference* newReference) {
		reference = newReference;
	}
	RIVReference* GetReference() {
		return reference;
	}
//	bool GetReferenceChainToTable(std::string tableName, RIVReferenceChain& chainToTarget, std::vector<std::string> *visitedTables = 0);
	RIVReference* GetReferenceToTable(const std::string& tableName);
	
	const RIVTable* FindTable(std::string tableName, std::vector<std::string> *visitedTables = 0);
	
	size_t NumberOfColumns() const {
		size_t total = 0;
		TupleForEach(records, [&](auto tRecords) {
			total += tRecords.size();
		});
		return total;
	}
	//TODO: This does not work if the table does not use one of its template types (i.e. if no record exists for
	size_t NumberOfRows() const {
		//If the tuple is not empty
		if(tupleSize > 0) {
			//And the record
			auto& t = std::get<0>(records);
			if(t.size())
				return t[0]->Size();
		}
		return 0;
	}
	std::string RowToString(size_t row) {
	    std::string rowText = "";
		TupleForEach(records, [&](auto tRecords) {
			for(auto record : tRecords) {
	
				const int columnWidth = 15;
				std::string valueString;
				size_t textWidth = 0;
				valueString = std::to_string(record->Value(row));
				textWidth = valueString.size();
				int padding = floor((columnWidth - textWidth - 1) / 2.F);
		
				rowText += generateString(' ',padding);
				rowText += valueString;
				rowText += generateString(' ',padding);
		
				int remainder = (columnWidth - textWidth - 1) % 2;
		
				rowText += generateString(' ', remainder);
				rowText += "|";
			}
		});
	    return rowText;
	}
	//Print functions
	void PrintUnfiltered();
	std::string generateString(char c, size_t number) {
	    std::string generatedString;
	
	    for(size_t i = 0 ; i < number ; i++) {
	        generatedString += c;
	    }
	    return generatedString;
	}
	void Print(size_t maxPrint = 1000, bool printFiltered = true) {
		size_t rows = NumberOfRows();
		if(maxPrint == 0) {
			maxPrint = rows;
		}
		printf("Table called %s has %zu records and %zu rows.\n First %zu rows:\n",name.c_str(),NumberOfColumns(),rows,maxPrint);
		std::string headerText = "|";
		std::string headerOrnament;
	
		int columnWidth = 17;
	
		TupleForEach(records, [&](auto tRecords) {
			for(auto record : tRecords) {
		
				size_t textWidth = record->name.size();
				if(textWidth > columnWidth) {
					columnWidth = textWidth;
				}
				int padding = (int)((columnWidth - textWidth) / 2.F);
		
				headerText += generateString(' ',padding);
				headerText += record->name.c_str();
				headerText += generateString(' ',padding);
				headerText += "|";
			}
		});
		for(size_t j = 0 ; j < headerText.size() ; j++) {
			headerOrnament += "-";
		}
	
	
	
		printf("%s\n",headerOrnament.c_str());
		printf("%s\n",headerText.c_str());
		printf("%s\n",headerOrnament.c_str());
	
		for(size_t j = 0 ; j < rows && (j < maxPrint) ; j++) {
			if(!printFiltered && filteredRows[j]) {
				continue;
			}
			std::string rowText = RowToString(j);
			if(printFiltered || !filteredRows[j]) {

					std::pair<size_t*,ushort> referenceIndexRange = reference->GetReferenceRows(j);
	//				printArray(referenceIndexRange.first,referenceIndexRange.second);
					if(referenceIndexRange.first) {
						rowText += "---> " + reference->targetTable->name + "{";
						for(size_t i = 0 ; i < referenceIndexRange.second ; ++i) {
	
							 rowText += std::to_string(referenceIndexRange.first[i]);
							if(i < referenceIndexRange.second - 1) {
								rowText +=  ",";
							}
						}
						rowText += "}";
					}
				
				if(filteredRows[j]) {
					rowText += "** FILTERED **";
				}
			}
	
			printf("%s\n",rowText.c_str());
		}
		printf("%s\n",headerOrnament.c_str());
		
		//    PrintFilteredRowMap();
	}
//	void PrintFilteredRowMap(); //Print the underlying filter map
	
	HistogramSet<Ts...> CreateHistogramSet(int bins) {
		HistogramSet<Ts...> histograms;
		
		TupleForEach(records, [&](auto tRecords) {
			for(auto record : tRecords) {
				histograms.AddHistogram(record->CreateHistogram(bins));
			}
		});
		return histograms;
	}
};

#endif /* defined(__RIVDataSet__Table__) */
