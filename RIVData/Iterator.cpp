//
//  Iterator.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 04/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Iterator.h"
#include "helper.h"

TableIterator::TableIterator(size_t maxIndex_, RIVClusterSet* clusterSet_) {
    maxIndex = maxIndex_;
    index = 0;
    clusterIndex = 0;
    clusterSet = clusterSet_;
};
void TableIterator::BackToStart() { index = 0; };
bool TableIterator::GetNext(size_t& row, RIVCluster*& cluster, RIVClusterSet*& parentSet, bool byCluster) {
    if(index >= maxIndex || (clusterSet && byCluster && clusterIndex >= clusterSet->Size())) {
        return false;
    }
    else {
        //If any cluster data is given, and an actual cluster is requested
        if(clusterSet && byCluster) {
            cluster = clusterSet->GetCluster(clusterIndex);
            parentSet = clusterSet;
            row = cluster->GetMedoidIndex();
            clusterIndex++;
            index = row;
            return (clusterIndex - 1) < clusterSet->Size();
        }
        else {
            row = index;
            index++;
            return (index - 1) < maxIndex;
        }
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
void TableIterator::Print() const {
    printf("TableIterator (maxIndex = %zu) \n", maxIndex);
}

bool FilteredTableIterator::GetNext(size_t &row,RIVCluster* cluster) {
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

void FilteredTableIterator::Print() const {
    printf("FilteredTableIterator index = %zu map of iterator object has %zu values :\n", index, maxIndex);
    printMap(*indexPointers);
}