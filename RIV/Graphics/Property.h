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
#include "SQLDataView.h"
#include "DataController.h"
#include "Reference.h"
#include "../helper.h"
	
class RIVTable;

enum INTERPOLATION_SCHEME {
	DISCRETE,
	CONTINUOUS
};

template <typename T>
class RIVProperty {
public:
	virtual void Start(sqlite::DataView* view) = 0; //Only necessary for dynamic properties
	virtual T Value() = 0;
	virtual void Stop() = 0; //Only necessary for dynamic properties
};

template <typename T>
class RIVEvaluatedProperty : RIVProperty<T> {
protected:
	
	sqlite::DataView* referralView;
	sqlite::Column* referralColumn;
	
	sqlite::Statement propertyStmt; //This statement will hold all the property values
	DataController* dataController;
	
	const INTERPOLATION_SCHEME defaultInterpolationMode = CONTINUOUS;
	
	//The type of interpolation used by the default interpolator, default is continuous
	INTERPOLATION_SCHEME interpolationMode;
	
	//The default evaluator to be used to evaluate a given row and return a size
	Evaluator<T,float>* defaultEvaluator = NULL;
	
	//Any optional special non-default interpolators to be used
//	std::vector<Evaluator<T,float>*> specificEvaluators;
	
	//Maps an index to a specific evaluator, if none is found, the default color interpolator is used
//	std::map<size_t,Evaluator<T,float>*> evaluatorRegister;
	
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
//		deletePointerVector(specificEvaluators);
//		evaluatorRegister.clear();
	}
	RIVEvaluatedProperty(sqlite::DataView* referralView, sqlite::Column* referralColumn,DataController* dataController) {
		this->dataController = dataController;
		this->referralView = referralView;
		this->referralColumn = referralColumn;
		
		//Create evaluator
		sqlite::Statement minMax = referralView->MinMax(referralColumn);
		minMax.Step();
		std::vector<T> interpolationValues;
		if(referralColumn->type == sqlite::INT) {
			interpolationValues.push_back(sqlite3_column_int(minMax.stmt,0));
			interpolationValues.push_back(sqlite3_column_int(minMax.stmt,1));
		}
		else {
			interpolationValues.push_back(sqlite3_column_double(minMax.stmt,0));
			interpolationValues.push_back(sqlite3_column_double(minMax.stmt,1));
		}
		defaultEvaluator = new LinearInterpolator<float>(interpolationValues);
	}
	//Get all the values required linked to the given view
	void Start(sqlite::DataView* view) {
		sqlite::Reference* reference = view->GetReference();
		std::string sql;
		if(reference != NULL && view->GetName() != referralView->GetName()) {
			sql = "SELECT " + referralColumn->name + " FROM " + referralView->GetName() + "," + view->GetName() + " WHERE " + reference->fromColumnName + " = " + reference->toColumnName + " GROUP BY " + reference->fromColumnName;
				printf("\n\t********* SQL = %s\n", sql.c_str());
			 sql = "SELECT DISTINCT " + referralColumn->name + "," + reference->toColumnName + " FROM " + referralView->GetName() + "," + view->GetName() + " WHERE " + reference->toColumnName + " = " + reference->fromColumnName;
			printf("\n\t********* SQL = %s\n", sql.c_str());
		}
		else
		{
			sql = "SELECT " + referralColumn->name + " FROM " + referralView->GetName();
		}
		propertyStmt = dataController->CustomSQLStmt(sql);
		
//		printf("***** PROPERTY SELECT STATEMENT\n");
//		propertyStmt.Print();
//		printf(" PROPERTY STATEMENT *****\n");
	}
	float Value() {
		if(propertyStmt.Step()) {
			if(referralColumn->type == sqlite::INT) {
				int value = sqlite3_column_int(propertyStmt.stmt, 0);
//				printf("Value = %d\n",value);
//				float eval = defaultEvaluator->Evaluate(value);
				return defaultEvaluator->Evaluate(value);
			}
			else if(referralColumn->type == sqlite::BIGINT) {
				size_t value = sqlite3_column_int(propertyStmt.stmt, 0);
				return defaultEvaluator->Evaluate(value);
			}
			else if(referralColumn->type == sqlite::REAL) {
				float value = sqlite3_column_double(propertyStmt.stmt, 0);
//				printf("value = %f\n",value);
				return defaultEvaluator->Evaluate(value);
			}
			else {
				printf("UNKNOWN TYPE\n");
			}
		}
//		printf("No more values!\n");
		return 0;
	}
	void Stop() {
		propertyStmt.Reset();
	}
};

#endif /* defined(__Afstuderen__GraphicsProperty__) */
