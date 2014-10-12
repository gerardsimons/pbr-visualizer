//
//  Selector.h
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__Selector__
#define __afstuderen_test__Selector__

#include <string>
#include "helper.h"

//Basic expression classes for filtering, deleting, updating

namespace sqlite {
	//Interface for all selectors
	class Expression {
	private:

	protected:



	public:
		virtual ~Expression() {
			
		}

		virtual std::string SQL() = 0;
	};

	
	//An expression that operates on a single column
	class SingularExpression : public Expression{
	protected:
		std::string columnName;
		SingularExpression(const std::string& columnName) {
			this->columnName = columnName;
		}
	public:
		std::string GetColumnName() {
			return columnName;
		}
		virtual std::string SQL() = 0;
	};
	
	class CompoundExpression {
	protected:
		std::vector<std::string> columnNames;
		std::vector<SingularExpression*> expressions;
		std::vector<std::string>& GetColumnNames() {
			return columnNames;
		}
		void createColumnNames() {
			for(SingularExpression* e : expressions) {
				columnNames.push_back(e->GetColumnName());
			}
		}
	public:
		CompoundExpression(const std::vector<SingularExpression*>& expressions) {
			if(expressions.size() == 0) {
				throw "At least 1 expressions required to construct a CompoundExpression";
			}
			this->expressions = expressions;
		}
	};
	
	class NotExpression : public SingularExpression {
	private:
		SingularExpression* expression;
	public:
		~NotExpression() {
			//			delete expression;
		}
		NotExpression(SingularExpression* expression) : SingularExpression(expression->GetColumnName()){
			this->expression = expression;
		}
		std::string SQL() {
			return "NOT(" + expression->SQL() + ")";
		}
		//Negates the not and returns the original expression
		Expression* Not() { //? What to do when this expression is destroyed?
			return expression;
		}
	};

	//A basic WHERE <ATTRIBUTE_NAME = VALUE> selector
	template <typename T>
	class EqualityExpression : public SingularExpression{
	private:
		T value;
	public:
		EqualityExpression(const std::string columnNam, const T& value) : SingularExpression(columnName) {
			this->value = value;
		}
		std::string SQL() {
			return columnName + " = " + std::to_string(value);
		}
		Expression* Not() {
			return new NotExpression(this);
		}
	};

	template <typename T>
	class RangeExpression : public SingularExpression {
	private:
		T min;
		T max;
	public:
		RangeExpression(const std::string columnName, const T& min,const T& max) : SingularExpression(columnName) {
			this->min = min;
			this->max = max;
		}
		std::string SQL() {
			return columnName + " > " + std::to_string(min) + " AND " + columnName + " < " + std::to_string(max);
		}
	};
	
	class AndExpression : public CompoundExpression {
	private:
		
	public:
		AndExpression(const std::vector<SingularExpression*> expressions) : CompoundExpression(expressions) {
			//Nothing to do here
		}
		std::string SQL() {
			std::string sql;
			for(size_t i = 0 ; i < expressions.size() ; i++) {
				SingularExpression* e = expressions[i];
				sql += e->SQL();
				printf("SQL= %s\n",sql.c_str());
				if(i != expressions.size() - 1) {
					sql += " AND ";
				}
			}
			return sql;
		}
	};
	
	class CustomSingularExpression : public SingularExpression {
	private:
		std::string sql;
	public:
		CustomSingularExpression(const std::string columnName, const std::string sql) : SingularExpression(columnName) {
			this->sql = sql;
		}
		std::string SQL() {
			return sql;
		}
	};

//	class CustomSelector {
//	private:
//		const std::string query;
//	public:
//		CustomSelector(const std::string& customQuery) : query(customQuery) {
//			
//		}
//	};
}

#endif /* defined(__afstuderen_test__Selector__) */
