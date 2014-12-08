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
	void drawSelectionBoxes();
    void drawText(char*,int,int,int,float[3],float);
    void drawText(std::string,int,int,float[3],float);
//    float* computeColor(size_t lineIndex, size_t totalNrOfLines);
	
	//Two separate color properties, one for the entire path and one for a single ray-intersection
	RIVColorProperty* pathColor;
	RIVColorProperty* rayColor;
    
    //Properties
    std::vector<ParallelCoordsAxisGroup<float,ushort>> axisGroups;
	
	//TODO : template this
    ParallelCoordsAxis<float>* selectedAxis = NULL;
	
	//Determines what graphical primitives should be redrawn
    bool axesAreDirty = true;
    bool linesAreDirty = true;
	bool selectionIsDirty = true;
    
    void clearSelection();
public:
    ParallelCoordsView(RIVDataSet<float,ushort>** dataset, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColor, RIVColorProperty *rayColor, RIVSizeProperty* sizeProperty);
    ParallelCoordsView(RIVDataSet<float,ushort>** dataset, RIVColorProperty *pathColor, RIVColorProperty *rayColor, RIVSizeProperty* sizeProperty);
	~ParallelCoordsView(void);
	
	static int windowHandle;
    
    //Implemented virtual functions prescribed by DataView
    void Draw();
    void Reshape(int width, int height);
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int,int);
	
    //implement virtual functions prescribed by DataSetListener
    virtual void OnDataChanged();
	virtual void OnFiltersChanged();
	
	//Create graphical primitives based on data currently set
	void InitializeGraphics();
	
    static void ReshapeInstance(int width, int height);
    static void DrawInstance();
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    static ParallelCoordsView *instance;
};

