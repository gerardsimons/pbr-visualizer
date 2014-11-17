//
//  DataController.h
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#ifndef __embree__DataController__
#define __embree__DataController__

#include "Data/DataSet.h"

class DataController {
private:
	RIVTable* pathTable;
	RIVTable* isectsTable;
	RIVDataSet* dataset;
	
	//Generate the dataset; create tables records etc.
	void createDataSet();
public:
	DataController(RIVDataSet* dataset);
	
	void AddPath();
};

#endif /* defined(__embree__DataController__) */
