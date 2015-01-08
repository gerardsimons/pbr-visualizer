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
	
	//Two separate color properties, one for the entire path and one for a single ray-intersection and for each renderer separate
	RIVColorProperty* pathColorOne = NULL;
	RIVColorProperty* rayColorOne = NULL;
	RIVColorProperty* pathColorTwo = NULL;
	RIVColorProperty* rayColorTwo = NULL;
	
	bool drawDataSetOne = true;
	bool drawDataSetTwo = true;
	
	HistogramSet<float,ushort>* distributionsOne;
	HistogramSet<float,ushort>* distributionsTwo = NULL;
    
    //Properties
    std::vector<ParallelCoordsAxisGroup<float,ushort>> axisGroups;
	
	//TODO : template this
	std::string selectedAxis;
	Area* selection = NULL;
	
	//Determines what graphical primitives should be redrawn
    bool axesAreDirty = true;
    bool linesAreDirty = true;
	bool selectionIsDirty = true;
    
    void clearSelection();
	//Create functions
	void createAxes();
	void createAxisDensities();
	//Draw helper functions
	void drawDensities();
	void drawAxes();
	void drawLines(int datasetId, RIVDataSet<float,ushort>* dataset, RIVColorProperty* pathColors, RIVColorProperty* rayColors);
	void drawSelectionBoxes();
//	void drawText(char*,int,int,int,float[3],float);
//	void drawText(std::string,int,int,float[3],float);
	
	void redisplayWindow();
public:
    ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColor, RIVColorProperty *rayColor);
    ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, RIVColorProperty *pathColor, RIVColorProperty *rayColor);
	//Constructors for double renderers
	ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColor, RIVColorProperty *rayColor,RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo);
	ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, RIVColorProperty *pathColor, RIVColorProperty *rayColor, RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo);

	~ParallelCoordsView();
	
	static int windowHandle;
    
    //Implemented virtual functions prescribed by DataView
    void Draw();
    void Reshape(int width, int height);
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int,int);
	
	void toggleDrawDataSetOne();
	void toggleDrawDataSetTwo();
	
    //implement virtual functions prescribed by DataSetListener
    virtual void OnDataChanged(RIVDataSet<float,ushort>* source);
	virtual void OnFiltersChanged();
	
	//Create graphical primitives based on data currently set
	void InitializeGraphics();
	
    static void ReshapeInstance(int width, int height);
    static void DrawInstance();
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    static ParallelCoordsView *instance;
};

