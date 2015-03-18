//
//  Reference.h
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef RIVDataSet_Reference_h
#define RIVDataSet_Reference_h

#include <set>
#include <stdio.h>
#include <vector>
#include "helper.h"

class RIVTableInterface;
typedef unsigned short ushort;

//Ma
class ReferenceMap {
public:
    std::vector<size_t> targetRows;
//    bool dynamic;
//    static size_t liveCount;
    ~ReferenceMap();
    ReferenceMap();
//    ReferenceMap(ushort size, size_t* rows);
//    ReferenceMap(ushort size, size_t* rows,bool dynamic);
//    ReferenceMap& operator=( const ReferenceMap& otherMap) {
//        
////        printf("ReferenceMap assigned!\n");
//        
//        dynamic = true;
//        rows = new size_t[otherMap.size];
//        size = otherMap.size;
//        for(int i = 0 ; i < size ; ++i) {
//            rows[i] = otherMap.rows[i];
//        }
//        
//        return *this;
//    }
//    ReferenceMap(const ReferenceMap& otherMap) {
//
////        printf("ReferenceMap copied!\n");
//        
//        dynamic = true;
//        rows = new size_t[otherMap.size];
//        size = otherMap.size;
//        for(int i = 0 ; i < size ; ++i) {
//            rows[i] = otherMap.rows[i];
//        }
//        
//    }
};

class RIVReference {
protected:
	RIVReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable) : sourceTable(sourceTable), targetTable(targetTable) {
        test = 123;
	}
public:
    size_t test;
	RIVTableInterface* sourceTable;
	RIVTableInterface* targetTable;
    virtual std::vector<size_t> GetReferenceRows(size_t row) = 0;
	virtual bool HasReference(size_t row) = 0;
};

class RIVSingleReference : public RIVReference {
private:
	std::map<size_t,size_t> indexMap;
public:
	std::map<size_t,size_t>& GetIndexMap() {
		return indexMap;
	}
	~RIVSingleReference() {
		
	}
	RIVSingleReference(const std::map<size_t,size_t>& indexMap, RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	RIVSingleReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	std::vector<size_t> GetReferenceRows(size_t row);
	void Print();
	void AddReference(size_t fromRow, size_t toRow);
	void FilterReferenceRow(size_t row);
    void FilterReferenceRow(size_t row, std::set<RIVReference*>& visited);
	bool HasReference(size_t row);
};

//Class that maps a row in one table to multiple other rows in another table. The char represents a very short unsigned short (max 256) number of rows it maps to
class RIVMultiReference : public RIVReference {
private:
	std::map<size_t,std::vector<size_t>> indexMap;
public:
	//The index arrays are allocated on the stack
//	~RIVMultiReference() {
//		for(auto iter : indexMap) {
////            delete iter.second;
//		}
//	}
	std::map<size_t,std::vector<size_t>>& GetIndexMap() {
		return indexMap;
	}
	RIVMultiReference(std::map<size_t,std::vector<size_t>>& indexMap, RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	RIVMultiReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	RIVSingleReference* ReverseReference();
	bool HasReference(size_t row);
	void AddReferences(size_t fromRow, const std::vector<size_t>& toRows);
	std::vector<size_t> GetReferenceRows(size_t row);
	void FilterReferenceRow(size_t row);
    void FilterReferenceRow(size_t row, std::set<RIVReference*>& visited);
	void Print();
};
//Reference class that is set for a table where each row always has a fixed number of references rows, this means the reference rows can easily be calculated from the source row
class RIVFixedReference : public RIVReference {
private:
    const ushort size;
public:
    RIVFixedReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable,ushort size);
    std::vector<size_t> GetReferenceRows(size_t row);
    bool HasReference(size_t row);
    void FilterReferenceRow(size_t row);
    void FilterReferenceRow(size_t row, std::set<RIVReference*>& visited);
};

//Multiple RIVreferences chained together, makes it easy for a table A connected to table C through intermediary table B to resolve the depenedencies
class RIVReferenceChain {
private:
	std::vector<RIVReference*> references;
public:
	RIVReferenceChain() { }
	RIVReferenceChain(RIVReference* singleReference);
	RIVReferenceChain(const std::vector<RIVReference*>& references);
	//Remove last added reference, useful for backtracking algorithm used in RIVTableInterface::GetReferenceChainToTable(...)
	void PopReference();
	void AddReference(RIVReference* newReference);
	std::vector<size_t> ResolveRow(const size_t& row);
};


#endif
