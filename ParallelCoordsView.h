#pragma once

#include "DataView.h"
#include "ParallelCoordsAxis.h"

class ParallelCoordsView : public RIVDataView
{
public:
    ParallelCoordsView(int x, int y, int width, int height);
	~ParallelCoordsView(void);

    //Implemented virtual functions
    void Draw();
    void ComputeLayout();
private:
    void DrawText(const char*,int,int,int,float[3],float);
    
    //Properties
    vector<ParallelCoordsAxis> axes;
};

