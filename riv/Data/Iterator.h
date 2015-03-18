//
//  Iterator.h
//  Afstuderen
//
//  Created by Gerard Simons on 04/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__Iterator__
#define __Afstuderen__Iterator__

#include "Reference.h"
#include <map>
#include <vector>

class RIVReference;

class TableIterator {
protected:
	size_t index;
	size_t maxIndex;
    std::vector<RIVReference*> references;
public:
	TableIterator(size_t maxIndex_, const std::vector<RIVReference*>& references);
	void BackToStart();
	virtual bool GetNext(size_t& row);
    virtual bool GetNext(size_t& row, std::map<RIVTableInterface*,std::vector<size_t>>& refRowsMap);
	//Return the next row and its cluster information (if any requested, indicated by bool)
//	virtual bool GetNext(size_t& row, RIVCluster*& cluster, RIVClusterSet*& parentSet,bool requestCluster = false);
	virtual void Print() const;
};

class FilteredTableIterator : public TableIterator {
private:
	std::map<size_t,bool>* indexPointers;
public:
    FilteredTableIterator(std::map<size_t,bool> * _indexPointers, size_t maxIndex, const std::vector<RIVReference*>& references) : TableIterator(maxIndex,references){
		indexPointers = _indexPointers;
		index = 0;
	};
	
	virtual bool GetNext(size_t &row);
    bool GetNext(size_t& row, std::map<RIVTableInterface*,std::vector<size_t>>& refRowsMap);
	//    virtual bool GetNextCluster(size_t& row, RIVCluster*& cluster, RIVClusterSet*& clusterSet);
//	virtual bool GetNext(size_t& row, RIVCluster*& cluster, RIVClusterSet*& parentSet,bool requestCluster = false);
	void Print() const;
};

#endif