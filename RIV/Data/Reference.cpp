//
//  File.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Reference.h"

RIVSingleReference::RIVSingleReference(const std::map<size_t,size_t>& indexMap, RIVTable* sourceTable, RIVTable* targetTable) : RIVReference(sourceTable,targetTable){
	this->indexMap = indexMap;
}
std::pair<size_t*,ushort> RIVSingleReference::GetIndexReferences(size_t row) {
	return std::pair<size_t*,ushort>(&indexMap[row],1);
}
void RIVSingleReference::Print() {
	printf("MultiReference has %zu mappings : \n",indexMap.size());
	printMap(indexMap);
}
void RIVSingleReference::FilterReferenceRow(size_t row) {
	RIVReference::targetTable->FilterRow(row);
}
bool RIVSingleReference::HasReference(size_t row) {
	if ( indexMap.find(row) == indexMap.end() ) {
		return false;
	} else {
		return true;
	}
}
RIVMultiReference::RIVMultiReference(std::map<size_t,std::pair<size_t*,ushort>>& indexMap, RIVTable* sourceTable, RIVTable* targetTable) : RIVReference(sourceTable,targetTable) {
	this->indexMap = indexMap;
}
RIVSingleReference* RIVMultiReference::ReverseReference() {
	std::map<size_t,size_t> reverseMap;
	for(auto it : indexMap) {
		size_t row = it.first;
		size_t* referenceRows = it.second.first;
		ushort size = (ushort)it.second.second;
		for(size_t i = 0 ; i < size ; ++i) {
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
std::pair<size_t*,ushort> RIVMultiReference::GetIndexReferences(size_t row) {
	return indexMap[row];
}
void RIVMultiReference::FilterReferenceRow(size_t row) {
	std::pair<size_t*,ushort> rows = GetIndexReferences(row);
	for(ushort i = 0 ; i < rows.second ; ++i) {
//		printf("Filter row->referenceRow : %zu->%zu in table %s\n",row,rows.first[i],targetTable->GetName().c_str());
		targetTable->FilterRow(rows.first[i]);
	}
}
void RIVMultiReference::Print() {
	//		printf("MultiReference has %zu mappings\n",indexMap.size());
	for(auto it : indexMap) {
		std::cout << it.first << " : ";
		std::pair<size_t*,ushort> pair = it.second;
		std::cout << "[ ";
		for(int i = 0 ; i < pair.second ; ++i) {
			std::cout << pair.first[i]	<< " ";
		}
		std::cout << " ]\n";
	}
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
			std::pair<size_t*,ushort> mappedRows = reference->GetIndexReferences(row);
			if(mappedRows.first != NULL) {
				for(ushort i = 0 ; i < mappedRows.second ; ++i) {
					intermediateRows.push_back(mappedRows.first[i]);
				}
			}
		}
		sourceRows = intermediateRows;
	}
	return sourceRows;
}