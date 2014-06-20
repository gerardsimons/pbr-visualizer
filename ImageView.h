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
#include "BMPImage.h"

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

class RIVImageView : public RIVDataView, public RIVDataSetListener {
public:
    RIVImageView(const BMPImage& image, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty* colorProperty,RIVSizeProperty* sizeProperty);
//    RIVImageView(const BMPImage& image,RIVColorProperty* colorProperty,RIVSizeProperty* sizeProperty);
    ~RIVImageView();
    virtual void Draw(); //Override
    void ComputeLayout(float startX, float startY, float width, float height, float paddingX, float paddingY); //Override
	virtual bool HandleMouse(int,int,int,int);
	virtual bool HandleMouseMotion(int x, int y);
    virtual void OnDataSetChanged();
    void createTextureImage();
    void createTextureImage(const BMPImage& image);
private:
    GLuint imageTexture;
	float imageMagnificationX,imageMagnificationY;
//    void createTextureImage();
    Point viewToPixelSpace(int x, int y);
	void clearSelection();
    BMPImage renderedImage;
	//In screenspace
	Point imageStart;
	Point imageEnd;
	//image size in pixels
	int imageWidth, imageHeight;
    
	Area selection;
};

#endif /* IMAGEVIEW_H */
