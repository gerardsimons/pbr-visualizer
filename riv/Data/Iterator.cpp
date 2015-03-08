//
//  Iterator.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 04/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Iterator.h"
#include "../helper.h"

TableIterator::TableIterator(size_t maxIndex, const std::vector<RIVReference*>& references) : maxIndex(maxIndex), references(references) {
	index = 0;
};
void TableIterator::BackToStart() {
	index = 0;
};
bool TableIterator::GetNext(size_t& row) {
	if(index >= maxIndex) {
		return false;
	}
	else {
		row = index;
		index++;
		return (index - 1) < maxIndex;
	}
}
bool TableIterator::GetNext(size_t& row, std::map<RIVTableInterface*,std::pair<size_t*,ushort>>& refRowsMap) {
	if(index >= maxIndex) {
		return false;
	}
	else {
		row = index;

        for(RIVReference* reference : references) {
            const std::pair<size_t*,ushort>& refRows = reference->GetReferenceRows(row);
            if(refRows.first) {
                refRowsMap[reference->targetTable] = refRows;
            }
        }
		index++;
		return (index - 1) < maxIndex;
	}
}
void TableIterator::Print() const {
	printf("TableIterator (maxIndex = %zu) \n", maxIndex);
}

bool FilteredTableIterator::GetNext(size_t& row) {
	if(index < maxIndex) {
		bool filtered = (*indexPointers)[index];
		while(filtered && index < maxIndex) {
			index++;
			filtered = (*indexPointers)[index];
			//                printf("index = %zu\n",index);
		}
		row = index;
		index++;
		return !filtered && index <= maxIndex;
	}
	return false;
}
bool FilteredTableIterator::GetNext(size_t& row, std::map<RIVTableInterface*,std::pair<size_t*,ushort>>& refRowsMap) {
	if(index < maxIndex) {
		bool filtered = (*indexPointers)[index];
		while(filtered && index < maxIndex) {
			index++;
			filtered = (*indexPointers)[index];
			//                printf("index = %zu\n",index);
		}
		row = index;
		for(size_t j = 0 ; j < references.size() ; ++j) {
            RIVReference* reference = references[j];
			std::pair<size_t*,ushort> refRows = reference->GetReferenceRows(row);
            //TODO: Not sure what this check is for
//			if(refRows.first) {
//				refRow = &refRows.first[0];
//			}
            refRowsMap[reference->targetTable] = refRows;
		}
		index++;
		return !filtered && index <= maxIndex;
	}
	return false;
}

//TODO: How would such a function make sense for a filtered dataset?

//bool FilteredTableIterator::GetNextCluster(size_t &row, RIVCluster *&cluster, RIVClusterSet *&clusterSet) {
//    if(index < maxIndex) {
//        bool filtered = (*indexPointers)[index];
//        while(filtered && index < maxIndex) {
//            index++;
//            filtered = (*indexPointers)[index];
//            //                printf("index = %zu\n",index);
//        }
//        row = index;
//        index++;
//        return !filtered && index <= maxIndex;
//    }
//    return false;
//}

void FilteredTableIterator::Print() const {
	printf("FilteredTableIterator index = %zu map of iterator object has %zu values :\n", index, maxIndex);
	printMap(*indexPointers);
}