//
//  Database.h
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__Database__
#define __afstuderen_test__Database__

#include <sqlite3.h>
#include <vector>
#include <string>

namespace sqlite {
	class Database {
	private:
		
		sqlite3* db;
	public:
		//Open database from filename
		Database(char* fileName) {
			int rc = sqlite3_open(fileName, &db);
			if(rc != SQLITE_OK) {
				throw "ERROR OPENING DATABASE ";
			}
		}
		Database(sqlite3* db_) : db(db_) {
			
		}
		sqlite3_stmt* SelectColumnsFromTable(const std::vector<std::string> columns, const std::string& table) const {
			if(columns.size() == 0) {
				throw "No columns set";
			}
			std::string sql = "SELECT ";
			for(size_t i = 0 ; i < columns.size() - 1 ; ++i) {
				sql += columns[i];
				sql += ",";
			}
			sql += columns[columns.size() - 1];
			
			sql += " FROM " + table;
			
			sqlite3_stmt* statement = PrepareStatement(sql);
			return statement;
		}
		 sqlite3_stmt* PrepareStatement(const std::string& sql) const {
			 sqlite3_stmt* statement = NULL;
			if(sqlite3_prepare(db, sql.c_str(), (int)sql.size() + 1, &statement, 0)) {
				const char* msg = sqlite3_errmsg(db);
				printf("SQL = %s\n",sql.c_str());
				printf("MSG = %s\n",msg);
				throw "ERROR while preparing statement ";
			}
			return statement;
		}
		bool ResetStatement(sqlite3_stmt* stmt) {
			int rc = sqlite3_reset(stmt);
			if(rc != SQLITE_OK) {
				return false;
			}
			return true;
		}
		void executeSQL(char const* sql) const {
			char *zErrMsg = 0;
			int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
			if( rc != SQLITE_OK ){
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				printf("SQL = %s : ",sql);
				sqlite3_free(zErrMsg);
				throw "SQL Error";
			}
		}
		void executeSQL(sqlite3_stmt* statement) {
			if(sqlite3_step(statement) != SQLITE_DONE) {
				const char *sql = sqlite3_sql(statement);
				printf("statement failed. %s SQL = %s\n",sqlite3_errmsg(db),sql);
			}
		}
		void executeSQL(const std::string& sql) const {
			char *zErrMsg = 0;
			int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
			if( rc != SQLITE_OK ){
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				printf("SQL = %s : ",sql.c_str());
				sqlite3_free(zErrMsg);
				throw "SQL Error";
			}
		}
		static void PrintStmt(sqlite3_stmt* stmt) {
			const char* textStmt = sqlite3_sql(stmt);
			printf("Stmt text = %s\n",textStmt);
		}
		const char* ErrorMessage() {
			return sqlite3_errmsg(db);
		}
	};
}

#endif /* defined(__afstuderen_test__Database__) */
