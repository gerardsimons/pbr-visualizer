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
	
	bool empty = true;
	bool isClustered = false;
	
	std::tuple<std::vector<RIVRecord<Ts>*>...> records;
	std::tuple<std::map<std::string,RIVRecord<Ts>*>...> recordsRegister;
	
	std::tuple<std::vector<riv::SingularFilter<Ts>*>...> filters;
	std::vector<riv::RowFilter*> rowFilters; //Custom row filters
	
	std::vector<std::string> attributes;
	
	//Stored here so I can easily clean them up as they are newly allocated
	TableIterator* iterator = NULL;
	
	std::map<size_t,bool> filteredRows;
	std::map<size_t,bool> newlyFilteredRows;
	
	template<typename T>
	std::map<std::string,RIVRecord<T>*>& getRecordsRegister() {
		return std::get<std::map<std::string,RIVRecord<T>*>>(recordsRegister);
	}
public:
	RIVTable(std::string name) : RIVTableInterface(name) {
		
	}
	~RIVTable() {
		tuple_for_each(filters, [&](auto tFilters) {
			deletePointerVector(tFilters);
		});
		tuple_for_each(records, [&](auto tRecords) {
			deletePointerVector(tRecords);
		});
		if(reference) {
			delete reference;
		}
	}
	RIVTable* CloneStructure() {
		RIVTable* clone = new RIVTable(name);
		tuple_for_each(records, [&](auto records) {
			for(auto& record : records) {
				clone->AddRecord(record->CloneStructure());
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
		if(numRows != 0 && numRows != record->Size()) {
			throw std::runtime_error("RIVRecord is of invalid size");
		}
	}
	void JoinTable(RIVTable* otherTable) {
		if(otherTable != NULL) {
			tuple_for_each(records, [&](auto tRecords) {
				for(auto& record : tRecords) {
					
					typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type templateType;
					auto otherRecord = otherTable->template GetRecord<templateType>(record->name);
					
					record->AppendRecord(otherRecord);
				}
			});
			
			//If there are some rows already present here (with references) we should put the references behind it (+1)
			size_t currentNrRows = NumberOfRows();
			short offset = 0;
			if(currentNrRows) {
				offset = 1;
			}
			
			RIVReference* otherReference = otherTable->reference;
			RIVSingleReference* otherSingleRef = dynamic_cast<RIVSingleReference*>(otherReference);
			RIVMultiReference* otherMultiRef = dynamic_cast<RIVMultiReference*>(otherReference);
			RIVSingleReference* singleRef = dynamic_cast<RIVSingleReference*>(reference);
			RIVMultiReference* multiRef = dynamic_cast<RIVMultiReference*>(reference);
			
			if(otherSingleRef) { //Single references
				auto otherIndices = otherSingleRef->GetIndexMap();
				auto indices = singleRef->GetIndexMap();
				size_t lastIndex = indices.rbegin()->first + offset;
				size_t lastToIndex = indices[lastIndex - 1] + offset;
				for(auto it : otherIndices) {
					
					singleRef->AddReference(it.first + lastIndex, it.second + lastToIndex);
				}
				//Increment all by
			}
			else if(otherMultiRef) { //Multi reference
				auto otherIndices = otherMultiRef->GetIndexMap();
				auto indices = multiRef->GetIndexMap();
				size_t lastIndex = indices.rbegin()->first + offset;
				auto toRows = indices[lastIndex - 1];
				size_t lastToIndex = 0;
				//Find end of this reference
				for(auto it : indices) {
					lastToIndex += it.second.second;
				}
				if(offset) {
					lastIndex -= offset;
				}
				for(auto it : otherIndices) {
					ushort size = it.second.second;
					size_t* newToRows = new size_t[size];
					for(ushort i = 0 ; i < size ; ++i) {
						newToRows[i] = lastToIndex++;
					}
					std::pair<size_t*,ushort> mapping(newToRows,size);
					multiRef->AddReferences(it.first + lastIndex, mapping);
				}
			}
		}
	}
	template<typename T>
	RIVRecord<T>* CreateRecord(const std::string& name) {
		//		AddRecord<float>(RIVRecord<float>(name));
		std::vector<RIVRecord<T>*>* tRecords = GetRecords<T>();
		RIVRecord<T>* newRecord = new RIVRecord<T>(name);
		tRecords->push_back(newRecord);
		getRecordsRegister<T>()[name] = newRecord;
		return newRecord;
	}
	//Template add a value of type T to a record of the given name
	template<typename T>
	void Add(const std::string& name, const T& value) {
		auto recordsRegister = getRecordsRegister<T>();
		RIVRecord<T>* record = recordsRegister[name];
		record->AddValue(value);
		empty = false;
	}
	template<typename T>
	RIVRecord<T>* CreateRecord(const std::string& name, T min, T max,bool clampOutliers = false) {
		//		AddRecord<float>(RIVRecord<float>(name));
		std::vector<RIVRecord<T>*>* tRecords = GetRecords<T>();
		RIVRecord<T>* newRecord = new RIVRecord<T>(name,min,max,clampOutliers);
		tRecords->push_back(newRecord);
		getRecordsRegister<T>()[name] = newRecord;
		return newRecord;
	}
	void ClearData() {
		tuple_for_each(records, [&](auto tRecords) {
			for(auto& record : tRecords) {
				record->Clear();
			}
		});
		empty = true;
	}
	bool IsEmpty() {
		tuple_for_each(records, [&](auto tRecords) {
			for(auto& record : tRecords) {
				if(record->Size()) {
					empty = false;
					break;
				}
			}
			if(!empty) {
				return;
			}
		});
		return empty;
	}
	std::tuple<std::vector<RIVRecord<Ts>*>...>& GetRecords() {
		return records;
	}
	//Filter this table according to the filters that are applied
	void Filter() {
//		filteredRows.clear();
		newlyFilteredRows.clear();
//		std::string task = "Filter " + name;
//		reporter::startTask(task);
		size_t rows = NumberOfRows();
		for(size_t row = 0 ; row < rows ; row++) {
			bool filterSourceRow = false;
			// Process the manual row filters
			if(!filterSourceRow) {
				for(auto filter : rowFilters) {
					
					filterSourceRow = !filter->PassesFilter(row);
					if(filterSourceRow) {
						break;
					}
					//							}
				}
			}
			//Process the regular filters such as range filters discrete filters etc.
			if(!filterSourceRow) {
				tuple_for_each(filters, [&](auto tFilters) {
					for(auto filter : tFilters) {
						typedef typename get_template_type<typename std::decay<decltype(*filter)>::type>::type templateType;
						auto recordForFilter = GetRecord<templateType>(filter->GetAttribute());
						filterSourceRow = !filter->PassesFilter(recordForFilter->Value(row));
						if(filterSourceRow) {
							break;
						}
					}
				});
			}
			
			//FINISH UP
			if(filterSourceRow) {
//				printf("row = %zu FILTERED\n",row);
				FilterRow(row);
				newlyFilteredRows[row] = true;
			}
		}
//		printf("%zu rows filtered in table %s\n",newlyFilteredRows.size(),name.c_str());
		//				Print();
//		reporter::stop(task);
	}
	void FilterReferences() {
		reporter::startTask("Filter References");
		
		if(newlyFilteredRows.size()) {
			//This happens to paths table
			RIVMultiReference* forwardRef = dynamic_cast<RIVMultiReference*>(reference);
			if(forwardRef) {
				for(auto row : newlyFilteredRows) {
					if(row.second) {
						forwardRef->FilterReferenceRow(row.first);
					}
				}
			}
			else { //This happens to intersections table
				RIVReference* backReference = reference->targetTable->reference;
				RIVMultiReference* multiRef = dynamic_cast<RIVMultiReference*>(backReference);
				if(multiRef) {
					for(auto iterator : multiRef->GetIndexMap()) {
						size_t referenceTableRow = iterator.first;
						
						const std::pair<size_t*,ushort>& backRows = iterator.second;
						bool filter = true;
						size_t rowsFound = 0;
						//					printMap(filteredRows);
						for(ushort i = 0 ; i < backRows.second ; ++i) { //Does the filtered map contain ALL of these rows? If so we should filter it in the reference table
							//						printArray(backRows.first, backRows.second);
							bool filteredBackRow = filteredRows[backRows.first[i]];
							if(!filteredBackRow) {
								filter = false;
							}
							++rowsFound;
						}
						if(filter) {
							//							printf("Filter reference row %zu at %s\n",referenceTableRow,reference->targetTable->name.c_str());
							reference->targetTable->FilterRow(referenceTableRow);
							
						}
					}
				}
			}
			newlyFilteredRows.clear();
		}
		reporter::stop("Filter References");
	}
	void ClearRowFilters() {
		deletePointerVector(rowFilters);
		rowFilters.clear();
		ClearFilteredRows();
		if(reference) {
			reference->targetTable->ClearFilteredRows();
		}
	}
	bool ClearRowFilter(riv::RowFilter* existingFilter) {

		for(int i = 0 ; i < rowFilters.size() ; ++i) {
			riv::RowFilter* rFilter = rowFilters[i];
			if(rFilter == existingFilter) {
				delete rFilter;
				rowFilters.erase(rowFilters.begin() + i);
				ClearFilteredRows();
				if(reference) {
					reference->targetTable->ClearFilteredRows();
				}
				return true;
			}
		}
		return false;
	}
	template<typename T>
	std::vector<riv::SingularFilter<T>*>* GetFilters() {
		return &std::get<std::vector<riv::SingularFilter<T>*>>(filters);
	}
	template<typename T>
	void AddFilter(riv::SingularFilter<T> *filter) {
		auto tFilters = GetFilters<T>();
		tFilters->push_back(filter);
	}
	void AddFilter(riv::RowFilter* rowFilter) {
		rowFilters.push_back(rowFilter);
	}
	std::vector<std::string> GetAttributes() const;
	void FilterRow(size_t row) {
		filteredRows[row] = true;
		
	}
	//Clears all the filters that may be present, returns true if any filters were actually removed
	bool ClearFilters() {
		tuple_for_each(filters, [&](auto filters) {
			filters.clear();
		});
		rowFilters.clear();
	}
	//Clears all the filters with the given attribute name, returns true if any filter was actually removed
	template<typename T>
	bool ClearFilter(const std::string& filterName) {
		bool filterFound = false;
		int filtersPresent = 0;
		
		auto tFilters = GetFilters<T>();
		
		size_t i;
		filterFound = false;
		filtersPresent += tFilters->size();
		for(i = 0 ; i < tFilters->size() ; ++i) {
			if(tFilters->at(i)->GetAttribute() == filterName) {
				filterFound = true;
				break;
			}
		}
		if(filterFound) {
			printf("Filter found.\n");
			--filtersPresent;
			tFilters->erase(tFilters->begin() + i);
			
			filteredRows.clear();
			if(reference) {
				reference->targetTable->ClearFilteredRows();
			}
			
			return true;
		}
		
		printf("No such filter found.\n");
		return false;
	}
	bool IsFiltered() {
		bool filtered = false;
//		tuple_for_each(filters, [&](auto tFilters) {
//			if(tFilters.size()) {
//				filtered = true;
//				return;
//			}
//		});
//		if(!filtered) {
//			return rowFilters.size();
//		}
		return filteredRows.size();
//		return filtered;
	}; //Any filters applied?
	bool IsClustered() { return isClustered; };
	void ClearFilteredRows() {
		filteredRows.clear();
	}
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
	void ReserveRows(const size_t rows) {
		tuple_for_each(records,[&](auto tRecords) {
			for(auto record : tRecords) {
				record->Reserve(rows);
			}
		});
	}
	TableIterator* GetFullIterator() {
		if(iterator) {
			delete iterator;
		}
		else {
			iterator = new TableIterator(NumberOfRows(), reference);
		}
		return iterator;
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
	
	size_t NumberOfRecords() const {
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
	std::string RowToString(size_t row,size_t columnWidth) {
		std::string rowText = "|";
		
		
		//		tuple_for_each(records, [&](auto tRecords) {
		//			for(auto record : tRecords) {
		//				std::string valueString = std::to_string(record->Value(row));
		//				size_t textWidth = valueString.size();
		//				if(textWidth > columnWidth) {
		//					columnWidth = textWidth;
		//				}
		//				int padding = (int)((columnWidth - textWidth) / 2.F);
		//
		//				rowText += generateString(' ',padding);
		//				rowText += record->name.c_str();
		//				rowText += generateString(' ',padding);
		//				rowText += "|";
		//			}
		//		});
		//		return rowText;
		
		tuple_for_each(records, [&](auto tRecords) {
			for(auto record : tRecords) {
				
				size_t textWidth = 0;
				std::string valueString = std::to_string(record->Value(row));
				textWidth = std::min(valueString.size(),columnWidth);
				int padding = (int)((columnWidth - textWidth) / 2.F);
				
				rowText += generateString(' ',padding);
				rowText += valueString;
				rowText += generateString(' ',padding);
				
				int remainder = columnWidth - textWidth - 2 * padding;
				//
				rowText += generateString(' ', remainder);
				rowText += "|";
			}
		});
		return rowText;
	}
	std::string generateString(char c, size_t number) {
		std::string generatedString;
		
		for(size_t i = 0 ; i < number ; i++) {
			generatedString += c;
		}
		return generatedString;
	}
	void Print(size_t maxPrint = 0, bool printFiltered = true) {
		size_t rows = NumberOfRows();
		if(maxPrint == 0 || maxPrint > rows) {
			maxPrint = rows;
		}
		printf("Table called %s has %zu records and %zu rows.\n First %zu rows:\n",name.c_str(),NumberOfRecords(),rows,maxPrint);
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
				int remainder = columnWidth - textWidth - 2 * padding;
				headerText += generateString(' ',remainder);
				headerText += "|";
			}
		});
		for(size_t j = 0 ; j < headerText.size() ; j++) {
			headerOrnament += "-";
		}
		
		printf("%s\n",headerOrnament.c_str());
		printf("%s\n",headerText.c_str());
		printf("%s\n",headerOrnament.c_str());
		
		for(size_t j = 0 ; j < maxPrint ; ++j) {
			if(!printFiltered && filteredRows[j]) {
				continue;
			}
			std::string rowText = RowToString(j,columnWidth);
			if(printFiltered || !filteredRows[j]) {
				if(reference) {
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
				}
				if(filteredRows[j]) {
					rowText += "** FILTERED **";
				}
			}
			printf("%s\n",rowText.c_str());
		}
		printf("%s\n",headerOrnament.c_str());
	}
	void PrintFilteredRows(size_t maxPrint = 0) {
		size_t rows = NumberOfRows();
		if(maxPrint == 0 || maxPrint > rows) {
			maxPrint = rows;
		}
		printf("Table called %s has %zu records and %zu rows.\n First %zu rows:\n",name.c_str(),NumberOfRecords(),rows,maxPrint);
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
				int remainder = columnWidth - textWidth - 2 * padding;
				headerText += generateString(' ',remainder);
				headerText += "|";
			}
		});
		for(size_t j = 0 ; j < headerText.size() ; j++) {
			headerOrnament += "-";
		}
		
		printf("%s\n",headerOrnament.c_str());
		printf("%s\n",headerText.c_str());
		printf("%s\n",headerOrnament.c_str());
		
		for(size_t j = 0 ; j < maxPrint ; j++) {
			if(filteredRows[j]) {
				std::string rowText = RowToString(j,columnWidth);
				if(reference) {
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
				}
				if(filteredRows[j]) {
					rowText += "** FILTERED **";
				}
				
				printf("%s\n",rowText.c_str());
			}
		}
		printf("%s\n",headerOrnament.c_str());
	}
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
