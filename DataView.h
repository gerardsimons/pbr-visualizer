#include "RIVDataSet.h"

class DataView
{
protected:
	//properties
	int startX,startY;
	int width,height;
	RIVDataSet *dataset;

	//constructor destructor
	DataView(int x, int y, int width, int height);
	virtual ~DataView(void);

	//functions
	void SetData(RIVDataSet*);
};

