#pragma once

#include "DataView.h"
#include "ParallelCoordsAxis.h"



class ParallelCoordsView : public RIVDataView
{
public:
    ParallelCoordsView(int x, int y, int width, int height, int paddingX, int paddingY);
	~ParallelCoordsView(void);

    //Implemented virtual functions
    void Draw();
    void ComputeLayout();
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int,int);

private:
    void DrawText(const char*,int,int,int,float[3],float);
    
    //Properties
    std::vector<ParallelCoordsAxis> axes;
};

