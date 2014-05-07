#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "DataSet.h"

class RIVDataView
{
protected:
	RIVDataSet *dataset;

	bool needsRedraw;

	//constructor destructor
	RIVDataView(int x, int y, int width, int height, int paddingX, int paddingY);
	RIVDataView(void);

	bool containsPoint(int x, int y);
public:
	//properties
	int startX,startY;
	int width,height;
	int paddingX,paddingY;
	//functions
	void SetData(RIVDataSet *newDataSet);
	void Invalidate() { needsRedraw = true; }
	//must implement
    virtual void ComputeLayout() = 0;
	virtual void Draw() = 0; 
	virtual bool HandleMouse(int button, int state, int x, int y) = 0;
	virtual bool HandleMouseMotion(int x, int y) = 0;
};

// your code
#endif

