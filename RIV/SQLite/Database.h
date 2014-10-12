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
#include "Expression.h"
#include "Statement.h"

namespace sqlite {
	class Database {
	private:
			sqlite3* db = NULL;
	public:
		//Open database from filename
		Database(char* fileName);
		Database(sqlite3* db_);
		
		void Delete(const std::string& tableName, Expression* expr);
		void DropTrigger(const std::string triggername);
		
		Statement SelectColumnsFromTable(const std::vector<std::string> columns, const std::string& table) const;
		Statement PrepareStatement(const std::string& sql) const;
		void executeSQL(char const* sql) const;
		void executeSQL(const std::string& sql) const;
		static void PrintStmt(sqlite3_stmt* stmt);
		const char* ErrorMessage();
	};
}

#endif /* defined(__afstuderen_test__Database__) */
