#pragma once

#include "DataView.h"
#include "SliderView.h"
#include "ParallelCoordsAxis.h"
#include "ParallelCoordsAxisGroup.h"
#include "../Graphics/ColorProperty.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <typeinfo>

enum INTERACTION_STATE {
	IDLE,
	MOUSE_DOWN_AXIS,
	MOUSE_DOWN_SELECTION,
	MOUSE_DOWN_AXIS_EXTRA,
	CREATE_SELECTION_BOX,
	DRAG_AXIS,
	DRAG_SELECTION,
};


class ParallelCoordsView : public RIVDataView, public RIVDataSetListener
{
private:
    std::vector<std::string> axesOrder;
	
	RIVSliderView* sliderView;
	
	bool useSaturation = false;
	
	bool drawDataSetOne = true;
	bool drawDataSetTwo = true;
	
	int axisWidth = 20;
	
	HistogramSet<float,ushort>* distributionsOne;
	HistogramSet<float,ushort>* distributionsTwo = NULL;
    
    RIVColorProperty* rayColorEnergyOne;
    RIVColorProperty* pathColorEnergyOne;
    
    RIVColorProperty* rayColorEnergyTwo = NULL;
    RIVColorProperty* pathColorEnergyTwo = NULL;
    
    //Properties
    std::vector<ParallelCoordsAxisGroup<float,ushort>> axisGroups;
	
	//TODO : template this
	ParallelCoordsAxisInterface* selectedAxis = NULL;
	RIVRectangle* selection = NULL;
	
	INTERACTION_STATE interactionState = IDLE;
	
	int mouseDownX;
	int mouseDownY;
	
	int axisUpdateY;
	int axisUpdateX;
	int axisOriginX;
	
	int dragStartSensitivity = 5;
	int updateSensitivity = 10;
	
	float lineOpacity = 0.1F;
	float lineOpacityIncrement = 0.01F;
	
	//Declares both where dragging the selection box originated and the last known point
	int dragBoxLastY;
	bool selectionBoxChanged = false;
	
    void clearSelection();
	//Create functions
	void createAxes();
	void createAxisPoints();
	void createAxisPoints(int datasetId, RIVDataSet<float,ushort>* dataset);

	void createAxisDensities(int datasetId, RIVDataSet<float,ushort>* dataset);
	//Draw helper functions
	void drawAxes();
	//Draw scale indicators and selection glyphs
	void drawAxesExtras();
	//Draws the lines of the parallel coordinates plot for the given dataset, note that it also 
	void drawLines(int datasetNumber, RIVDataSet<float,ushort>* dataset, RIVColorProperty* pathColors, RIVColorProperty* rayColors);
	void drawSelectionBoxes();
	void swapAxes(ParallelCoordsAxisGroup<float,ushort>* axisGroup, const std::string& swapAxisOne, const std::string& swapAxisTwo);

	void filterData();
public:
	void redisplayWindow();
	void createAxisDensities();
    ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColor, RIVColorProperty *rayColor, RIVSliderView* sliderView);
    ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, RIVColorProperty *pathColor, RIVColorProperty *rayColor,RIVSliderView* sliderView);
	//Constructors for double renderers
	ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColor, RIVColorProperty *rayColor,RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo,RIVSliderView* sliderView);
	ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, RIVColorProperty *pathColor, RIVColorProperty *rayColor, RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo,RIVSliderView* sliderView);

	~ParallelCoordsView();
	
	static int windowHandle;
    
    //Helper functions to create color property for a given dataset
    RIVColorRGBProperty<float>* createPathEnergyColorProperty(RIVDataSet<float,ushort>* dataset);
    RIVColorRGBProperty<float>* createRayEnergyColorProperty(RIVDataSet<float,ushort>* dataset);
    
    //Implemented virtual functions prescribed by DataView
    void Draw();
    void Reshape(int width, int height);
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int,int);
	
	void ToggleDrawDataSetOne();
	void ToggleDrawDataSetTwo();
	void ToggleSaturationMode();
	
    //implement virtual functions prescribed by DataSetListener
    virtual void OnDataChanged(RIVDataSet<float,ushort>* source);
	virtual void OnFiltersChanged(RIVDataSet<float,ushort>* dataset);
	
	bool DecreaseLineOpacity();
	bool IncreaseLineOpacity();

	
	//Create graphical primitives based on data currently set
	void InitializeGraphics();
	
    static void ReshapeInstance(int width, int height);
    static void DrawInstance();
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    static ParallelCoordsView *instance;
};

