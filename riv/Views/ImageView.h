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
#include "devices/device_singleray/embree_renderer.h"

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

class RIVImageView : public RIVDataView, public RIVDataSetListener {
public:
//    RIVImageView(RIVDataSet* dataset,  BMPImage* image, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty* colorProperty,RIVSizeProperty* sizeProperty);
    RIVImageView(int parentWindow, RIVDataSet* dataset, EMBREERenderer* renderer, int x, int y, int width, int height);
    
    static void DrawInstances();
    static void ReshapeInstances(int newWidth,int newHeight);
    static void Mouse(int state,int button,int x,int y);
    static void Motion(int x, int y);
    
    virtual void Draw(); //Override
    virtual void Reshape(int newWidth, int newHeight);
	virtual bool HandleMouse(int,int,int,int);
	virtual bool HandleMouseMotion(int x, int y);
	
	void InitializeGraphics();
	
	void OnFiltersChanged();
	void OnDataChanged();
	
	int windowHandle;
private:
	static std::vector<RIVImageView*> instances;
	
	EMBREERenderer* renderer = NULL;
	
	float imageMagnificationX,imageMagnificationY;

    RIVPoint viewToPixelSpace(int x, int y);
	void clearSelection();
	//In screenspace
	RIVPoint imageStart;
	RIVPoint imageEnd;
	//image size in pixels
	int imageWidth, imageHeight;
    
	Area selection;
};

#endif /* IMAGEVIEW_H */
