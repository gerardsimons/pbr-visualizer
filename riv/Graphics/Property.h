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
	
class RIVTableInterface;

enum INTERPOLATION_SCHEME {
	DISCRETE,
	CONTINUOUS
};

class RIVProperty {
public:
	virtual bool Value(RIVTableInterface*,const size_t&,float& value) = 0;
};

template <typename T>
class RIVEvaluatedProperty {
protected:
	//The table whose index defines the value of the property
	RIVTableInterface* propertyReference;
	
	RIVRecord<T>* referenceRecord = NULL;
	
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

//    float const* colorForMultipleResolvedRows(const std::vector<size_t>& rows);
public:
	~RIVEvaluatedProperty() {
		deletePointerVector(specificEvaluators);
		evaluatorRegister.clear();
	}
	void Reset() {
		if(referenceRecord) {
			std::pair<T,T> minMax = referenceRecord->MinMax();
			std::vector<T> interpolationValues;
			interpolationValues.push_back(minMax.first);
			interpolationValues.push_back(minMax.second);
			init(interpolationValues);
		}
		
	}
	bool Value(RIVTableInterface* sourceTable, const size_t& row, float& computedValue) {
		//Determine what interpolator we should use to compute the color
		if(sourceTable->name == propertyReference->name) {
			Evaluator<T,float>* evaluator = evaluatorRegister[row];
			if(evaluator == NULL) {
				evaluator = defaultEvaluator;
			}
			//If we want the value according to a certain record
			if(referenceRecord) {
				computedValue = evaluator->Evaluate(referenceRecord->Value(row));
			}
			else computedValue = evaluator->Evaluate(row); //just use the row as input
			return true;
		}
		else {
			//If found
			if(sourceTable->reference->targetTable->name == propertyReference->name) {
				//Find target index
				std::pair<size_t*,ushort> rows = sourceTable->reference->GetReferenceRows(row);
				if(rows.second > 0){
//						size_t propertyIndex = (targetRange)[0]; //Very arbitrary, pass all and average?
					//Repeat using the new row and table
					return Value(propertyReference,rows,computedValue);
				}
			}
		}
		return false;
	}
	bool Value(RIVTableInterface* sourceTable, const std::pair<size_t*,ushort>& rows,float& computedValue) {
		//    float const* overallColor = NULL;
		Evaluator<T,float>* specificEvaluator = NULL;
		Evaluator<T,float>* evaluator = defaultEvaluator;
		size_t rowFound = 0;
		for(size_t i = 0 ; i < rows.second ; ++i) {
			rowFound = rows.first[i];
			//Give preference to special interpolators
			specificEvaluator = evaluatorRegister[rowFound];
			if(specificEvaluator != NULL) {
				evaluator = specificEvaluator;
				break;
			}
		}
		//If we want the value according to a certain record
		if(referenceRecord) {
			computedValue = evaluator->Evaluate(referenceRecord->Value(rowFound));
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

	RIVEvaluatedProperty(RIVTableInterface *propertyReference_,float fixedValue) {
		propertyReference = propertyReference_;
		defaultEvaluator = new FixedEvaluator<T, float>(fixedValue);
	}
	RIVEvaluatedProperty(RIVTableInterface *propertyReference_,std::vector<size_t>& interpolationValues) {
		propertyReference = propertyReference_;
		init(interpolationValues);
	}
	RIVEvaluatedProperty(RIVTableInterface *propertyReference_) {
		propertyReference = propertyReference_;
		T lower = 0;
		T upper = propertyReference_->NumberOfRows();
		std::vector<T> interpolationValues;
		interpolationValues.push_back(lower);
		interpolationValues.push_back(upper);
		init(propertyReference_, interpolationValues);
	}
	RIVEvaluatedProperty(RIVTableInterface *propertyReference_,std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme) {
		interpolationMode = scheme;
		propertyReference = propertyReference_;
		init(propertyReference_, scheme, interpolationValues);
	}
	RIVEvaluatedProperty(RIVTableInterface *propertyReference_,RIVRecord<T>* referenceRecord) : propertyReference(propertyReference_), referenceRecord(referenceRecord) {
		interpolationMode = defaultInterpolationMode;
		propertyReference = propertyReference_;

		std::vector<T> interpolationValues;
		interpolationValues.push_back(referenceRecord->Min());
		interpolationValues.push_back(referenceRecord->Max());
		init(interpolationValues);
		
		
	}
};

#endif /* defined(__Afstuderen__GraphicsProperty__) */
