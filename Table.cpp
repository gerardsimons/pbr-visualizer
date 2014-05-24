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

RIVTable::RIVTable(std::string name) {
    this->name = name;
}

void RIVTable::AddRecord(RIVRecord* record) {
    if(rows == 0 || record->Size() == rows) {
        rows = record->Size();
        records.push_back(record);
    }
    else throw std::string("Tried to add a record of altering size than present records in table.");
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

//The second argument may be a reference that triggered this filter, this is useful to avoid looping filter behaviour
void RIVTable::filterRecords(Filter* filter) {
    for(RIVRecord* record : records) {
        RIVFloatRecord* floatRecord = CastToFloatRecord(record);
        if(floatRecord) {
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
        if(shortRecord) {
            for(size_t j = 0 ; j < shortRecord->values.size() ; j++) {
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
    for(RIVReference *reference : references) {
        RIVTable *targetTable = reference->targetTable;
        targetTable->FilterRowsUnlinkedTo(this);
    }
}

void RIVTable::FilterRowsUnlinkedTo(RIVTable *table) {
    for(size_t row = 0 ; row < rows ; ++row) {
        for(RIVReference *reference : references) {
            if(reference->targetTable->GetName() == table->GetName() && !reference->HasReference(row)) {
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
    for(RIVReference *reference : references) {
        RIVTable* targetTable = reference->targetTable;
        std::pair<size_t,size_t>* targetIndexRange = reference->GetIndexReference(rowIndex);
        if(targetIndexRange != 0 && ( sourceReference == 0 || targetTable->name != sourceReference->sourceTable->name)) { //Do not update a table who just updated this table
            //            printf("Force table %s to filter row %zu\n",targetTable->name.c_str(),*targetIndex);
            if(targetIndexRange->second >= targetIndexRange->first) {
                bool runOnce = true;
                for(size_t filterIndex = targetIndexRange->first ; runOnce || filterIndex < targetIndexRange->second ; ++filterIndex) {
                    runOnce = false;
                    targetTable->FilterRow(filterIndex,filterOrUnfilter,reference);
                }
            }
            else throw "Target index range was malformed (second < first)";
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
    for(size_t i = 0 ; i < filters.size() ; i++) {
        Filter *filter = filters[i];
        if(filter->attributeName == name) {
            std::vector<Filter*>::iterator it = filters.begin();
            std::advance(it, i);
            filters.erase(it);
        }
    }
}

void RIVTable::AddFilter(Filter *filter) {
    //    printf("Filtering on ");
    //    filter->Print();
    filters.push_back(filter);
    filterRecords(filter);
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
    std::string rowText = "|";
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

void RIVTable::PrintAll() {
    printf("Table called %s has %zu records %zu rows and %zu references.\n",name.c_str(),records.size(),rows,references.size());
    std::string headerText = "|";
    std::string headerOrnament;
    
    int columnWidth = 15;
    
    for(size_t i = 0 ; i < records.size() ; i++) {
        RIVRecord* record = records[i];
        
        size_t textWidth = record->name.size();
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
    
    for(size_t j = 0 ; j < rows ; j++) {
        std::string rowText = RowToString(j);
        
        for(RIVReference *reference : references) {
            std::pair<size_t,size_t> *referenceIndexRange = reference->GetIndexReference(j);
            if(referenceIndexRange) {
                rowText += "---> " + reference->targetTable->name + " rows ";
                bool firstRun = true;
                for(size_t i = referenceIndexRange->first ; firstRun || i < referenceIndexRange->second ; i++) {
                    firstRun = false;
                    rowText += std::to_string(i) + ",";
                }
            }
        }
        if(filteredRows[j]) {
            rowText += "* FILTERED *";
        }
        
        printf("%s\n",rowText.c_str());
    }
    printf("%s\n",headerOrnament.c_str());
    
    //    PrintFilteredRowMap();
}

const RIVReference* RIVTable::GetReferenceToTable(std::string tableName) {
    for(const RIVReference *reference : references) {
        if(reference->targetTable->name == tableName) {
            return reference;
        }
    }
    return 0;
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
        iterator = new FilteredTableIterator(&filteredRows,rows);
    }
    else {
        iterator = new TableIterator(rows);
    }
    return iterator;
}

void AddOnChangeListener(void(*onChangeFunction)(const RIVTable*)) {
    
}

void RIVTable::PrintUnfiltered() {
    printf("Table called %s has %zu columns %zu rows and %zu references.\n",name.c_str(),records.size(),rows,references.size());
    std::string headerText = "|";
    std::string headerOrnament;
    
    int columnWidth = 15;
    
    for(size_t i = 0 ; i < records.size() ; i++) {
        RIVRecord* record = records[i];
        
        size_t textWidth = record->name.size();
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
    
    for(size_t j = 0 ; j < rows ; j++) {
        std::string rowText = RowToString(j);
        if(!filteredRows[j]) {
            for(RIVReference *reference : references) {
                std::pair<size_t,size_t> *referenceIndexRange = reference->GetIndexReference(j);
                if(referenceIndexRange) {
                    rowText += "---> " + reference->targetTable->name + " rows ";
                    bool firstRun = true;
                    for(size_t i = referenceIndexRange->first ; firstRun || i < referenceIndexRange->second ; i++) {
                        firstRun = false;
                        rowText += std::to_string(i) + ",";
                    }
                }
            }
            printf("%s\n",rowText.c_str());
        }
        printf("%s\n",headerOrnament.c_str());
    }
}
