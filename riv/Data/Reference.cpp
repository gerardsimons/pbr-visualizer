//
//  File.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Reference.h"
#include "Table.h"
//size_t ReferenceMap::liveCount = 0;
ReferenceMap::ReferenceMap() {
//    printf("ReferenceMap default (empty) constructor.\n");
//    ++liveCount;
}
//ReferenceMap::ReferenceMap(ushort size,size_t* rows) : rows(rows), size(size), dynamic(false) {
//    printf("ReferenceMap NON dynamic constructor.\n");
//    ++liveCount;
//}
//ReferenceMap::ReferenceMap(ushort size, size_t* rows, bool dynamic) : rows(rows), size(size), dynamic(dynamic) {
//    printf("ReferenceMap dynamic constructor.\n");
//    ++liveCount;
//}
ReferenceMap::~ReferenceMap() {

}
void RIVFixedReference::FilterReferenceRow(size_t row)   {
    std::vector<size_t> rows = GetReferenceRows(row);
    for(ushort i = 0 ; i < rows.size() ; ++i) {
//        printf("Filter row->referenceRow : %zu->%zu in table %s\n",row,rows[i],targetTable->name.c_str());
        targetTable->FilterRowFromReference(rows[i],this);
    }
}
void RIVFixedReference::FilterReferenceRow(size_t row,std::set<RIVReference*>& visited)   {
    std::vector<size_t> rows = GetReferenceRows(row);
    for(ushort i = 0 ; i < rows.size() ; ++i) {
        //        printf("Filter row->referenceRow : %zu->%zu in table %s\n",row,rows[i],targetTable->name.c_str());
        targetTable->FilterRowFromReference(rows[i],this);
    }
}
void RIVSingleReference::FilterReferenceRow(size_t row) {
    targetTable->FilterRowFromReference(indexMap[row],this);
}
void RIVSingleReference::FilterReferenceRow(size_t row, std::set<RIVReference*>& visited) {
    targetTable->FilterRowFromReference(indexMap[row],visited);
}
//ReferenceMap::ReferenceMap(const std::vector<size_t> )
RIVSingleReference::RIVSingleReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable) : RIVReference(sourceTable,targetTable) {
    
}
RIVSingleReference::RIVSingleReference(const std::map<size_t,size_t>& indexMap, RIVTableInterface* sourceTable, RIVTableInterface* targetTable) : RIVReference(sourceTable,targetTable), indexMap(indexMap) {
}
std::vector<size_t> RIVSingleReference::GetReferenceRows(size_t row) {
    return (std::vector<size_t>){indexMap[row]};
}
void RIVSingleReference::Print() {
    printf("MultiReference has %zu mappings : \n",indexMap.size());
    printMap(indexMap);
}
//void RIVSingleReference::FilterReferenceRow(size_t row) {
//	RIVReference::targetTable->FilterRow(row);
//}
bool RIVSingleReference::HasReference(size_t row) {
    if ( indexMap.find(row) == indexMap.end() ) {
        return false;
    } else {
        return true;
    }
}
void RIVSingleReference::AddReference(size_t fromRow, size_t toRow) {
    indexMap[fromRow] = toRow;
}
RIVMultiReference::RIVMultiReference(std::map<size_t,std::vector<size_t>>& indexMap, RIVTableInterface* sourceTable, RIVTableInterface* targetTable) : RIVReference(sourceTable,targetTable), indexMap(indexMap) {
    
}
RIVMultiReference::RIVMultiReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable) : RIVReference(sourceTable,targetTable) {
    
}
void RIVMultiReference::AddReferences(size_t fromRow, const std::vector<size_t>& toRows) {
    indexMap[fromRow] = toRows;
}
RIVSingleReference* RIVMultiReference::ReverseReference() {
    std::map<size_t,size_t> reverseMap;
    for(auto it : indexMap) {
        size_t row = it.first;
        std::vector<size_t> referenceRows = it.second;
        for(size_t i = 0 ; i < referenceRows.size() ; ++i) {
            reverseMap[referenceRows[i]] = row;
        }
    }
    return new RIVSingleReference(reverseMap,targetTable,sourceTable);
}
bool RIVMultiReference::HasReference(size_t row) {
    if ( indexMap.find(row) == indexMap.end() ) {
        return false;
    } else {
        return true;
    }
}
std::vector<size_t> RIVMultiReference::GetReferenceRows(size_t row) {
    return indexMap[row];
}
void RIVMultiReference::FilterReferenceRow(size_t row) {
    std::vector<size_t> rows = GetReferenceRows(row);
    for(ushort i = 0 ; i < rows.size() ; ++i) {
        //		printf("Filter row->referenceRow : %zu->%zu in table %s\n",row,rows.first[i],targetTable->name.c_str());
        targetTable->FilterRowFromReference(rows[i],this);
    }
}
void RIVMultiReference::FilterReferenceRow(size_t row, std::set<RIVReference*>& visited) {
    std::vector<size_t> rows = GetReferenceRows(row);
    for(ushort i = 0 ; i < rows.size() ; ++i) {
        //		printf("Filter row->referenceRow : %zu->%zu in table %s\n",row,rows.first[i],targetTable->name.c_str());
        targetTable->FilterRowFromReference(rows[i],visited);
    }
}
void RIVMultiReference::Print() {
    //		printf("MultiReference has %zu mappings\n",indexMap.size());
    for(auto it : indexMap) {
        std::cout << it.first << " : ";
        std::vector<size_t> pair = it.second;
        std::cout << "[ ";
        for(int i = 0 ; i < pair.size() ; ++i) {
            std::cout << pair[i]	<< " ";
        }
        std::cout << " ]\n";
    }
}
RIVFixedReference::RIVFixedReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable, const ushort size) : RIVReference(sourceTable,targetTable), size(size) {
    if(size == 0) {
        throw std::runtime_error("Minimum of 1 references expected.");
    }
};
//Because the fixed reference requires that each source row has a fixed mapping
bool RIVFixedReference::HasReference(size_t row) {
    return true;
}
std::vector<size_t> RIVFixedReference::GetReferenceRows(size_t row) {
    
    std::vector<size_t> refRows(size);
    for(ushort i = 0 ; i < size ; ++i) {
        refRows[i] = row * size + i;
    }
    //    printArray(refRows, size);
    return refRows;
}
RIVReferenceChain::RIVReferenceChain(RIVReference* singleReference) {
    references.push_back(singleReference);
}
RIVReferenceChain::RIVReferenceChain(const std::vector<RIVReference*>& references_) {
    references = references_;
}
void RIVReferenceChain::AddReference(RIVReference* newReference) {
    if(newReference != NULL)
        references.push_back(newReference);
}
void RIVReferenceChain::PopReference() {
    references.pop_back();
}
std::vector<size_t> RIVReferenceChain::ResolveRow(const size_t& sourceRow) {
    
    std::vector<size_t> targetRows; //Contains the rows of the reference at the end of the chain.
    std::vector<size_t> sourceRows;
    
    sourceRows.push_back(sourceRow);
    
    for(RIVReference *reference : references) {
        std::vector<size_t> intermediateRows;
        for(size_t row : sourceRows) {
            std::vector<size_t> mappedRows = reference->GetReferenceRows(row);
            if(mappedRows.size()) {
                for(ushort i = 0 ; i < mappedRows.size() ; ++i) {
                    intermediateRows.push_back(mappedRows[i]);
                }
            }
        }
        sourceRows = intermediateRows;
    }
    return sourceRows;
}