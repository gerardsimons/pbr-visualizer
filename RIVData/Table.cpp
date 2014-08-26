//
//  Table.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

// Template for iteration
/*
 for(RIVRecord* record : records) {
 RIVFloatRecord* floatRecord = CastToFloatRecord(record);
 if(floatRecord) {
 //Code
 continue;
 }
 
 RIVUnsignedShortRecord* shortRecord = CastToUnsignedShortRecord(record);
 if(shortRecord) {
 //Code
 continue;
 }
 }
 */

#include "Table.h"
#include "helper.h"
#include <algorithm>


RIVTable::RIVTable(std::string name) {
    this->name = name;
}

void RIVTable::AddRecord(RIVRecord* record) {
    if(rows == 0 || record->Size() == rows) {
        rows = record->Size();
        records.push_back(record);
    }
    else throw std::string("Record should have the same size as present records.");
}

RIVRecord* RIVTable::GetRecord(size_t index) {
    return records[index];
}

void RIVTable::filterRecords() {
    filteredRows.clear();
    for(Filter* filter : filters) {
        filterRecords(filter);
    }
}

void RIVTable::filterRecords(Filter* filter) {
    for(RIVRecord* record : records) {
        RIVFloatRecord* floatRecord = CastToFloatRecord(record);
        if(floatRecord && floatRecord->name == filter->attributeName) {
            for(size_t j = 0 ; j < floatRecord->Size() ; j++) {
                if(filteredRows[j]) continue; //Already filtered
                float value = floatRecord->Value(j);
                if(filter->PassesFilter(record->name, value) == false) {
                    FilterRow(j,true);
                    //                    printf("filtered out %f\n",(float)value);
                }
                else FilterRow(j,false);
            }
            continue;
        }
        RIVUnsignedShortRecord* shortRecord = CastToUnsignedShortRecord(record);
        if(shortRecord && shortRecord->name == filter->attributeName) {
            for(size_t j = 0 ; j < shortRecord->GetValues().size() ; j++) {
                if(filteredRows[j]) continue; //Already filtered
                unsigned short value = shortRecord->Value(j);
                if(filter->PassesFilter(record->name, value) == false) {
                    FilterRow(j,true);
                    //                    printf("filtered out %hu\n",(ushort)value);
                }
                else FilterRow(j,false);
            }
            continue;
        }
    }
    
    //Filter unlinked records of linked tables
    for(RIVReference &reference : references) {
        RIVTable *targetTable = reference.targetTable;
        targetTable->FilterRowsUnlinkedTo(this);
    }
}

void RIVTable::FilterRowsUnlinkedTo(RIVTable *table) {
    for(size_t row = 0 ; row < rows ; ++row) {
        for(RIVReference &reference : references) {
            if(reference.targetTable->GetName() == table->GetName() && !reference.HasReference(row)) {
                FilterRow(row,true);
            }
        }
    }
}

void RIVTable::FilterRow(size_t rowIndex, bool filterOrUnfilter, RIVReference* sourceReference) {
    //    if(sourceReference) {
    //        printf("Linked filte§r caused by %s\n",sourceReference->sourceTable->GetName().c_str());
    //    }
    //    printf("Table %s filter row %lu\n",name.c_str(),rowIndex);
    filteredRows[rowIndex] = filterOrUnfilter;
    if(filterOrUnfilter) filtered = true;
    for(RIVReference &reference : references) {
        RIVTable* targetTable = reference.targetTable;
        std::vector<size_t>* targetIndexRange = reference.GetIndexReferences(rowIndex);
        if(targetIndexRange != 0 && ( sourceReference == 0 || targetTable->name != sourceReference->sourceTable->name)) { //Do not update a table who just updated this table
            //            printf("Force table %s to filter row %zu\n",targetTable->name.c_str(),*targetIndex);
            for(size_t filterIndex : *targetIndexRange) {
                targetTable->FilterRow(filterIndex,filterOrUnfilter,&reference);
            }
        }
    }
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

void RIVTable::ClearFilters() {
    filters.clear();
    filteredRows.clear();
    filtered = false;
}

void RIVTable::ClearFilter(std::string name) {
    bool recomputeNeeded = false;
    for(size_t i = 0 ; i < filters.size() ; i++) {
        Filter *filter = filters[i];
        if(filter->attributeName == name) {
            std::vector<Filter*>::iterator it = filters.begin();
            std::advance(it, i);
            filters.erase(it);
            recomputeNeeded = true;
        }
    }
    if(recomputeNeeded) {
        filterRecords();
    }
}

void RIVTable::AddFilter(Filter *filter) {
    //    printf("Filtering on ");
    //    filter->Print();
    filters.push_back(filter);
    filterRecords(filter);
}

void RIVTable::AddReference(const RIVReference& reference) {
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
            throw new std::string("Unrecognized record type.");
        }
        
        int padding = floor((columnWidth - textWidth - 1) / 2.F);
        
        //                printf("valueString = %s\n",valueString.c_str());
        
        
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
    
    printf("%s\n",headerOrnament.c_str());
    printf("%s\n",headerText.c_str());
    printf("%s\n",headerOrnament.c_str());
    
    for(size_t j = 0 ; j < rows && (j < maxPrint) ; j++) {
        std::string rowText = RowToString(j);
        if(printFiltered || !filteredRows[j]) {
            for(RIVReference &reference : references) {
                std::vector<size_t> *referenceIndexRange = reference.GetIndexReferences(j);
                if(referenceIndexRange && !referenceIndexRange->empty()) {
                    rowText += "---> " + reference.targetTable->name + "{";
                    
                    for(size_t i = 0 ; i < referenceIndexRange->size(); ++i) {
                        
                        rowText += std::to_string((*referenceIndexRange)[i]);
                        if(i < referenceIndexRange->size() - 1) {
                            rowText +=  ",";
                        }
                    }
                    rowText += "}";
                }
            }
            if(filteredRows[j]) {
                rowText += "***";
            }
        }
        
        printf("%s\n",rowText.c_str());
    }
    printf("%s\n",headerOrnament.c_str());
    
    //    PrintFilteredRowMap();
}

