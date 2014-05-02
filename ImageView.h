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

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

typedef struct Point {
	int x;
	int y;
} Point;

//Used to highlight the pixels
typedef struct SelectionBox {
	Point start;
	Point end;
} SelectionBox;

class RIVImageView : public RIVDataView {
public:
    RIVImageView(char* filename, int x, int y, int width, int height, int paddingX, int paddingY);
    ~RIVImageView();
    void Draw(); //Override
    void ComputeLayout(); //Override
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int x, int y);
private:
    GLuint imageTexture;
	float imageMagnificationX,imageMagnificationY;
	bool isDragging;
	Point screenToPixelSpace(int,int);
	//In screenspace
	Point imageStart;
	Point imageEnd;
	//image size in pixels
	int imageWidth, imageHeight;
	SelectionBox selection;
};

#endif /* IMAGEVIEW_H */
