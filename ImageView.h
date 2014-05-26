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
#include "Geometry.h"

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

class RIVImageView : public RIVDataView {
public:
    RIVImageView(std::string filename, int x, int y, int width, int height, int paddingX, int paddingY);
    ~RIVImageView();
    virtual void Draw(); //Override
    virtual void ComputeLayout(); //Override
	virtual bool HandleMouse(int,int,int,int);
	virtual bool HandleMouseMotion(int x, int y);
    virtual void OnFilterChange();
private:
    GLuint imageTexture;
	float imageMagnificationX,imageMagnificationY;

	Point screenToPixelSpace(int,int);
	void clearSelection();
	//In screenspace
	Point imageStart;
	Point imageEnd;
	//image size in pixels
	int imageWidth, imageHeight;
    
	Area selection;
};

#endif /* IMAGEVIEW_H */
