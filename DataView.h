#include "DataSet.h"

class RIVDataView
{
protected:
	//properties
	int startX,startY;
	int width,height;
	RIVDataSet *dataset;

	//constructor destructor
	RIVDataView(int x, int y, int width, int height);
	RIVDataView(void);
public:
	//functions
	virtual void SetData(RIVDataSet*);
};

