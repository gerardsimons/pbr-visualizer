//
//  Table.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Table.h"
#include "../helper.h"
#include <algorithm>

RIVTable::RIVTable(std::string name) {
    this->name = name;
}

void RIVTable::AddRecord(RIVRecord* record) {
    if(rows == 0 || record->Size() == rows) {
        rows = record->Size();
		attributes.push_back(record->name);
        records.push_back(record);
    }
    else throw std::string("Record should have the same size as present records.");
}

RIVRecord* RIVTable::GetRecord(size_t index) {
    return records[index];
}

void RIVTable::filterRecords() {
	printf("Filtering table %s \n",name.c_str());
	filteredRows.clear();
	std::vector<size_t> newFilteredRows;
	if(filters.size() > 0 || groupFilters.size() > 0) {
		printf("Filtering table %s with filter:\n",name.c_str());
		for(riv::Filter* f : filters) {
			printf("\t");
			f->Print();
		}
		printf("\n");
		for(size_t row = 0 ; row < rows ; row++) {
			if(filteredRows[row]) {
//				printf("row %zu was already filtered\n",row);
				continue; //Already filtered
			}
			bool filterSourceRow = false;
			//Group filters
			for(riv::GroupFilter* groupFilter : groupFilters) {
//				printf("Group filtering : ");
//				groupFilter->Print();
				if(!groupFilter->PassesFilter(this, row)) {
					filterSourceRow = true;
					break;
				}
			}
			
			if(!filterSourceRow) {
				for(riv::Filter* filter : filters) {
					//If the filter applies to this table, filter according to
					if(filter->AppliesToTable(this)) {
						filterSourceRow = !filter->PassesFilter(this, row);
						if(filterSourceRow) {
							break;
						}
					}
				}
			}
			
			if(filterSourceRow) {
//				printf("row = %zu FILTERED\n",row);
				FilterRow(row);
				newFilteredRows.push_back(row);
			}
			else {
//				printf("row %zu PASSES.\n",row);
			}
		}
	}
	if(newFilteredRows.size() > 0) {
		filtered = true;
	}
	else filtered = false;
	printf("%zu new filtered rows\n",newFilteredRows.size());
//	printf("filteredRows = \n");
//	printMap(filteredRows);
	//This checks its references to create a group that it is referring to and see if ALL of its rows are filtered, only then is the reference row filtered as well
	//TODO: I have feeling this is really ugly... and its really costly ?
	for(RIVReference *reference : references) {
//		reference->targetTable->ClearFilters();
		RIVMultiReference* forwardRef = dynamic_cast<RIVMultiReference*>(reference);
		if(forwardRef) {
			for(size_t row : newFilteredRows) {
//				printf("filter reference row of %zu\n",row);
				forwardRef->FilterReferenceRow(row);
			}
			continue;
		}
		else {
			RIVReference* backReference = reference->targetTable->GetReferenceToTable(name);
			RIVMultiReference* multiRef = dynamic_cast<RIVMultiReference*>(backReference);
			if(multiRef) {
				for(auto iterator = multiRef->indexMap.begin(); iterator != multiRef->indexMap.end(); iterator++) {
					std::pair<size_t*,ushort> backRows = iterator->second;
					bool filterReference = true;
					for(ushort i = 0 ; i < backRows.second ; ++i) { //Does the filtered map contain ALL of these rows? If so we should filter it in the reference table
						if(!filteredRows[backRows.first[i]]) {
							filterReference = false;
							break;
						}
					}
					
					if(filterReference) {
						reference->targetTable->FilterRow(iterator->first);
					}
				}
			}
		}
	}
	
	printf("After filtering : ");
	PrintUnfiltered();
	
	printf("References : ");
	for(RIVReference* ref : references) {
		ref->targetTable->PrintUnfiltered();
	}
}

float RIVTable::PercentageFiltered() {
	return filteredRows.size() / (float)rows * 100.F;
}

