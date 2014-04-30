#pragma once

#include "DataView.h"

class ParallelCoordsView : public RIVDataView
{
public:
	ParallelCoordsView(int x, int y, int width, int height);
	~ParallelCoordsView(void);
	virtual void Draw();
};

