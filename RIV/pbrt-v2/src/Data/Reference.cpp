//
//  File.cpp
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Reference.h"


	RIVReference::RIVReference(RIVTable* _sourceTable, RIVTable *_targetTable) {
		sourceTable = _sourceTable;
		targetTable = _targetTable;
	}

	//void RIVReference::AddReference(size_t sourceIndex, size_t targetIndex) {
	//    (*indexReferences)[sourceIndex] = targetIndex;
	//}

	void RIVReference::SetReferences(const std::map<size_t,std::vector<size_t> >& references) {
		indexReferences = references;
	}

	std::vector<size_t>* RIVReference::GetIndexReferences(const size_t& sourceIndex) {
		if(HasReference(sourceIndex)) {
			return &indexReferences[sourceIndex];
		}
		else return NULL;
	}

	bool RIVReference::HasReference(size_t sourceIndex) const {
		return indexReferences.count(sourceIndex) != 0;
	}

	RIVReference RIVReference::ReverseReference() {
		RIVReference reverse = RIVReference(targetTable,sourceTable);
		
		std::map<size_t,std::vector<size_t> > reverseIndexReferences = std::map<size_t,std::vector<size_t> >();
		
		for(auto it = indexReferences.begin() ; it != indexReferences.end() ; ++it) {
			std::vector<size_t> indexRange = it->second;
			bool firstRun = true;
			for(size_t i : indexRange)  {
				firstRun = false;
	//            printf("indexRange i = %d\n",i);
	//            printf("indexRange.second = %d\n",indexRange.second);
				std::vector<size_t> range;
				range.push_back(it->first);
				reverseIndexReferences[i] = range;
			}
		}

		reverse.SetReferences(reverseIndexReferences);
		
		return reverse;
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
				std::vector<size_t>* mappedRows = reference->GetIndexReferences(row);
				if(mappedRows != NULL) {
					for(size_t targetRow : (*mappedRows)) {
						intermediateRows.push_back(targetRow);
					}
				}
			}
			sourceRows = intermediateRows;
	//          Copy the new rows in to rows
	//        rows.clear();
	//        for(size_t copyRow : newRows) {
	//            rows.push_back(copyRow);
	//        }
	//        printVector(rows);
		}
		return sourceRows;
}