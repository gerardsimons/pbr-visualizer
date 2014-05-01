#include "DataView.h"

RIVDataView::RIVDataView(int x, int y, int width, int height)
{
	startX = x;
	startY = y;
	this->width = width;
	this->height = height;
}

RIVDataView::RIVDataView(void)
{
	delete &startX;
	delete &startY;
	delete &width;
	delete &height;
}
