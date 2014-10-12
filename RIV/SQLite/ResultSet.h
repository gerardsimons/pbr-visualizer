//
//  ResultSet.h
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__ResultSet__
#define __afstuderen_test__ResultSet__

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <sqlite3.h>

#include "Statement.h"

namespace sqlite {

class Row {
private:
	std::map<std::string,int>* colToIndex;
	sqlite3_stmt* statement;
public:
	Row() {
		
	}
	Row(sqlite3_stmt* statement, std::map<std::string,int>* colToIndex) {
		this->statement = statement;
		this->colToIndex = colToIndex;
	}
	int GetInt(const std::string& colName) {
		int columnIndex = colToIndex->at(colName);
		return sqlite3_column_int(statement, columnIndex);
	}
	int GetInt(int index) {
		return sqlite3_column_double(statement, index);
	}
	double GetDouble(const std::string& colName) {
		int columnIndex = colToIndex->at(colName);
		return sqlite3_column_double(statement, columnIndex);
	}
	float GetFloat(const std::string& colName) {
		int columnIndex = colToIndex->at(colName);
		return sqlite3_column_double(statement, columnIndex);
	}
};
	class Database;
class ResultSet {
private:
	Statement resultStatement;
	Database* db;
	std::map<std::string,int> colNamesToIndex;
public:
	ResultSet(sqlite3_stmt* statement, Database* db) {
		this->db = db;
		this->resultStatement = Statement(statement);
		if(sqlite3_step(statement) == SQLITE_ROW) {
			int cols = sqlite3_data_count(statement);
			for(int i = 0 ; i < cols ; ++i) {
				colNamesToIndex[sqlite3_column_name(statement, i)] = i;
			}
		}
		else {
//			printf("ERROR = %s\n",db->ErrorMessage());
		}
		sqlite3_reset(statement);
	}
	ResultSet(const Statement& statement, Database* db) {
		this->db = db;
		this->resultStatement = statement;
		if(resultStatement.Step()) {
			int cols = sqlite3_data_count(resultStatement.stmt);
			for(int i = 0 ; i < cols ; ++i) {
				colNamesToIndex[sqlite3_column_name(resultStatement.stmt, i)] = i;
			}
		}
		else {
			//			printf("ERROR = %s\n",db->ErrorMessage());
		}
		resultStatement.Reset();
	}
	bool GetNext(Row& row) {
		if(resultStatement.Step()) {
			row = Row(resultStatement.stmt,&colNamesToIndex);
			return true;
		}
		return false;
	}
	
};
}

#endif /* defined(__afstuderen_test__ResultSet__) */
