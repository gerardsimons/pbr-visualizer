//
//  GraphicsProperty.h
//  Afstuderen
//
//  Created by Gerard Simons on 15/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef GRAPHICS_PROPERTY
#define GRAPHICS_PROPERTY

#include "Evaluator.h"
#include <sqlite3.h>
#include "../helper.h"
	
class RIVTable;

enum INTERPOLATION_SCHEME {
	DISCRETE,
	CONTINUOUS
};

class RIVProperty {
public:
	virtual bool Value(RIVTable*,const size_t&,float& value) = 0;
};

template <typename T>
class RIVEvaluatedProperty {
protected:
	sqlite3* database = NULL;
	std::string columnReference;
	std::string tableReference;
	
	const INTERPOLATION_SCHEME defaultInterpolationMode = CONTINUOUS;
	
	//The type of interpolation used by the default interpolator, default is continuous
	INTERPOLATION_SCHEME interpolationMode;
	
	//The default evaluator to be used to evaluate a given row and return a size
	Evaluator<T,float>* defaultEvaluator;
	
	//Any optional special non-default interpolators to be used
	std::vector<Evaluator<T,float>*> specificEvaluators;
	
	//Maps an index to a specific evaluator, if none is found, the default color interpolator is used
	std::map<size_t,Evaluator<T,float>*> evaluatorRegister;
	
	void init(const INTERPOLATION_SCHEME &scheme, const std::vector<T> &interpolationValues) {
		switch(scheme) {
			case CONTINUOUS:
			{
				defaultEvaluator = new LinearInterpolator<T>(interpolationValues);
				break;
			}
			case DISCRETE:
			{
				defaultEvaluator = new DiscreteInterpolator<T>(interpolationValues);
				break;
			}
				
		}
	}
	//SUPER HACK ALERT
	void init(RIVTable* propertyReference_, const INTERPOLATION_SCHEME &scheme, const std::vector<ushort> &interpolationValues) {
		std::vector<float> interpolationValuesFloats;
		for(ushort i : interpolationValues) {
			interpolationValuesFloats.push_back(i);
		}
		switch(scheme) {
			case CONTINUOUS:
			{
				defaultEvaluator = new LinearInterpolator<float>(interpolationValuesFloats);
				break;
			}
			case DISCRETE:
			{
				defaultEvaluator = new DiscreteInterpolator<float>(interpolationValuesFloats);
				break;
			}
		}
	}


//    float const* colorForMultipleResolvedRows(const std::vector<size_t>& rows);
public:
	~RIVEvaluatedProperty() {
		deletePointerVector(specificEvaluators);
		evaluatorRegister.clear();
	}
	bool Value(const std::string& columnName, const std::string& tableName, const size_t& row, float& computedValue) {
		
		//If the table used is the table reference we dont need to create a join
		if(tableName == tableReference) {
			
		}
		else {
			
		}

		Evaluator<T,float>* evaluator = evaluatorRegister[row];
		if(evaluator == NULL) {
			evaluator = defaultEvaluator;
		}
		computedValue = evaluator->Evaluate(row); //just use the row as input
		return true;

	}
	bool Value(RIVTable* sourceTable, const std::vector<size_t>& rows,float& computedValue) {
		//    float const* overallColor = NULL;
		Evaluator<T,float>* specificEvaluator = NULL;
		Evaluator<T,float>* evaluator = defaultEvaluator;
		size_t rowFound = 0;
		for(size_t i = 0 ; i < rows.size() ; ++i) {
			rowFound = rows[i];
			//Give preference to special interpolators
			specificEvaluator = evaluatorRegister[rowFound];
			if(specificEvaluator != NULL) {
				evaluator = specificEvaluator;
				break;
			}
		}
		computedValue = evaluator->Evaluate(rowFound); //just use the row as input
		return true;
	}
	void AddEvaluationScheme(std::vector<T>& indices, Evaluator<T, float>* newEvaluator) {
		for(size_t i : indices) {
			AddEvaluationScheme(i, newEvaluator);
		}
	}
	void AddEvaluationScheme(const T& index, Evaluator<T, float>* newEvaluator) {
		if(newEvaluator) {
			specificEvaluators.push_back(newEvaluator);
			evaluatorRegister[index] = newEvaluator;
		}
		else throw "New evaluator cannot be NULL.";
	}

	RIVEvaluatedProperty(sqlite3 *db,float fixedValue) {
		database = db;
		defaultEvaluator = new FixedEvaluator<T, float>(fixedValue);
	}
//	RIVEvaluatedProperty(sqlite3 *db,std::vector<size_t>& interpolationValues) {
//		init(propertyReference_, defaultInterpolationMode, interpolationValues);
//	}
//	RIVEvaluatedProperty(sqlite3 *db) {
//		T lower = 0;
//		T upper = propertyReference_->GetNumRows();
//		std::vector<T> interpolationValues;
//		interpolationValues.push_back(lower);
//		interpolationValues.push_back(upper);
//		init(propertyReference_, defaultInterpolationMode, interpolationValues);
//	}
//	RIVEvaluatedProperty(sqlite3 *db,std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme) {
//		init(propertyReference_, scheme, interpolationValues);
//	}
	RIVEvaluatedProperty(sqlite3 *db,const std::string tableName, const std::string& columnName) {
		//Get the min and the max of the
		
//		char* sql = "SELECT MIN(%s), MAX(%s) FROM %s";
//		sprintf(sql,columnName.c_str(),columnName.c_str(),tableName.c_str());
//		executeSQL(sql, db);
//		
//		if(referenceFloatRecord) {
//			std::vector<float> interpolationValues;
//			interpolationValues.push_back(referenceFloatRecord->Min());
//			interpolationValues.push_back(referenceFloatRecord->Max());
//			init(propertyReference_,defaultInterpolationMode,interpolationValues);
//		}
//		else {
//			std::vector<ushort> interpolationValues;
//			interpolationValues.push_back(referenceShortRecord->Min());
//			interpolationValues.push_back(referenceShortRecord->Max());
//			
//			init(propertyReference_,defaultInterpolationMode,interpolationValues);
//		}
//		if(!propertyReference->HasRecord(referenceRecord)) {
//			throw "Reference table does not contain reference record.";
//		}
	}
	RIVEvaluatedProperty(sqlite3 *db,const std::string& referenceColumnName) {
//		propertyReference = propertyReference_;
//		RIVRecord* referenceRecord = propertyReference->GetRecord(referenceRecordName);
//		initRecord(referenceRecord);
//		if(referenceFloatRecord) {
//			std::vector<float> interpolationValues;
//			interpolationValues.push_back(referenceFloatRecord->Min());
//			interpolationValues.push_back(referenceFloatRecord->Max());
//			init(propertyReference_,defaultInterpolationMode,interpolationValues);
//		}
//		else {
//			std::vector<ushort> interpolationValues;
//			interpolationValues.push_back(referenceShortRecord->Min());
//			interpolationValues.push_back(referenceShortRecord->Max());
//			
//			init(propertyReference_,defaultInterpolationMode,interpolationValues);
//		}
//		if(!propertyReference->HasRecord(referenceRecord)) {
//			throw "Reference table does not contain reference record.";
//		}
	}
};

#endif /* defined(__Afstuderen__GraphicsProperty__) */
