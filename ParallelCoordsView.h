#pragma once

#include "DataView.h"
#include "ParallelCoordsAxis.h"
#include "ParallelCoordsAxisGroup.h"
#include "ColorProperty.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class ParallelCoordsView : public RIVDataView, public RIVDataSetListener
{
public:
    ParallelCoordsView(int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty);
    ParallelCoordsView(RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty);
	~ParallelCoordsView(void);

    //Implemented virtual functions prescribed by DataView
    void Draw();
    void ComputeLayout(float startX, float startY, float width, float height, float paddingX, float paddingY);
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int,int);
    //implement virtual functions prescribed by DataSetListener
    virtual void OnDataSetChanged();
private:
    std::vector<std::string> axesOrder;
    //Create functions
    void createAxes();
    
    //Draw helper functions
    void drawAxes();
    void drawLines();
    void drawText(char*,int,int,int,float[3],float);
    void drawText(std::string,int,int,float[3],float);
//    float* computeColor(size_t lineIndex, size_t totalNrOfLines);
    
    //Properties
    std::vector<ParallelCoordsAxisGroup> axisGroups;
    ParallelCoordsAxis* selectedAxis;
    bool axesAreDirty = false;
    bool linesAreDirty = false;
};

