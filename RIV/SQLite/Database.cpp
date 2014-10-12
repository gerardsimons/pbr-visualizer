//
//  Database.cpp
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Database.h"
#include "reporter.h"

namespace sqlite {
	//Open database from filename
//	Database::Database(char* fileName) {
//		int rc = sqlite3_open(fileName, &db);
//		if(rc != SQLITE_OK) {
//			throw "ERROR OPENING DATABASE ";
//		}
//	}
	Database::Database(sqlite3* db_) {
		db = db_;
	}
	void Database::DropTrigger(const std::string triggername) {
		executeSQL(("DROP TRIGGER IF EXISTS " + triggername).c_str());
	}
	Statement Database::SelectColumnsFromTable(const std::vector<std::string> columns, const std::string& table) const {
		if(columns.size() == 0) {
			throw "No columns set";
		}
//		printVector(columns);
		std::string sql = "SELECT ";
		for(size_t i = 0 ; i < columns.size() - 1 ; ++i) {
			sql += columns[i];
			sql += ",";
		}
		sql += columns[columns.size() - 1];
		
		sql += " FROM " + table;
		
		return PrepareStatement(sql);
	}
	Statement Database::PrepareStatement(const std::string& sql) const
	{
//		reporter::startTask("Preparing statement : " + sql);
		sqlite3_stmt* stmt;
		if(sqlite3_prepare(db, sql.c_str(), (int)sql.size() + 1, &stmt, 0))
		{
			const char* msg = sqlite3_errmsg(db);
			printf("SQL = %s\n",sql.c_str());
			printf("MSG = %s\n",msg);
			throw "ERROR while preparing statement ";
		}
//		reporter::stop("Preparing statement : " + sql);
		return Statement(stmt);
	}
	void Database::executeSQL(char const* sql) const {
		char *zErrMsg = 0;
		std::string taskName = "Executing SQL " + std::string(sql);
		reporter::startTask(taskName);
		int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
		if( rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			printf("SQL = %s : ",sql);
			sqlite3_free(zErrMsg);
			throw "SQL Error";
		}
		reporter::stop(taskName);
	}
	void Database::executeSQL(const std::string& sql) const {
		executeSQL(sql.c_str());
	}
	void Database::Delete(const std::string& tableName, Expression* expr) {
		std::string sql = "DELETE FROM " + tableName + " WHERE " + expr->SQL();
		executeSQL(sql);
	}
	void Database::PrintStmt(sqlite3_stmt* stmt) {
		const char* textStmt = sqlite3_sql(stmt);
		printf("Stmt text = %s\n",textStmt);
	}
	const char* Database::ErrorMessage() {
		return sqlite3_errmsg(db);
	}
}