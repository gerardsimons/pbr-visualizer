//
//  Iterator.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 04/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Iterator.h"
#include "../helper.h"

TableIterator::TableIterator(size_t maxIndex_, RIVClusterSet* clusterSet_,std::vector<RIVReference*>* references) {
	this->references = references;
	maxIndex = maxIndex_;
	index = 0;
	clusterIndex = 0;
	clusterSet = clusterSet_;
};
void TableIterator::BackToStart() { index = 0; };
bool TableIterator::GetNextCluster(size_t& row, RIVCluster*& cluster, RIVClusterSet*& parentSet) {
	if(index >= maxIndex || (clusterSet && clusterIndex >= clusterSet->Size())) {
		return false;
	}
	else {
		cluster = clusterSet->GetCluster(clusterIndex);
		parentSet = clusterSet;
		row = cluster->GetMedoidIndex();
		clusterIndex++;
		index = row;
		return (clusterIndex - 1) < clusterSet->Size();
	}
}
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
bool TableIterator::GetNext(size_t& row, size_t*& refRow) {
	if(index >= maxIndex) {
		return false;
	}
	else {
		row = index;
		for(size_t j = 0 ; j < references->size() ; ++j) {
			std::pair<size_t*,ushort> refRows = references->at(j)->GetIndexReferences(row);
			if(refRows.first) {
				refRow = &refRows.first[0];
			}
		}
		index++;
		return (index - 1) < maxIndex;
	}
}
bool TableIterator::GetNext(size_t& row, RIVCluster*& cluster, RIVClusterSet*& parentSet,bool requestCluster) {
	if(index >= maxIndex) {
		return false;
	}
	else {
		row = index;
		if(requestCluster) {
			cluster = clusterSet->ClusterForMemberIndex(index);
			parentSet = clusterSet;
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
bool FilteredTableIterator::GetNext(size_t& row, size_t*& refRow) {
	if(index < maxIndex) {
		bool filtered = (*indexPointers)[index];
		while(filtered && index < maxIndex) {
			index++;
			filtered = (*indexPointers)[index];
			//                printf("index = %zu\n",index);
		}
		row = index;
		for(size_t j = 0 ; j < references->size() ; ++j) {
			std::pair<size_t*,ushort> refRows = references->at(j)->GetIndexReferences(row);
			if(refRows.first) {
				refRow = &refRows.first[0];
			}
		}
		index++;
		return !filtered && index <= maxIndex;
	}
	return false;
}

bool FilteredTableIterator::GetNext(size_t& row, RIVCluster*& cluster, RIVClusterSet*& parentSet,bool requestCluster) {
	if(index < maxIndex) {
		bool filtered = (*indexPointers)[index];
		while(filtered && index < maxIndex) {
			index++;
			filtered = (*indexPointers)[index];
			//                printf("index = %zu\n",index);
		}
		row = index;
		if(clusterSet) {
			cluster = clusterSet->ClusterForMemberIndex(row);
			parentSet = clusterSet;
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