//
//  DataController.h
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__DataController__
#define __afstuderen_test__DataController__

#include "Database.h"
#include "SQLDataView.h"
#include "Reference.h"

/**
 *	This class simulates a stateful in memory SQL database. Any selectors added are appended to the query and used to create the temporary view reflecting the state of the application
 */
class DataController {
private:
	sqlite::Database db;
	
	sqlite::DataView* pathsView = NULL;
	sqlite::DataView* isectsView = NULL;

	std::vector<sqlite::DataView*> allViews;
	
	
	//These are the main tables, they do not change and act as a source for any reinsertions that may be necessary upon the views
	const std::string pathsTable = "PATHS";
	const std::string intersectionsTable = "INTERSECTIONS";
	
	//Create temporary views for each table according to the selectors
	//TODO : Generalize: get all tables in database create views for each one and link together in arbitrary fashion
	void createViews();
	
public:
	//TODO: Do not create views immediately but only after some filtering happens
	DataController(const sqlite::Database& memoryDB) : db(memoryDB) {
		//Load the data in a temp view which is kept in memory
		createViews();
	}
	std::vector<sqlite::DataView*> GetAllViews() {
		return allViews;
	}
	//This supercedes the temporary views
	sqlite::Statement CustomSQLStmt(const std::string& sql) {
		return db.PrepareStatement(sql);
	}
	sqlite::DataView* GetPathsView() {
		return pathsView;
	}
	sqlite::DataView* GetIsectsView() {
		return isectsView;
	}
	std::vector<sqlite::DataView*> GetViews() {
		return allViews;
	}
	//Fetch the latest error
	std::string GetError() {
		return std::string(db.ErrorMessage());
	}
};

#endif /* defined(__afstuderen_test__DataController__) */
