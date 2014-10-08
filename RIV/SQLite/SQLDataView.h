//
//  SQLDataView.h
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__SQLDataView__
#define __afstuderen_test__SQLDataView__

#include "Database.h"
#include "ResultSet.h"

#include <sqlite3.h>

//This class models a temporary view in SQL
namespace sqlite {
	
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
		
		std::string name;
		std::string selectSQL;
		std::string asSQL;
		
		sqlite3_stmt* selectStatement = NULL;
		sqlite3_stmt* selectAllStatement = NULL;
		
		std::vector<Column> columns;
		
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
			sqlite3_stmt* stmt = db->PrepareStatement("pragma table_info (" + name  + " )");
			int index = 1;
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
				char name[50];
				char typeName[50];
				sprintf(name, "%s", sqlite3_column_text(stmt, 1));
				sprintf(typeName, "%s", sqlite3_column_text(stmt, 2));
				Type t = TypeFromStringType(typeName);
				columns.push_back(Column(t,name,index));
				++index;
			}
			
		}
	public:
		//The name of the view and the SQL that defines how this view should be populated (AS operator)
		DataView(Database* db, const std::string& name, const std::string& asSQL) {
			if(db == NULL) {
				throw "NULL database given.";
			}
			this->db = db;
			this->name = name;
			this->asSQL = asSQL;
			
			Create();
			
			determineColumns();
			
			//Generate statements
			selectSQL = "SELECT * FROM " + name;
			selectStatement = db->PrepareStatement(selectSQL);
		}
		DataView(Database* db, const std::string& name, const std::string& asSQL, DataView* linkedTo) {
			if(db == NULL) {
				throw "NULL database given.";
			}
			this->db = db;
			this->name = name;
			this->asSQL = asSQL;
			
			//Determine
			Create();
			
			//Determine the column names
			determineColumns();
			
			//Generate statements
			selectSQL = "SELECT * FROM " + name;
			selectStatement = db->PrepareStatement(selectSQL);
		}
		//Remove this view from the database if the object is destroyed
		~DataView() {
			Drop();
		}
		void Drop() {
			db->executeSQL("DROP IF EXISTS " + name);
		}
		void Create() {
			db->executeSQL("CREATE VIEW IF NOT EXISTS " + name + " AS " + asSQL);
		}
		sqlite3_stmt* MinMax(const std::string& column) {
			std::string sql = "SELECT ";
			sql += "MIN(" + column + ")" + ", MAX(" + column + ") FROM " + name;
			return db->PrepareStatement(sql);
		}
		sqlite3_stmt* MinMax(const std::vector<std::string>& columns) {
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
		std::vector<Column> GetColumns() {
			return columns;
		}
		std::string GetSelectSQL() {
			return selectSQL;
		}
		void ResetSelect() {
			if(!db->ResetStatement(selectStatement)) {
				//Recreate
				selectStatement = db->PrepareStatement(selectSQL);
			}
		}
		ResultSet Select() {
			ResetSelect();
			return ResultSet(selectStatement,db);
		}
		sqlite3_stmt* SelectStmt() {
			ResetSelect();
			return selectStatement;
		}
		sqlite3_stmt* SelectStmt(const std::vector<std::string>& column_names) {
			return db->SelectColumnsFromTable(column_names, name);
		}
	};
	
	class Reference {
	public:
		DataView* from;
		DataView* to;
		std::string fromColumnName;
		std::string toColumnName;
	};
}

#endif /* defined(__afstuderen_test__SQLDataView__) */