//Tries to find a table by visiting references recursively and seeing if any reference target table names matches the one given.
//DEPRECATED; this only will work for depth of one, not for a chain of references (see new GetReferenceChainToTable)
//RIVReference* RIVTable::GetReferenceToTable(std::string tableName, std::vector<std::string> *visitedTables) {
//    if(!visitedTables) {
//        visitedTables = new std::vector<std::string>();
//    }
//    visitedTables->push_back(this->name);
//    for(RIVReference& reference: references) {
//        if(reference.targetTable->name == tableName) {
//            return &reference;
//        }
//    }
//    for(RIVReference& reference : references) {
//        for(std::string visitedName : *visitedTables) {
//            if(visitedName != reference.targetTable->name) {
//                RIVReference *found = reference.targetTable->GetReferenceToTable(tableName, visitedTables);
//                if(found) {
//                    return found;
//                }
//            }
//        }
//    }
//    delete visitedTables;
//    return NULL;
//}

//bool RIVTable::GetReferenceChainToTable(std::string tableName, RIVReferenceChain& chainToTarget, std::vector<std::string> *visitedTables) {
//    if(visitedTables == NULL) {
//        visitedTables = new std::vector<std::string>();
//    }
//    visitedTables->push_back(this->name);
//    for(RIVReference& reference: references) {
//        if(reference.targetTable->name == tableName) {
//            chainToTarget.AddReference(&reference);
//            return true;
//        }
//    }
//    for(RIVReference& reference : references) {
//        for(std::string visitedName : *visitedTables) {
//            if(visitedName != reference.targetTable->name) {
////                RIVReference *found = reference.targetTable->GetReferenceToTable(tableName, visitedTables);
////                if(found) {
////                    chainToTarget.AddReference(found);
////                    return true;
////                }
//                chainToTarget.AddReference(&reference);
//                if(reference.targetTable->GetReferenceChainToTable(tableName, chainToTarget,visitedTables)) {
//                    return true;
//                }
//            }
//        }
//    }
//    delete visitedTables;
//    return false;
//}

//bool RIVTable::GetReferenceChainToTable(std::string tableNameSearch, RIVReferenceChain& chainToTarget, std::vector<std::string> *visitedTables) {
//    //Start of recursion
//    if(visitedTables == NULL) {
//        visitedTables = new std::vector<std::string>();
//    }
//    //End of recursion, target found
//    if(name == tableNameSearch) {
//        delete visitedTables;
//        return true;
//    }
//    visitedTables->push_back(name);
//    for(RIVReference& reference : references) {
//        bool visited = false;
//        for(std::string visitedName : *visitedTables) {
//            if(visitedName == reference.targetTable->name) { //If we did not already visit this table
//                visited = true;
//                break;
//            }
//        }
//        if(!visited) {
//            chainToTarget.AddReference(&reference);
//            if(reference.targetTable->GetReferenceChainToTable(tableNameSearch, chainToTarget, visitedTables)) {
//                return true;
//            }
//        }
//    }
//        
//    return false;
//}

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
    for(RIVReference& reference : references) {
        bool visited = false;
        for(std::string visitedName : *visitedTables) {
            if(visitedName == reference.targetTable->name) { //If we did not already visit this table
                visited = true;
                break;
            }
        }
        if(!visited) {
            chainToTarget.AddReference(&reference);
            if(reference.targetTable->GetReferenceChainToTable(tableNameSearch, chainToTarget, visitedTables)) {
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
        return this;
        delete visitedTables;
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
        for(RIVReference &reference : references) {
            const RIVTable* foundTable = reference.targetTable->FindTable(tableName, visitedTables);
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

const std::vector<RIVReference>* RIVTable::GetReferences() {
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
    if(IsFiltered()) {
        iterator = new FilteredTableIterator(&filteredRows,rows,&clusterSet);
    }
    else {
        iterator = new TableIterator(rows,&clusterSet);
    }
    return iterator;
}

RIVRecord* RIVTable::GetRecord(std::string name) const {
    for(RIVRecord* record : records) {
        if(record->name == name) {
            return record;
        }
    }
    return 0;
}

RIVRecord* RIVTable::GetRecord(std::string recordName) {
    for(RIVRecord *record : records) {
        if(record->name == recordName) {
            return record;
        }
    }
    throw std::string("No such record.");
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