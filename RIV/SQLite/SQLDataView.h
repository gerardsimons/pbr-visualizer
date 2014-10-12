//
//  SQLDataView.h
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__SQLDataView__
#define __afstuderen_test__SQLDataView__

#include "Reference.h"
#include "Database.h"
#include "ResultSet.h"
#include "Statement.h"
#include "Expression.h"

#include <sqlite3.h>

//This class models a temporary view in SQL
namespace sqlite {
	class OnDataChangedListener {
	public:
		virtual void OnDataChanged() = 0;
	};
	enum Type {
		REAL,
		INT,
		BIGINT,
	};
	class Column {
	public:
		Column(Type type, const std::string& name, int index) {
			this->type = type;
			this->name = name;
			this->index = index;
		}
		int index;
		Type type;
		std::string name;
	};
	
	class DataView {
	private:
		Database* db;
		Reference* reference = NULL;
		
		static const std::string prefix;
		
		std::string sourceName;
		std::string name;
		std::string selectSQL;
		std::string asSQL;
		
		bool modifiable = false;
		bool changesMade = false; //Were there any changes made during the time the table was locked down?
		
		bool selectCreated = false;
		
		Statement selectStatement;
		//Keep track of what expressions were used to filter (delete) data from this view so that it may be negated when filters are to be removed (re-inserted)
		std::map<std::string,SingularExpression*> filterExpressions;  //?TODO: This should probably be a string,expression map, mapping columns to a single (or none) filters
		
		std::vector<Column> columns;
		std::vector<OnDataChangedListener*> listeners;
		
		AndExpression* compoundFilterExpression = NULL;
		
		sqlite::Type TypeFromStringType(const std::string& typeString) {
			if(typeString == "UNSIGNED INTEGER" || typeString == "INT" || typeString == "TINYINT") {
				return INT;
			}
			else if(typeString == "REAL") {
				return sqlite::REAL;
			}
			else if (typeString == "UNSIGNED BIG INT") {
				return sqlite::BIGINT;
			}
			else {
				throw "UNKNOWN TYPE";
			}
		}
		
