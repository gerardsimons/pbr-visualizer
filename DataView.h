#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "DataSet.h"

class RIVDataView
{
protected:
	//properties
	int startX,startY;
	int width,height;
	
	RIVDataSet dataset;

	//constructor destructor
	RIVDataView(int x, int y, int width, int height);
	RIVDataView(void);
public:
	//functions
	void SetData(RIVDataSet newDataSet) { dataset = newDataSet; } ;
    virtual void ComputeLayout() = 0;
	virtual void Draw() = 0; //Strictly virtual, MUST be implemented
};

// your code
#endif

