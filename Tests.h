//
//  Tests.h
//  afstuderen_test
//
//  Created by Gerard Simons on 08/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef afstuderen_test_Tests_h
#define afstuderen_test_Tests_h

#include "DataController.h"
#include "Database.h"
#include "SQLDataView.h"

#include <sqlite3.h>

void run_tests(DataController* dc) {

	return; //BAIL
	sqlite::DataView* paths = dc->GetPathsView();
	sqlite::ResultSet pathsResult = paths->Select();
	size_t row = 1;
	printf("Custom stmt\n");
	sqlite3_stmt* pathsStmt = paths->SelectStmt();
	sqlite::Database::PrintStmt(pathsStmt);
	while(sqlite3_step(pathsStmt) == SQLITE_ROW) {
		printf("row = %zu\n",row);
//		printf("result = %zu\n",sqlite3_column_int(pathsStmt, 0));
		++row;
	}
	
	sqlite3_stmt* countPathsView = dc->CustomSQLStmt("SELECT COUNT(*) FROM RIV_PATHS");
	while(sqlite3_step(countPathsView) == SQLITE_ROW) {
		printf("COUNT(*) = %d\n",sqlite3_column_int(countPathsView, 0));
	}
	row = 1;
	countPathsView = dc->CustomSQLStmt(paths->GetSelectSQL());
	sqlite::Database::PrintStmt(countPathsView);
	while(sqlite3_step(countPathsView) == SQLITE_ROW) {
//		printf("COUNT(*) = %d\n",sqlite3_column_int(countPathsView, 0));
		++row;
	}
	printf("Rows = %zu\n",row);
	
	printf("************ Custom stmt\n\n");
	printf("ResultSet\n");
	row = 1;
	sqlite::Row rowObj;
	while(pathsResult.GetNext(rowObj)) {
		printf("row = %zu\n",row);
	   ++row;
	}
	printf("************ ResultSet \n");
	
	printf("Tests done.\n");
}

#endif
