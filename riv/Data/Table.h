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
#include "TableInterface.h"
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

template <typename... Ts>
class RIVTable : public RIVTableInterface {
private:
	const size_t tupleSize = sizeof...(Ts);
	
	std::vector<RIVDataSetListener*> onChangeListeners;
	std::vector<size_t> selectedRows;
	
	bool filtered = false;
	bool isClustered = false;
	
	std::tuple<std::vector<RIVRecord<Ts>*>...> records;
	std::tuple<std::vector<riv::SingularFilter<Ts>*>...> filters;
	std::tuple<std::vector<riv::GroupFilter<Ts>>...> groupFilters;

	std::vector<std::string> attributes;
	
	//Stored here so I can easily clean them up as they are newly allocated
	TableIterator* iterator = NULL;
	
	std::map<size_t,bool> filteredRows;
	std::vector<size_t> newlyFilteredRows;
	
public:
	RIVTable(std::string name) : RIVTableInterface(name) {
		
	}
	~RIVTable() {
		tuple_for_each(filters, [&](auto tFilters) {
			deletePointerVector(tFilters);
		});
	}
	RIVTable* CloneStructure() {
		RIVTable* clone = new RIVTable(name);
		tuple_for_each(records, [&](auto records) {
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
		tuple_for_each(records, [&](auto tRecords) {
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
		
		printf("Filtering table %s with filters:\n",name.c_str());
		tuple_for_each(filters, [&](auto tFilters) {
			for(auto filter : tFilters) {
				printf("\t");
				filter->Print();
			}
		});
		printf("\n");
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
					tuple_for_each(filters, [&](auto tFilters) {
						for(auto filter : tFilters) {
							
//							If the filter applies to this table, filter according to
//							if(filter->AppliesToTable(thisInterface)) {
								typedef typename get_template_type<typename std::decay<decltype(*filter)>::type>::type templateType;
								auto recordForFilter = GetRecord<templateType>(filter->GetAttribute());
								filterSourceRow = !filter->PassesFilter(recordForFilter->Value(row));
								if(filterSourceRow) {
									break;
								}
//							}
						}
					});
				}
	
				if(filterSourceRow) {
//									printf("row = %zu FILTERED\n",row);
					FilterRow(row);
					newlyFilteredRows.push_back(row);
				}
				else {
//					printf("row = %zu SELECTED\n",row);
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
	void FilterReferences() {
		//This checks its references to create a group that it is referring to and see if ALL of its rows are filtered, only then is the reference row filtered as well
		//TODO: I have feeling this is really ugly... and its really costly ?
		reporter::startTask("Filter References");
		//If this one is filtered
		//			printf("Filter references for row = %zu\n",row);
		//This happens to paths table
		RIVMultiReference* forwardRef = dynamic_cast<RIVMultiReference*>(reference);
		if(forwardRef) {
			for(size_t row : newlyFilteredRows) {
				forwardRef->FilterReferenceRow(row);
			}
		}
		else { //This happens to intersections table
			RIVReference* backReference = reference->targetTable->reference;
			RIVMultiReference* multiRef = dynamic_cast<RIVMultiReference*>(backReference);
			if(multiRef) {
				for(auto iterator = multiRef->indexMap.begin(); iterator != multiRef->indexMap.end(); iterator++) {
					std::pair<size_t*,ushort> backRows = iterator->second;
					bool filterReference = true;
					//					printMap(filteredRows);
					for(ushort i = 0 ; i < backRows.second ; ++i) { //Does the filtered map contain ALL of these rows? If so we should filter it in the reference table
						//						printArray(backRows.first, backRows.second);
						bool filteredBackRow = filteredRows[backRows.first[i]];
						if(!filteredBackRow) {
							filterReference = false;
							break;
						}
					}
					
					if(filterReference) {
						//						printf("Filter reference row %zu at %s\n",iterator->first,reference->targetTable->name.c_str());
						reference->targetTable->FilterRow(iterator->first);
					}
				}
			}
			newlyFilteredRows.clear();
		}
		reporter::stop("Filter References");
	}
	template<typename T>
	std::vector<riv::SingularFilter<T>*>* GetFilters() {
		return &std::get<std::vector<riv::SingularFilter<T>*>>(filters);
	}
	template<typename T>
	bool ContainsFilter(riv::Filter<T>* filter);
	template<typename T>
	void AddFilter(riv::SingularFilter<T> *filter) {
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
//		tuple_for_each(records, [&](auto records) {
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
		tuple_for_each(filters, [&](auto filters) {
			filters.clear();
		});
		tuple_for_each(groupFilters, [&](auto groupFilters) {
			groupFilters.clear();
		});
	}
	//Clears all the filters with the given attribute name, returns true if any filter was actually removed
	bool ClearFilter(const std::string& filterName) {
		bool filterFound = false;
		int filtersPresent = 0;
		tuple_for_each(filters, [&](auto tFilters) {
			size_t i;
			filterFound = false;
			filtersPresent += tFilters.size();
			for(i = 0 ; i < tFilters.size() ; ++i) {
				if(tFilters[i]->AppliesToAttribute(filterName)) {
					filterFound = true;
					break;
				}
			}
			if(!filterFound) {
				printf("No such filter found.\n");
			}
			else {
				--filtersPresent;
				tFilters.erase(tFilters.begin() + i);
				return;
			}
		});
		if(filtersPresent == 0) {
			filtered = false;
		}
		return filterFound;
	}
	bool ClearFilter(size_t fid);
	void ClearFilteredRows();
	
	float PercentageFiltered();
	bool IsFiltered() { return filtered; }; //Any filters applied?
	bool IsClustered() { return isClustered; };
	
	bool HasRecord(const std::string& name) const {
		bool found = false;
		tuple_for_each(records,[&](auto tRecords) {
			for(auto record : tRecords) {
				if(record->name == name) {
					found = true;
					return; //Escape from the tuple for each
				}
			}
		});
		return found;
	}
	
	TableIterator* GetIterator() {
		if(iterator) {
			delete iterator;
		}
		if(IsFiltered()) {
			
			iterator = new FilteredTableIterator(&filteredRows,NumberOfRows(), reference);
		}
		else {
			iterator = new TableIterator(NumberOfRows(), reference);
		}
		return iterator;
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
		tuple_for_each(records, [&](auto tRecords) {
			total += tRecords.size();
		});
		return total;
	}
	//TODO: This does not work if the table does not use  its first template types (i.e. if no float records exists for example)
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
		tuple_for_each(records, [&](auto tRecords) {
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
	
		tuple_for_each(records, [&](auto tRecords) {
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
		
		tuple_for_each(records, [&](auto tRecords) {
			for(auto record : tRecords) {
				histograms.AddHistogram(record->CreateHistogram(bins));
			}
		});
		return histograms;
	}
};

#endif /* defined(__RIVDataSet__Table__) */
