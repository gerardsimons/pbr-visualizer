#pragma once

#include "DataView.h"
#include "ParallelCoordsAxis.h"
#include "ParallelCoordsAxisGroup.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

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
    std::vector<std::string> axesOrder;
    
    //Create functions
    void createAxes();
    
    //Draw helper functions
    void drawAxes();
    void drawLines();
    
    void drawText(char*,int,int,int,float[3],float);
    void drawText(std::string,int,int,float[3],float);
    float* computeColor(size_t lineIndex, size_t totalNrOfLines);
    
    //Properties
    std::vector<ParallelCoordsAxisGroup> axisGroups;
    ParallelCoordsAxis* selectedAxis;
    bool axesAreDirty = false;
    bool linesAreDirty = false;
};

