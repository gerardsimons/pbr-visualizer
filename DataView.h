#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "DataSet.h"

class RIVDataView
{
protected:
	RIVDataSet *dataset;

	bool needsRedraw;

	//constructor destructor
	RIVDataView(int x, int y, int _width, int _height, int _paddingX, int _paddingY) {
        startX = x;
        startY = y;
        width = _width;
        height = _height;
        paddingX = _paddingX;
        paddingY = _paddingY;
    };
    
	~RIVDataView(void) { /* Delete some stuff I guess */ };

	bool containsPoint(int x, int y) {
        int endX = startX + width;
        int endY = startY + height;
        return(x > startX && x < endX && y > startY && y < endY);
    }
public:
	//properties
	int startX,startY;
	int width,height;
	int paddingX,paddingY;
	//functions
	void SetData(RIVDataSet *newDataSet) { dataset = newDataSet; needsRedraw = true;}
	void Invalidate() { needsRedraw = true; }
	//must implement
    virtual void ComputeLayout() = 0;
	virtual void Draw() = 0; 
	virtual bool HandleMouse(int button, int state, int x, int y) = 0;
	virtual bool HandleMouseMotion(int x, int y) = 0;
};

// your code
#endif

