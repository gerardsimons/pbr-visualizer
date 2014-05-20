#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "DataSet.h"
#include "Geometry.h"

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
    
    void ToViewSpaceCoordinates(int* x, int* y) {
        (*x) -= startX;
        (*y) -= startY;
    };

    //Should be converted to view space coordinates!
	bool containsPoint(int x, int y) {
        return(x > 0 && x < width && y > 0 && y < height);
    }
public:
	//properties
	int startX,startY;
	int width,height;
	int paddingX,paddingY;
    bool isDragging;
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

