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
#include "Expression.h"
#include "Statement.h"

#include <sqlite3.h>

DataController *dc = NULL;


void test_select() {
	sqlite::DataView* paths = dc->GetPathsView();
	sqlite::DataView* isects = dc->GetIsectsView();
	
	sqlite::EqualityExpression<size_t>* expr = new sqlite::EqualityExpression<size_t>("pid",0);
	//Delete all else
	sqlite::NotExpression* deleteExpr = new sqlite::NotExpression(expr);
	isects->Filter(deleteExpr);
	
	sqlite::Statement pathsStatement = paths->SelectStmt();
	
	std::string latestError = dc->GetError();
	
	
	//	latestError = dc->GetError();
	
	sqlite::Statement isectStatement = isects->SelectStmt();
	isectStatement.Print();
	pathsStatement.Print();
	
}

void test_filter() {
	std::string bounceSQL = "BOUNCE_NR < 1 OR BOUNCE_NR > 2";
	sqlite::CustomSingularExpression* expr = new sqlite::CustomSingularExpression("BOUNCE_NR",bounceSQL);
	sqlite::DataView* view = dc->GetIsectsView();
	view->StartModifying();
	view->Filter(expr);
	view->StopModifying();
}


void run_tests(DataController* dataController) {
	
	dc = dataController;
	
//	test_filter();
}

#endif
