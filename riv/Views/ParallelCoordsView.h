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
    std::vector<ParallelCoordsAxisGroup> axisGroups;
    ParallelCoordsAxis* selectedAxis = NULL;
	
	//Determines what graphical primitives should be redrawn
    bool axesAreDirty = true;
    bool linesAreDirty = true;
	bool selectionIsDirty = true;
    
    void clearSelection();
public:
	ParallelCoordsView(int parentWindow, RIVDataSet* dataset, RIVColorProperty *pathColor, RIVColorProperty *rayColor, RIVSizeProperty* sizeProperty,int x, int y, int width, int height);
//    ParallelCoordsView(RIVDataSet* dataset, RIVColorProperty *pathColor, RIVColorProperty *rayColor, RIVSizeProperty* sizeProperty);
	~ParallelCoordsView(void);
	
	int windowHandle;
    
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
	
    static void ReshapeInstances(int width, int height);
    static void DrawInstances();
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
	static std::vector<ParallelCoordsView*> instances;
};

