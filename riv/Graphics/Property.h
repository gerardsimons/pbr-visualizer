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
#include "../Data/Table.h"
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
	//The table whose index defines the value of the property
	RIVTable* propertyReference;
	
	RIVFloatRecord* referenceFloatRecord = NULL;
	RIVUnsignedShortRecord* referenceShortRecord = NULL;
	
	const INTERPOLATION_SCHEME defaultInterpolationMode = CONTINUOUS;
	
	//The type of interpolation used by the default interpolator, default is continuous
	INTERPOLATION_SCHEME interpolationMode = defaultInterpolationMode;
	
	//The default evaluator to be used to evaluate a given row and return a size
	Evaluator<T,float>* defaultEvaluator;
	
	//Any optional special non-default interpolators to be used
	std::vector<Evaluator<T,float>*> specificEvaluators;
	
	//Maps an index to a specific evaluator, if none is found, the default color interpolator is used
	std::map<size_t,Evaluator<T,float>*> evaluatorRegister;
	void setEvaluator() {
		
	}
	void init(const std::vector<T> &interpolationValues) {
		switch(interpolationMode) {
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
	void init(const std::vector<ushort> &interpolationValues) {
		//Now what? This is so hacky
		//convert to floats
		std::vector<float> interpolationValuesFloats;
		for(ushort i : interpolationValues) {
			interpolationValuesFloats.push_back(i);
		}
		switch(interpolationMode) {
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
	void initRecord(RIVRecord* record) {
		RIVFloatRecord *floatRecord =  RIVTable::CastToFloatRecord(record);
		if(floatRecord) {
			referenceFloatRecord = floatRecord;
			return;
		}
		RIVUnsignedShortRecord *shortRecord = RIVTable::CastToUnsignedShortRecord(record);
		if(shortRecord) {
			referenceShortRecord = shortRecord;
		}
	}
//    float const* colorForMultipleResolvedRows(const std::vector<size_t>& rows);
public:
	~RIVEvaluatedProperty() {
		deletePointerVector(specificEvaluators);
		evaluatorRegister.clear();
	}
	void Reset() {
		if(referenceFloatRecord) {
			std::pair<float,float> minMax = referenceFloatRecord->MinMax();
			std::vector<float> interpolationValues;
			interpolationValues.push_back(minMax.first);
			interpolationValues.push_back(minMax.second);
			init(interpolationValues);
		}
		
	}
	bool Value(RIVTable* sourceTable, const size_t& row, float& computedValue) {
		//Determine what interpolator we should use to compute the color
		if(sourceTable->GetName() == propertyReference->GetName()) {
			Evaluator<T,float>* evaluator = evaluatorRegister[row];
			if(evaluator == NULL) {
				evaluator = defaultEvaluator;
			}
			//If we want the value according to a certain record
			if(referenceFloatRecord) {
				computedValue = evaluator->Evaluate(referenceFloatRecord->Value(row));
			}
			else if(referenceShortRecord) {
				computedValue = evaluator->Evaluate(referenceShortRecord->Value(row));
			}
			else computedValue = evaluator->Evaluate(row); //just use the row as input
			return true;
		}
		else {
			//Find the table through its chain of references
			RIVReferenceChain chainToColorTable;
			//If found
			if(sourceTable->GetReferenceChainToTable(propertyReference->GetName(),chainToColorTable)) {
				//Find target index
				std::vector<size_t> targetRange = chainToColorTable.ResolveRow(row);
				if(targetRange.size() > 0){
//						size_t propertyIndex = (targetRange)[0]; //Very arbitrary, pass all and average?
					//Repeat using the new row and table
					return Value(propertyReference,targetRange,computedValue);
				}
			}
		}
		return false;
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
		//If we want the value according to a certain record
		if(referenceFloatRecord) {
			computedValue = evaluator->Evaluate(referenceFloatRecord->Value(rowFound));
		}
		else if(referenceShortRecord) {
			computedValue = evaluator->Evaluate(referenceShortRecord->Value(rowFound));
		}
		else computedValue = evaluator->Evaluate(rowFound); //just use the row as input
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

	RIVEvaluatedProperty(RIVTable *propertyReference_,float fixedValue) {
		propertyReference = propertyReference_;
		defaultEvaluator = new FixedEvaluator<T, float>(fixedValue);
	}
	RIVEvaluatedProperty(RIVTable *propertyReference_,std::vector<size_t>& interpolationValues) {
		propertyReference = propertyReference_;
		init(interpolationValues);
	}
	RIVEvaluatedProperty(RIVTable *propertyReference_) {
		propertyReference = propertyReference_;
		T lower = 0;
		T upper = propertyReference_->GetNumRows();
		std::vector<T> interpolationValues;
		interpolationValues.push_back(lower);
		interpolationValues.push_back(upper);
		init(propertyReference_, interpolationValues);
	}
	RIVEvaluatedProperty(RIVTable *propertyReference_,std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme) {
		interpolationMode = scheme;
		propertyReference = propertyReference_;
		init(propertyReference_, scheme, interpolationValues);
	}
	RIVEvaluatedProperty(RIVTable *propertyReference_,RIVRecord* referenceRecord) {
		interpolationMode = defaultInterpolationMode;
		propertyReference = propertyReference_;
		initRecord(referenceRecord);
		if(referenceFloatRecord) {
			std::vector<float> interpolationValues;
			interpolationValues.push_back(referenceFloatRecord->Min());
			interpolationValues.push_back(referenceFloatRecord->Max());
			init(propertyReference_,defaultInterpolationMode,interpolationValues);
		}
		else {
			std::vector<ushort> interpolationValues;
			interpolationValues.push_back(referenceShortRecord->Min());
			interpolationValues.push_back(referenceShortRecord->Max());
			
			init(propertyReference_,defaultInterpolationMode,interpolationValues);
		}
		if(!propertyReference->HasRecord(referenceRecord)) {
			throw "Reference table does not contain reference record.";
		}
	}
	RIVEvaluatedProperty(RIVTable *propertyReference_,const std::string& referenceRecordName) {
		interpolationMode = defaultInterpolationMode;
		propertyReference = propertyReference_;
		RIVRecord* referenceRecord = propertyReference->GetRecord(referenceRecordName);
		initRecord(referenceRecord);
		if(referenceFloatRecord) {
			std::vector<float> interpolationValues;
			interpolationValues.push_back(referenceFloatRecord->Min());
			interpolationValues.push_back(referenceFloatRecord->Max());
			init(interpolationValues);
		}
		else {
			std::vector<ushort> interpolationValues;
			interpolationValues.push_back(referenceShortRecord->Min());
			interpolationValues.push_back(referenceShortRecord->Max());
			init(interpolationValues);
		}
		if(!propertyReference->HasRecord(referenceRecord)) {
			throw "Reference table does not contain reference record.";
		}
	}
};

#endif /* defined(__Afstuderen__GraphicsProperty__) */