//This is only necessary if there are some orphaned rows in reference tables
void RIVTable::FilterRowsUnlinkedTo(RIVTable *table) {
    for(size_t row = 0 ; row < rows ; ++row) {
        for(RIVReference *reference : references) {
            if(reference->targetTable->GetName() == table->GetName() && !reference->HasReference(row)) {
                FilterRow(row);
            }
        }
    }
}

//Warning: Only gets a reference to a table if an immediate reference exists, use GetReferenceChain if you want to find a complete chain of reference to a target table
RIVReference* RIVTable::GetReferenceToTable(const std::string &tableName) {
    for(size_t row = 0 ; row < references.size() ; ++row) {
        RIVReference *ref = references[row];
        if(ref->targetTable->name == tableName) {
            return ref;
        }
    }
    return NULL;
}

void RIVTable::FilterRow(size_t rowIndex) {
    filteredRows[rowIndex] = true;
    filtered = true;
}

void RIVTable::SelectRow(size_t rowIndex) {
//	printf("table %s selects row %zu\n",name.c_str(),rowIndex);
    filteredRows[rowIndex] = false;
}

std::vector<RIVRecord*> RIVTable::GetRecords() {
    return records;
}

void RIVTable::FunctionOnRecords(void(*someFunction)(const RIVRecord*)) {
    for(RIVRecord* record : records) {
        RIVFloatRecord* floatRecord = CastToFloatRecord(record);
        if(floatRecord) {
            someFunction(floatRecord);
            continue;
        }
        RIVUnsignedShortRecord* shortRecord = CastToUnsignedShortRecord(record);
        if(shortRecord) {
            someFunction(record);
        }
    }
}

std::vector<std::string> RIVTable::GetAttributes() const {
	return attributes;
}


bool RIVTable::ContainsColumn(std::string name) {
    for(RIVRecord* record : records) {
        RIVFloatRecord* floatRecord = CastToFloatRecord(record);
        if(floatRecord) {
            //Code
            if(floatRecord->name == name) {
                return true;
            }
            continue;
        }
        
        RIVUnsignedShortRecord* shortRecord = CastToUnsignedShortRecord(record);
        if(shortRecord) {
            if(shortRecord->name == name) {
                return true;
            }
            continue;
        }
    }
    return false;
}

bool RIVTable::ClearFilters() {
	bool filtersCleared = filters.size() > 0;
    filters.clear();
    filteredRows.clear();
    filtered = false;
	return filtersCleared;
}

bool RIVTable::ClearFilter(size_t fid) {
    bool filterFound = false;
    for(size_t i = 0 ; i < filters.size() ; i++) {
        riv::Filter *filter = filters[i];
        if(filter->GetId() == fid) {
            std::vector<riv::Filter*>::iterator it = filters.begin();
            std::advance(it, i);
            filters.erase(it);
			delete filter;
			std::cout << "Table " << name << " has cleared filter ";
			filter->Print();
			return true;
        }
    }
	if(filters.size() == 0) {
		filtered = false;
	}
	return filterFound;
}


bool RIVTable::ClearFilter(const std::string& name) {
    bool filterFound = false;
    for(size_t i = 0 ; i < filters.size() ; i++) {
        riv::Filter *filter = filters[i];
        if(filter->AppliesToAttribute(name)) {
            std::vector<riv::Filter*>::iterator it = filters.begin();
            std::advance(it, i);
            filters.erase(it);
			std::cout << "Table " << name << " has cleared filter ";
			filter->Print();
			return true;
        }
    }
	if(filters.size() == 0) {
		filtered = false;
	}
	return filterFound;
}

void RIVTable::AddFilter(riv::Filter *filter) {
    filters.push_back(filter);
}

void RIVTable::AddFilter(riv::GroupFilter *groupFilter) {
	groupFilters.push_back(groupFilter);
}

void RIVTable::AddReference(RIVReference* reference) {
    references.push_back(reference);
}


RIVFloatRecord* RIVTable::CastToFloatRecord(RIVRecord* record) {
    //    printf("record name = %s\n",record->name.c_str());
    RIVFloatRecord *floatRecord = dynamic_cast<RIVFloatRecord*>(record);
    return floatRecord;
}

