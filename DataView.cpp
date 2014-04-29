#include "DataView.h"

DataView::DataView(int x, int y, int width, int height)
{
	startX = x;
	startY = y;
	this->width = width;
	this->height = height;
}

DataView::~DataView(void)
{
	delete &startX;
	delete &startY;
	delete &width;
	delete &height;
}

void DataView::SetData(RIVDataSet *dataset) {
	this->dataset = dataset;
}
