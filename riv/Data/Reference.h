//
//  Reference.h
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef RIVDataSet_Reference_h
#define RIVDataSet_Reference_h

#include <stdio.h>
#include <vector>
#include "helper.h"

class RIVTableInterface;
typedef unsigned short ushort;

class RIVReference {
protected:
	RIVReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable) : sourceTable(sourceTable), targetTable(targetTable) {
		
	}
public:
	RIVTableInterface* sourceTable;
	RIVTableInterface* targetTable;
	virtual std::pair<size_t*,ushort> GetReferenceRows(size_t row) = 0;
	virtual bool HasReference(size_t row) = 0;
};

class RIVSingleReference : public RIVReference {
private:
	std::map<size_t,size_t> indexMap;;
public:
	RIVSingleReference(const std::map<size_t,size_t>& indexMap, RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	RIVSingleReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	std::pair<size_t*,ushort> GetReferenceRows(size_t row);
	void Print();
	void AddReference(size_t fromRow, size_t toRow);
	void FilterReferenceRow(size_t row);
	bool HasReference(size_t row);
};

//Class that maps a row in one table to multiple other rows in another table. The char represents a very short unsigned short (max 256) number of rows it maps to
class RIVMultiReference : public RIVReference {
public:
	std::map<size_t,std::pair<size_t*,ushort> > indexMap;
	RIVMultiReference(std::map<size_t,std::pair<size_t*,ushort> >& indexMap, RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	RIVMultiReference(RIVTableInterface* sourceTable, RIVTableInterface* targetTable);
	RIVSingleReference* ReverseReference();
	bool HasReference(size_t row);
	void AddReferences(size_t fromRow, const std::pair<size_t*,ushort>& toRows);
	std::pair<size_t*,ushort> GetReferenceRows(size_t row);
	void FilterReferenceRow(size_t row);
	void Print();
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