RIVUnsignedShortRecord* RIVTable::CastToUnsignedShortRecord(RIVRecord* record) {
    RIVUnsignedShortRecord *unsignedShortRecord = dynamic_cast<RIVUnsignedShortRecord*>(record);
    return unsignedShortRecord;
}

size_t RIVTable::NumberOfColumns() {
    return records.size();
}

size_t RIVTable::NumberOfRows() {
    return rows;
}

//Helper function for row to string
std::string generateString(char c, size_t number) {
    std::string generatedString;
    
    for(size_t i = 0 ; i < number ; i++) {
        generatedString += c;
    }
    return generatedString;
}

std::string RIVTable::RowToString(size_t row) {
    std::string rowText = std::to_string(row) + "|";
    for(size_t i = 0 ; i < records.size() ; i++) {
        RIVRecord* record = records[i];
		
        RIVFloatRecord *floatRecord = CastToFloatRecord(record);
        RIVUnsignedShortRecord *shortRecord = CastToUnsignedShortRecord(record);
        
        const int columnWidth = 15;
        std::string valueString;
        size_t textWidth;
        if(floatRecord) {
            valueString = std::to_string(floatRecord->Value(row));
            textWidth = valueString.size();
        }
        else if(shortRecord) {
            valueString = std::to_string(shortRecord->Value(row));
            textWidth = valueString.size();
        }
        else {
            throw "Unrecognized record type.";
        }
        
        int padding = floor((columnWidth - textWidth - 1) / 2.F);
        
        rowText += generateString(' ',padding);
        rowText += valueString;
        rowText += generateString(' ',padding);
		
        int remainder = (columnWidth - textWidth - 1) % 2;
        
        rowText += generateString(' ', remainder);
        rowText += "|";
        
        
    }
    return rowText;
}

bool RIVTable::HasRecord(RIVRecord* requested) {
    if(records.size() > 0) {
        for(RIVRecord *record : records) {
            if(record->name == requested->name) {
                return true;
            }
        }
    }
    return false;
}

void RIVTable::PrintUnfiltered() {
	Print(0,false);
}

