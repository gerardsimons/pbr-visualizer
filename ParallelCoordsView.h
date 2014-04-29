#pragma once
#include "view.h"
class ParallelCoordsView : public View
{
public:
	ParallelCoordsView(int x, int y, int width, int height);
	~ParallelCoordsView(void);
	virtual void Draw();
};

