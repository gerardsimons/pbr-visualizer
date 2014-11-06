//
//  RenderView.h
//  afstuderen_test
//
//  Created by Gerard Simons on 20/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__RenderView__
#define __afstuderen_test__RenderView__

#include <stdio.h>
#include "DataView.h"
#include "../../pbrt-v2/src/film/image.h"

class RIVRenderView : public RIVDataView, public RIVDataSetListener{
private:
	//Stores the computed image from all the rays that are not filtered
	std::vector<std::vector<Color>> renderedImage;
	
	float pixelWidth,pixelHeight = 0;
	
	ImageFilm* image = NULL;
	//Draw the current state of the PBRT image film to the screen
	void drawImageFilm();
public:
	static RIVRenderView* instance;
	static int windowHandle; //The glut sub window it draws to
	
	RIVRenderView(RIVDataSet* dataset);
	
	void OnDataSetChanged();
	//Create the PBRT image film from the selected rays
	void CreateImageFilm();
	void SaveImage();
	
	void Draw();
	void Reshape(int width, int height);
	bool HandleMouse(int,int,int,int);
	bool HandleMouseMotion(int,int);
	
	//IO functions
	static void DrawInstance(); //Override
	static void ReshapeInstance(int,int);
	static void Mouse(int button, int state, int x, int y);
	static void Motion(int x, int y);
};

#endif /* defined(__afstuderen_test__RenderView__) */
