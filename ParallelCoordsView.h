#pragma once

#include "DataView.h"

class ParallelCoordsView : public RIVDataView
{
public:
	ParallelCoordsView(int x, int y, int width, int height);
	~ParallelCoordsView(void);
	void Draw();
	void DrawText(const char*,int,int,int,float[3],float);
};

