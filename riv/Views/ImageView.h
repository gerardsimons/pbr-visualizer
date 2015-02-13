//
//  ImageView.h
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "DataView.h"
#include "../Geometry/Geometry.h"
#include "../Graphics/BMPImage.h"
#include "../Configuration.h"
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
	float imageMagnificationX,imageMagnificationY;

    RIVPoint viewToPixelSpace(int x, int y);
	void clearSelection();
	//In screenspace
	RIVPoint imageStart;
	RIVPoint imageEnd;
	bool isDragging;
	//image size in pixels
	int imageWidth, imageHeight;
    
	Area selection;
    
    const unsigned int bins = 25;
    Histogram2D<float> heatmapOne;
    Histogram2D<float> heatmapTwo;
	
	void drawRenderedImage(EMBREERenderer* renderer,int startX, int startY, int width, int height);
    void computeHeatmap(RIVDataSet<float,ushort>*, Histogram2D<float>& heatmap);
    void drawHeatmap(int startX, Histogram2D<float>& heatmap, Histogram2D<float>& otherHeatmap, float r, float g, float b);
public:
	//Single renderer constructor
	RIVImageView(RIVDataSet<float,ushort>** datasetOne,  EMBREERenderer* rendererOne);
	//Dual renderer constructor
	RIVImageView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo);
	
	void redisplayWindow();
	
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
