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
#include "Selector.h"

/**
 *	This class simulates a stateful in memory SQL database. Any selectors added are appended to the query and used to create the temporary view reflecting the state of the application
 */
class DataController {
private:
	sqlite::Database db;
	
	sqlite::DataView* pathsView = NULL;
	sqlite::DataView* isectsView = NULL;
	
	std::vector<sqlite::DataView*> allViews;
	
	std::string mainTableName = "PATHS";
	
	std::vector<sqlite::Selector*> selectors; //The select operations applied upon the SQL database
	
	const std::string pathsViewName = "RIV_PATHS";
	const std::string isectsViewName = "RIV_INTERSECTIONS";
	
	//Create temporary views for each table according to the selectors
	//TODO : Generalize: get all tables in database create views for each one and link together in arbitrary fashion
	void createViews() {
		//Remove old views (if any)
		if(pathsView) {
			delete pathsView;
		}
		if(isectsView) {
			delete isectsView;
		}
		allViews.clear();
		
		std::string selectSQL = "SELECT * FROM " + mainTableName;
		
		//Possibly add any selectors that may exist
		if(selectors.size() > 0) {
			for(size_t i = 0 ; i < selectors.size() - 1 ; ++i){
				selectSQL += selectors[i]->generateSQL() + " AND ";
			}
			selectSQL += selectors[selectors.size() - 1]->generateSQL();
		}
		
		pathsView = new sqlite::DataView(&db,pathsViewName,selectSQL);
		isectsView = new sqlite::DataView(&db,isectsViewName,"SELECT intersections.* from intersections," + pathsViewName + " WHERE " + pathsViewName + ".id = intersections.pid");
		
		allViews = {pathsView,isectsView};
		
		printf("DATA VIEWS CREATED!\n");
		
		//<For each table>
		//<Iterate over selectors and generate the SQL>
		//<SQL operates on the original data or on the view if it already exists and if it is known the exisitng views will contain all of the data possibly requested
		//<Execute the SQL
	}
	
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
	sqlite3_stmt* CustomSQLStmt(const std::string& sql) {
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
	void AddSelector(sqlite::Selector* selector) {
		if(selector != NULL) {
			selectors.push_back(selector);
		}
		printf("WARNING : supplied selector was NULL");
	}
};

#endif /* defined(__afstuderen_test__DataController__) */