void RIVTable::Print(size_t maxPrint, bool printFiltered) {
    printf("Table called %s has %zu records %zu rows and %zu references.\n First %zu rows:\n",name.c_str(),records.size(),rows,references.size(),maxPrint);
    std::string headerText = "|";
    std::string headerOrnament;
    
    int columnWidth = 17;
    
    for(size_t i = 0 ; i < records.size() ; i++) {
        RIVRecord* record = records[i];
        
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
    for(size_t j = 0 ; j < headerText.size() ; j++) {
        headerOrnament += "-";
    }
    
	if(maxPrint == 0) {
		maxPrint = rows;
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
            for(RIVReference *reference : references) {
				std::pair<size_t*,ushort> referenceIndexRange = reference->GetIndexReferences(j);
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
    
    //    PrintFilteredRowMap();
}

bool RIVTable::GetReferenceChainToTable(std::string tableNameSearch, RIVReferenceChain& chainToTarget, std::vector<std::string> *visitedTables) {
    //Start of recursion
    if(visitedTables == NULL) {
        visitedTables = new std::vector<std::string>();
    }
    //End of recursion, target found
    if(name == tableNameSearch) {
        delete visitedTables;
        return true;
    }
    visitedTables->push_back(name);
    for(RIVReference* reference : references) {
        bool visited = false;
        for(std::string visitedName : *visitedTables) {
            if(visitedName == reference->targetTable->name) { //If we did not already visit this table
                visited = true;
                break;
            }
        }
        if(!visited) {
            chainToTarget.AddReference(reference);
            if(reference->targetTable->GetReferenceChainToTable(tableNameSearch, chainToTarget, visitedTables)) {
                return true;
            }
            else {
                chainToTarget.PopReference();
            }
        }
    }
    //It's a dead end
    return false;
}


const RIVTable* RIVTable::FindTable(std::string tableName, std::vector<std::string>* visitedTables) {
    if(this->name == tableName) {
		delete visitedTables;
        return this;
    }
    if(visitedTables) {
        for(std::string name : *visitedTables) {
            if(name == this->name) {
                return NULL;
            }
        }
    }
    else {
        if(!visitedTables) {
            visitedTables = new std::vector<std::string>();
        }
        visitedTables->push_back(this->name);
        for(RIVReference *reference : references) {
            const RIVTable* foundTable = reference->targetTable->FindTable(tableName, visitedTables);
            if(foundTable) {
                return foundTable;
            }
        }
    }
    delete visitedTables;
    return NULL;
}

RIVCluster* RIVTable::ClusterForRow(const size_t& row) const {
    return clusterSet.ClusterForMemberIndex(row); //Will return NULL if row is not contained in any cluster, or there has been no clustering
}

const std::vector<RIVReference*>* RIVTable::GetReferences() {
    return &references;
}

void RIVTable::PrintFilteredRowMap() {
    for(auto iterator = filteredRows.begin(); iterator != filteredRows.end(); iterator++) {
        // iterator->first = key
        // iterator->second = value
        // Repeat if you also want to iterate through the second map.
        printf("%lu : ",iterator->first);
        printf("%s\n", iterator->second ? "filtered" : "not filtered");
    }
}

TableIterator* RIVTable::GetIterator() {
	if(iterator) {
		delete iterator;
	}
	if(IsFiltered()) {
		iterator = new FilteredTableIterator(&filteredRows,rows,&clusterSet, &references);
	}
	else {
		iterator = new TableIterator(rows,&clusterSet, &references);
	}
	return iterator;
}

//TableIterator RIVTable::GetIterator() {
//    if(IsFiltered()) {
//        return FilteredTableIterator(&filteredRows,rows,&clusterSet, &references);
//    }
//    else {
//        return TableIterator(rows,&clusterSet, &references);
//    }
//}

bool RIVTable::HasRecord(const std::string& recordName) {
	for(RIVRecord *record : records) {
        if(record->name == recordName) {
            return true;
        }
    }
	return false;
}

RIVRecord* RIVTable::GetRecord(std::string recordName) {
    for(RIVRecord *record : records) {
        if(record->name == recordName) {
            return record;
        }
    }
    throw std::string("No such record" + recordName);
}

//Cluster with requested cluster size, rather than number of clusters
void RIVTable::ClusterWithSize(const std::string& xRecordName, const std::string& yRecordName, const std::string& zRecordName, const size_t& P, const size_t& maxRepeat) {
    size_t numRows = NumberOfRows();
    if(P > numRows) {
        throw "Requested cluster size larger than total number of elements.";
    }
    const size_t K = round((float)numRows/P);
    
    Cluster(xRecordName,yRecordName,zRecordName,K,maxRepeat);
}

RIVClusterSet* RIVTable::Cluster(const std::string& xRecordName, const std::string& yRecordName, const std::string& zRecordName, const size_t& K, const size_t& maxRepeat) {
    if(!(ContainsColumn(xRecordName) && ContainsColumn(yRecordName) && ContainsColumn(zRecordName))) {
        throw "Table does not contain said records.";
    }
    RIVFloatRecord *xRecord = RIVTable::CastToFloatRecord(GetRecord(xRecordName));
    RIVFloatRecord *yRecord = RIVTable::CastToFloatRecord(GetRecord(yRecordName));
    RIVFloatRecord *zRecord = RIVTable::CastToFloatRecord(GetRecord(zRecordName));
    if(K < 1) {
        throw std::string("Invalid cluster size " + std::to_string(K));
    }
    if(xRecord->Size() == 0) {
        throw "Cannot cluster 0 values.";
    }
    //    clusterSet = new RIVClusterSet(K,xRecord->GetValuesPointer(),yRecord->GetValuesPointer(),zRecord->GetValuesPointer());
    //    clusterSet->Cluster(maxRepeat);
    clusterSet = RIVClusterSet::MakeCluster(maxRepeat, K, xRecord->GetValuesPointer(), yRecord->GetValuesPointer(), zRecord->GetValuesPointer());
    isClustered = true;
    //    clusterSet.CreateClusterRegister();
    
    return &clusterSet;
}
