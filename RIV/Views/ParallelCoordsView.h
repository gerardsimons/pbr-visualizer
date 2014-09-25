#pragma once

#include "DataView.h"
#include "ParallelCoordsAxis.h"
#include "ParallelCoordsAxisGroup.h"
#include "../Graphics/ColorProperty.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <typeinfo>

class ParallelCoordsView : public RIVDataView, public RIVDataSetListener
{
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
    ParallelCoordsAxis* selectedAxis = NULL;
    bool axesAreDirty = true;
    bool linesAreDirty = true;
    
    void clearSelection();
public:
    ParallelCoordsView(RIVDataSet* dataset);
    ParallelCoordsView(RIVDataSet* dataset, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty);
    ParallelCoordsView(RIVDataSet* dataset, RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty);
	~ParallelCoordsView(void);
	
	static int windowHandle;
    
    //Implemented virtual functions prescribed by DataView
    void Draw();
    void Reshape(int width, int height);
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int,int);
    //implement virtual functions prescribed by DataSetListener
    virtual void OnDataSetChanged();
	//Create graphical primitives based on data currently set
	void InitializeGraphics();
	
    static void ReshapeInstance(int width, int height);
    static void DrawInstance();
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    static ParallelCoordsView *instance;
};

