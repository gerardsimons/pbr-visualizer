//
//  DataController.cpp
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "DataController.h"
#include <string>

using namespace sqlite;

void DataController::createViews() {
	//Remove old views (if any)
	if(pathsView) {
		delete pathsView;
	}
	if(isectsView) {
		delete isectsView;
	}
	allViews.clear();
	

	
	//isects is linked to paths view

	//Precreate the tables
	std::string precreate =
	"CREATE TABLE RIV_PATHS(" \
						   "ID	UNSIGNED INTEGER PRIMARY KEY," \
						   "IMAGE_X UNSIGNED INTEGER    NOT NULL," \
						   "IMAGE_Y UNSIGNED INTEGER     NOT NULL," \
						   "LENS_U REAL NOT NULL," \
						   "LENS_V REAL NOT NULL," \
						   "TIMESTAMP REAL NOT NULL," \
						   "THROUGHPUT_ONE REAL NOT NULL," \
						   "THROUGHPUT_TWO REAL NOT NULL," \
						   "THROUGHPUT_THREE REAL NOT NULL," \
						   "RADIANCE_R REAL NOT NULL," \
						   "RADIANCE_G REAL NOT NULL," \
						   "RADIANCE_B REAL NOT NULL);" \

	"CREATE TABLE RIV_INTERSECTIONS(" \
							   "PID UNSIGNED BIG INT NOT NULL," \
							   "BOUNCE_NR		 TINYINT NOT NULL," \
							   "POS_X REAL NOT NULL," \
							   "POS_Y           REAL     NOT NULL," \
							   "POS_Z           REAL	NOT NULL," \
							   "PRIMITIVE_ID    TINYINT NOT NULL," \
							   "SHAPE_ID		 TINYINT NOT NULL," \
							   "OBJECT_ID		 TINYINT NOT NULL," \
							   "SPECTRUM_R        REAL NOT NULL," \
							   "SPECTRUM_G        REAL NOT NULL," \
							   "SPECTRUM_B        REAL NOT NULL," \
							   "INTERACTION_TYPE	TINYINT NOT NULL," \
							   "LIGHT_ID			TINYINT NOT NULL," \
							   "FOREIGN KEY(PID) REFERENCES RIV_PATHS(ID)" \
							   ");";

	db.executeSQL(precreate);
	
	//TODO generate automatically from reference?
	std::string asSQL = " SELECT intersections.* FROM intersections,paths WHERE pid = id";
	isectsView = new DataView(&db,intersectionsTable,asSQL);
	std::string selectSQL = "SELECT * FROM " + pathsTable;
	pathsView = new sqlite::DataView(&db,pathsTable,selectSQL);
	
	Reference *reference = new Reference("pid","id",isectsView,pathsView);
	Reference *reverseReference = reference->Reverse();
	isectsView->SetReference(reference);
	pathsView->SetReference(reverseReference);
	
	

	std::string pathssDeleteTriggerName = "PATHS_DELETE";
	std::string isectsDeleteTriggerName = "ISECTS_DELETE";
	
	std::string pathsInsertTriggerName = "PATHS_INSERT";
	std::string isectsInsertTriggerName = "ISECTS_INSERT";
	
	//Drop any previously created triggers
	db.DropTrigger(pathssDeleteTriggerName);
	db.DropTrigger(isectsDeleteTriggerName);
	db.DropTrigger(pathsInsertTriggerName);
	db.DropTrigger(isectsInsertTriggerName);

	//NOTE : BELOW SHOULD NOT BE USED IN CONJUNCTION WITH THE CASCADE PRESENT
	std::string pathsToIsectsDelete =
	"CREATE TRIGGER " + pathssDeleteTriggerName +
	" BEFORE DELETE ON " + pathsView->GetName() +
	" FOR EACH ROW " \
	"BEGIN " \
	"DELETE FROM " + isectsView->GetName() + " WHERE pid = OLD.id; " \
	"END";

	//Slightly more complicated, only delete if all intersections of a given path have been removed
	std::string isectToPathsDelete =
	"CREATE TRIGGER " +  isectsDeleteTriggerName + " after delete ON " + isectsView->GetName() + " " \

	"BEGIN " \
	"DELETE FROM RIV_PATHS WHERE id = OLD.pid ; " \
	"END ";
	
	std::string pathsInsertTrigger =
	"CREATE TRIGGER " + pathsInsertTriggerName + " " \
	"AFTER INSERT ON " + pathsView->GetName() +
	" FOR EACH ROW " \
	"BEGIN " \
	"INSERT INTO " + isectsView->GetName() + " SELECT * FROM " + intersectionsTable + " WHERE " + intersectionsTable + ".pid = new.id AND NOT EXISTS(SELECT 1 FROM " + isectsView->GetName() + " WHERE pid = NEW.id); " \
	"END";
	
	std::string isectsInsertTrigger =
	"CREATE TRIGGER " + isectsInsertTriggerName +
	" BEFORE INSERT ON " + isectsView->GetName() +
	" FOR EACH ROW " \
	"BEGIN "
		"INSERT INTO " + pathsView->GetName() + " SELECT * FROM " + pathsTable + " WHERE " + pathsTable + ".id = new.pid AND NOT EXISTS(SELECT 1 FROM " + pathsView->GetName() + " WHERE id = NEW.pid); " \
	"END";
//
//	db.executeSQL(pathsToIsectsDelete);
//	db.executeSQL(isectToPathsDelete);
//	db.executeSQL(pathsInsertTrigger);
//	db.executeSQL(isectsInsertTrigger);
	
	allViews = {pathsView,isectsView};
	
	printf("DATA VIEWS CREATED!\n");
	
	//<For each table>
	//<Iterate over selectors and generate the SQL>
	//<SQL operates on the original data or on the view if it already exists and if it is known the exisitng views will contain all of the data possibly requested
	//<Execute the SQL
}