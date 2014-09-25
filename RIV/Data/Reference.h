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

#include "Table.h"



	class RIVTable;

	//Maps rows from a sourceTable to a targetTable
	class RIVReference {
	public:
		//The source table is implicit as a reference is supplied to a table
		RIVTable *targetTable;
		RIVTable *sourceTable;
		
		~RIVReference() {
	//        delete indexReferences; //Very important as index references is created as new in reverse function
		}
		
		std::map<size_t,std::vector<size_t> > indexReferences;
		
		RIVReference(RIVTable *_sourceTable, RIVTable *_targetTable);
		bool HasReference(size_t) const; //Does it have a reference from a given source index
	//    void AddReference(size_t sourceIndex, size_t targetIndex);
		void SetReferences(const std::map<size_t,std::vector<size_t> >&);
		std::vector<size_t>* GetIndexReferences(const size_t&);
		RIVReference* ReverseReference();
		void FilterReferenceRow(size_t row);
	};

	//Multiple RIVreferences chained together, makes it easy for a table A connected to table C through intermediary table B to resolve the depenedencies
	class RIVReferenceChain {
	private:
		std::vector<RIVReference*> references;
	public:
		RIVReferenceChain() { }
		RIVReferenceChain(RIVReference* singleReference);
		RIVReferenceChain(const std::vector<RIVReference*>& references);
		//Remove last added reference, useful for backtracking algorithm used in RIVTable::GetReferenceChainToTable(...)
		void PopReference();
		void AddReference(RIVReference* newReference);
		std::vector<size_t> ResolveRow(const size_t& row);
	};


#endif
