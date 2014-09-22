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

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif


class RIVImageView : public RIVDataView, public RIVDataSetListener {
public:
    RIVImageView(RIVDataSet* dataset,  BMPImage* image, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty* colorProperty,RIVSizeProperty* sizeProperty);
    RIVImageView(RIVDataSet* dataset,  BMPImage* image, RIVColorProperty* color, RIVSizeProperty *size);
//    RIVImageView(const BMPImage& image,RIVColorProperty* colorProperty,RIVSizeProperty* sizeProperty);
    ~RIVImageView();
    
    static void DrawInstance();
    static void ReshapeInstance(int,int);
    static void Mouse(int state,int button,int x,int y);
    static void Motion(int x, int y);
    
    virtual void Draw(); //Override
    void Reshape(int newWidth, int newHeight);
	virtual bool HandleMouse(int,int,int,int);
	virtual bool HandleMouseMotion(int x, int y);
    virtual void OnDataSetChanged();
	
	void InitializeGraphics();

private:
    static RIVImageView* instance;
    
    GLuint imageTexture;
	float imageMagnificationX,imageMagnificationY;

	void createTextureImage();
    void createTextureImage(BMPImage* image);
    Point viewToPixelSpace(int x, int y);
	void clearSelection();
    BMPImage* renderedImage;
	//In screenspace
	Point imageStart;
	Point imageEnd;
	//image size in pixels
	int imageWidth, imageHeight;
    
	Area selection;
};

#endif /* IMAGEVIEW_H */
