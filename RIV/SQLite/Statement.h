//
//  Statement.h
//  afstuderen_test
//
//  Created by Gerard Simons on 09/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__Statement__
#define __afstuderen_test__Statement__

#include <sqlite3.h>
#include <stdio.h>
#include <string>
#include <vector>


#include "helper.h"

//A thin wrapper class for the sqlite3_stmt
namespace sqlite {
	class Statement {
	private:
		size_t stepCount = 0;
	public:
		sqlite3_stmt* stmt = NULL;
		Statement() { };
		Statement(sqlite3_stmt* stmt) {
			this->stmt = stmt;
		}
		bool Reset() {
			int rc = sqlite3_reset(stmt);
			if(rc != SQLITE_OK) {
				return false;
			}
//			stepCount = 0;
			return true;
		}
		bool Step() {
			int rc = sqlite3_step(stmt);
			if(rc == SQLITE_ROW) {
				return true;
			}
			else if(rc == SQLITE_DONE) {
				return false;
			}
			else {
				char err[50];
				printf("ERROR = %d\n",rc);
				return false;
			}
//			++stepCount;
		}
		int GetInt(int index) {
			return sqlite3_column_int(stmt, index);
		}
		float GetFloat(int index) {
			return sqlite3_column_double(stmt,index);
		}
		void Print() {
			int column_count = sqlite3_column_count(stmt);
			std::vector<int> types;
			std::vector<std::string> names;
			int colWidth = 10;
			Step();
			for(int i = 0 ; i < column_count ; i++) {
				std::string name = sqlite3_column_name(stmt, i);
				types.push_back(sqlite3_column_type(stmt, i));
				right_pad(name, ' ', colWidth);
				std::cout << name;
//				names.push_back();
			}
			Reset();
			std::cout << std::endl;
			std::string delimiter;
			for(int i = 0 ; i < column_count * colWidth ; i++) {
				delimiter += '-';
			}
			std::cout << delimiter << std::endl;
			while(Step()) {
				for(int i = 0 ; i < column_count ; i++) {
					int type = sqlite3_column_type(stmt, i);
					std::string valueString;
					if(type == SQLITE_INTEGER) {
						int value = sqlite3_column_int(stmt, i);
						valueString = std::to_string(value);
					}
					else if(type == SQLITE_FLOAT) {
						float value = sqlite3_column_double(stmt, i);
						valueString = std::to_string(value);
					}
					right_pad(valueString,' ',colWidth);
					std::cout << valueString;
				}
				std::cout << std::endl;
			}
		   Reset();
		}
	};
}

#endif /* defined(__afstuderen_test__Statement__) */
