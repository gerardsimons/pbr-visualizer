//
//  ImageView.h
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "../Grid.h"
#include "DataView.h"
#include "../Geometry/Geometry.h"
#include "../Graphics/BMPImage.h"
#include "../Configuration.h"
#include "../Data/Filter.h"
#include "devices/device_singleray/embree_renderer.h"

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

#include <vector>

class RIVImageView : public RIVDataView, public RIVDataSetListener {
private:
    static RIVImageView* instance;
	
	EMBREERenderer* rendererOne;
	EMBREERenderer* rendererTwo = NULL;
	
//	std::vector<EMBREERenderer*> renderers;
    int imagePadding = 5;

	void clearSelection();
	bool isDragging;
	//image size in pixels
	int imageWidth, imageHeight;

    //TODO: I think these histograms belong in the data controller, or that is where they should originate from
    bool showHeatmap = true;
    const unsigned int bins = 25;
    Histogram2D<float>* heatmapOne = NULL;
    Histogram2D<float>* heatmapTwo = NULL;

    bool showFillArea = false;
    unsigned int gridSize = 100;
    riv::RowFilter* pixelFilterOne = NULL;
    riv::RowFilter* pixelFilterTwo = NULL;
    Grid* paintGridOne;
    Grid* paintGridTwo = NULL;
    Grid* interactingGrid = NULL;
	
	void drawRenderedImage(EMBREERenderer* renderer,int startX, int startY, int width, int height);
    void computeHeatmap(RIVDataSet<float,ushort>*, Histogram2D<float>*& heatmap);
    void drawGrid(float startX, Grid* paintGrid);
    void drawHeatmap(int startX, Histogram2D<float>* heatmap, float r, float g, float b);
    void filterImage(RIVDataSet<float,ushort>* dataset, Grid* activeGrid, riv::RowFilter* previousFilter);
    void toGridSpace(int xIn, int yIn, Grid*& outGrid, int& gridX, int& gridY);
public:
    void redisplayWindow();
	//Single renderer constructor
	RIVImageView(RIVDataSet<float,ushort>** datasetOne,  EMBREERenderer* rendererOne);
	//Dual renderer constructor
	RIVImageView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo);
	
    Histogram2D<float>* GetHeatmapOne();
    Histogram2D<float>* GetHeatmapTwo();
    void ToggleShowHeatmap();
	
	static void DrawInstance();
	static void ReshapeInstance(int,int);
	static void Mouse(int state,int button,int x,int y);
	static void Motion(int x, int y);
	
	virtual void Draw(); //Override
	void Reshape(int newWidth, int newHeight);
	virtual bool HandleMouse(int,int,int,int);
	virtual bool HandleMouseMotion(int x, int y);
	
	//Change the currently active renderer if necessary, return true if a change was necessary
//	bool SetActiveRenderer(size_t i);
	
	void OnFiltersChanged(RIVDataSet<float,ushort>* dataset);
	void OnDataChanged(RIVDataSet<float,ushort>* dataset);
	
	static int windowHandle;
};

#endif /* IMAGEVIEW_H */
