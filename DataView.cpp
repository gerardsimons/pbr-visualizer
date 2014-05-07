#include "DataView.h"

RIVDataView::RIVDataView(int x, int y, int width, int height, int paddingX, int paddingY)
{
	startX = x;
	startY = y;
	this->width = width;
	this->height = height;
	this->paddingX = paddingX;
	this->paddingY = paddingY;

	needsRedraw = true;
}

RIVDataView::RIVDataView(void)
{
	delete &startX;
	delete &startY;
	delete &width;
	delete &height;
}

void RIVDataView::SetData(RIVDataSet *dataset) {
	this->dataset = dataset;
}

bool RIVDataView::containsPoint(int x, int y) {
	//TODO: Deal with padding/margins?
	bool xInRange = x > startX && x < startX + width;
	bool yInRange = y > startY && y < startY + height;

	return xInRange && yInRange;
}