		void determineColumns() {
			//Determine the column names
			Statement stmt = db->PrepareStatement("pragma table_info (" + name  + " )");
			int index = 1;
			while(stmt.Step())
			{
				char name[50];
				char typeName[50];
				sprintf(name, "%s", sqlite3_column_text(stmt.stmt, 1));
				sprintf(typeName, "%s", sqlite3_column_text(stmt.stmt, 2));
				Type t = TypeFromStringType(typeName);
				columns.push_back(Column(t,name,index));
				++index;
			}
		}
		//Create one big AND expression of all the filter expressions present, useful to create a single SQL statement
		AndExpression* createFilterExpression() {
			if(compoundFilterExpression) {
//				delete compoundFilterExpression;
			}
			if(filterExpressions.size() > 0) {
				std::vector<SingularExpression*> pointers;
				for(auto it : filterExpressions) {
					pointers.push_back(it.second);
				}
				compoundFilterExpression = new AndExpression(pointers);
				return compoundFilterExpression;
			}
			else return NULL;
		}
	public:
		//The name of the view and the SQL that defines how this view should be populated (AS operator)
		DataView(Database* db, const std::string& sourceName, const std::string& asSQL) {
			if(db == NULL) {
				throw "NULL database given.";
			}
			this->db = db;
			this->sourceName = sourceName;
			this->name = prefix + sourceName;
			this->asSQL = asSQL;
			
			Create();
			
			determineColumns();
			
			//Generate statements
			selectSQL = "SELECT * FROM " + name;
			selectStatement = db->PrepareStatement(selectSQL);
		}
		DataView(Database* db, const std::string& sourceName, const std::string& asSQL, Reference* reference_) : reference(reference_){
			if(db == NULL) {
				throw "NULL database given.";
			}
			this->db = db;
			this->sourceName = sourceName;
			this->name = prefix + sourceName;
			this->asSQL = asSQL;
			
			//Determine
			Create();
			
			//Determine the column names
			determineColumns();
		}
		void StartModifying() {
			if(!modifiable) {
				modifiable = true;
				changesMade = false;
			}
			else throw "Already in modifiable mode.";
		}
		void SetReference(Reference* newReference) {
			this->reference = newReference;
		}
		void StopModifying() {
			if(modifiable) {
				modifiable = false;
				if(reference) { //Make sure the reference view is in-sync with this view
					std::string SQL = "DELETE FROM " + reference->toView->name + "; "\
					"INSERT INTO " + reference->toView->name + " SELECT * FROM " + reference->toView->sourceName + " WHERE EXISTS ( SELECT 1 FROM "  + name + " WHERE " + reference->fromColumnName + " = " + reference->toColumnName + " ) ";
					
					//insert into riv_paths select * from paths where exists (select 1 from riv_intersections where id = pid);
					
					db->executeSQL(SQL);
				}
				if(changesMade) {
					NotifyListeners();
				}
			}
			else throw "Not in modifiable mode.";
		}
		//Remove this view from the database if the object is destroyed
		~DataView() {
			Drop();
		}
		void AddListener(OnDataChangedListener* newListener) {
			listeners.push_back(newListener);
		}
		void NotifyListeners() {
			for(OnDataChangedListener* listener : listeners) {
				listener->OnDataChanged();
			}
		}
		void Drop() {
			db->executeSQL("DROP TABLE IF EXISTS " + name);
		}
		void Create() {
			//Drop any previously existing tables and create anew
//			Drop();
//			db->executeSQL("CREATE TABLE IF NOT EXISTS " + name + " AS " + asSQL);
			db->executeSQL("INSERT INTO " + name + " SELECT * FROM " + sourceName);
		}
		Statement MinMax(const std::string& column) {
			std::string sql = "SELECT ";
			sql += "MIN(" + column + ")" + ", MAX(" + column + ") FROM " + name;
			return db->PrepareStatement(sql);
		}
		Statement MinMax(Column* column) {
			std::string sql = "SELECT ";
			sql += "MIN(" + column->name + ")" + ", MAX(" + column->name + ") FROM " + name;
			return db->PrepareStatement(sql);
		}
		Statement MinMax(const std::vector<std::string>& columns) {
			if(columns.size() == 0) {
				throw "At least 1 column required";
			}
			std::string sql = "SELECT ";
			
			for(size_t i = 0 ; i < columns.size() - 1 ; ++i) {
				sql += "MIN(" + columns[i] + ")" + ", MAX(" + columns[i] + "), ";
			}
			sql += "MIN(" + columns[columns.size() - 1] + ")" + ", MAX(" + columns[columns.size() - 1] + ") FROM " + name;
			return db->PrepareStatement(sql);
		}
		Reference* GetReference() {
			return reference;
		}
		ResultSet MinMaxAll() {
			if(columns.size() == 0) {
				throw "At least 1 column required";
			}
			std::string sql = "SELECT ";
			
			for(size_t i = 0 ; i < columns.size() - 1 ; ++i) {
				sql += "MIN(" + columns[i].name + ")" + ", MAX(" + columns[i].name + "), ";
			}
			sql += "MIN(" + columns[columns.size() - 1].name + ")" + ", MAX(" + columns[columns.size() - 1].name + ") FROM " + name;
//			return
			return ResultSet(db->PrepareStatement(sql),db);
		}
		int NumberOfColumns() {
			return (int)columns.size();
		}
		std::string GetName() {
			return name;
		}
		Column* GetColumn(const std::string& columnName) {
			for(size_t i = 0 ; i < columns.size() ; ++i) {
				Column& col = columns[i];
				if(col.name == columnName) {
					return &col;
				}
			}
			throw "No such column";
		}
		std::vector<Column> GetColumns() {
			return columns;
		}
		void ResetSelect() {
			if(!selectCreated) {
				//Generate statements
				selectSQL = "SELECT * FROM " + name;
				selectStatement = db->PrepareStatement(selectSQL);
				selectCreated = true;
			}
			if(!selectStatement.Reset()) {
				//Recreate
				selectStatement = db->PrepareStatement(selectSQL);
			}
		}
		void Reinsert() {
			AndExpression* allExpr = createFilterExpression();
			if(allExpr != NULL) {
				std::string insertSQL = "INSERT INTO " + name + " SELECT * FROM " + sourceName + " WHERE " + allExpr->SQL() + " ";
				changesMade = true;
				db->executeSQL(insertSQL);
			}
			else {
				std::string insertSQL = "INSERT INTO " + name + " SELECT * FROM " + sourceName;
				changesMade = true;
				db->executeSQL(insertSQL);
			}
		}
		void RemoveFilter(SingularExpression* filterExpression) {
			if(!modifiable) {
				throw "Call startModifying before doing any filtering operations";
			}
			//Delete the filter expression from the filter register
			bool exprFound = false;
			SingularExpression* expr = filterExpressions[filterExpression->GetColumnName()];
			if(expr != filterExpression) {
				exprFound = true;
				filterExpressions.erase(expr->GetColumnName());
				delete expr;
			}
			if(exprFound) {
				//If some filter was deleted, we have to update the table
				//Reinsert all those that pass all of the other filter expressions
				Reinsert();
			}
			else printf("No such filter expression found.\n");
		}
		void RemoveFilter(const std::string& name) {
			if(!modifiable) {
				throw "Call startModifying before doing any filtering operations";
			}
			//Delete the filter expression from the filter register
			bool exprFound = false;
			SingularExpression* expr = filterExpressions[name];
			if(expr != NULL) {
				exprFound = true;
				filterExpressions.erase(expr->GetColumnName());
				delete expr;
			}
			if(exprFound) {
				//If some filter was deleted, we have to update the table
				//Reinsert all those that pass all of the other filter expressions
				Reinsert();
			}
		}
		//TODO: It is not always required to remove the old expression
		void Filter(SingularExpression* newFilterExpr) {
			//Delete
			//If some filter expression already exists for this view, remove them
			if(!modifiable) {
				throw "Call startModifying before doing any filtering operations";
			}
			std::vector<Expression*> toRemove;
			std::string columnName = newFilterExpr->GetColumnName();
			SingularExpression *expr = filterExpressions[columnName];
			if(expr != NULL) {
				RemoveFilter(expr); //Remove the old filter. Update this table view, re-insert values
			}
			filterExpressions[columnName] = newFilterExpr;
		
			//And delete according to the new expression
			AndExpression* andExpr = createFilterExpression();
			std::string deleteSQL = "DELETE FROM " + name + " WHERE " + andExpr->SQL();
			changesMade = true;
			db->executeSQL(deleteSQL);
		}

		void Select(const Expression& filterExpression) {
			//Basically a Not(Select)
		}
		ResultSet Select() {
			ResetSelect();
			return ResultSet(selectStatement,db);
		}
		Statement SelectStmt() {
			ResetSelect();
			return selectStatement;
		}
		//A custom selection of columns, do not use the precompiled statement
		Statement SelectStmt(const std::vector<std::string>& column_names) {
			return db->SelectColumnsFromTable(column_names, name);
		}
	};
}

#endif /* defined(__afstuderen_test__SQLDataView__) */
