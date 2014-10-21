//
//  RenderView.cpp
//  afstuderen_test
//
//  Created by Gerard Simons on 20/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "RenderView.h"
#include "../reporter.h"
#include "../Data/DataSet.h"
#include "../../pbrt-v2/src/core/filter.h"
#include "../../pbrt-v2/src/filters/box.h"

RIVRenderView* RIVRenderView::instance = NULL;
int RIVRenderView::windowHandle = -1;

RIVRenderView::RIVRenderView(RIVDataSet* dataset) : RIVDataView(dataset) {
	instance = this;
};

void RIVRenderView::DrawInstance() {
	if(instance != NULL) {
		instance->Draw();
	}
}

void RIVRenderView::ReshapeInstance(int width, int height){
	if(instance != NULL) {
		instance->Reshape(width, height);
	}
}

void RIVRenderView::Mouse(int button, int state, int x, int y) {
	if(instance != NULL) {
		instance->Mouse(button,state,x,y);
	}
}

void RIVRenderView::Motion(int x, int y) {
	if(instance != NULL) {
		instance->Motion(x,y);
	}
}

void RIVRenderView::Reshape(int width, int height) {
	printf("RIVRenderView Reshape called.\n");
	this->width = width;
	this->height = height;
	
	paddingX = 0;
	paddingY = 0;
	
	startX = 0;
	startY = 0;
	
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	CreateImageFilm();
}

void RIVRenderView::Draw() {
	//Nothing yet
	printf("\n****** RIVRenderView Draw\n");
	
	std::string taskName = "Drawing RenderView";
	reporter::startTask(taskName);
	
	glClearColor(1.0, 0, 0, 0.0);
	glClear( GL_COLOR_BUFFER_BIT );
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glColor3f(1, 0, 0);
	glBegin(GL_LINE);
	glVertex3f(0, 0, 1);
	glVertex3f(width, height, 1);
	glEnd();
	
	drawImageFilm();
	
	glutSwapBuffers();
	
	reporter::stop(taskName);
}

//Not implemented
bool RIVRenderView::HandleMouse(int,int,int,int) {
	return false;
}

// Not implemented
bool RIVRenderView::HandleMouseMotion(int,int) {
	return false;
}

void RIVRenderView::OnDataSetChanged() {
	//Recreate image film
	CreateImageFilm();
	
	//Post redisplay for this window
	int currentWindow = glutGetWindow();
	glutSetWindow(RIVRenderView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
}

void RIVRenderView::CreateImageFilm() {
	//If any previous film present, delete it
	if(image) {
		delete image;
	}
	std::string taskName = "Create image film";
	reporter::startTask(taskName);
	renderedImage.clear();
	
	RIVTable *pathTable = dataset->GetTable("path");
	
	//Find x and y record
	RIVFloatRecord* xRecord = pathTable->GetRecord<RIVFloatRecord>("x");
	RIVFloatRecord* yRecord = pathTable->GetRecord<RIVFloatRecord>("y");
//	RIVUnsignedShortRecord* intersections = pathTable->GetRecord<RIVUnsignedShortRecord>("#intersections");
	
	ushort xMax = xRecord->Max() + 1;
	ushort yMax = yRecord->Max() + 1;
	
	pixelWidth = width / (float)xMax;
	pixelHeight = height / (float)yMax;
	
	float filterSize = 0.5F;
	Filter* boxFilter = new BoxFilter(filterSize,filterSize);
	
	const float crop[4] = {0,1,0,1};
	std::string outputfile = "";
	
	image = new ImageFilm(xMax, yMax, boxFilter, crop, outputfile, false);
	
	//Add the samples to the film
	RIVFloatRecord* radianceR = pathTable->GetRecord<RIVFloatRecord>("radiance R");
	RIVFloatRecord* radianceG = pathTable->GetRecord<RIVFloatRecord>("radiance G");
	RIVFloatRecord* radianceB = pathTable->GetRecord<RIVFloatRecord>("radiance B");
	RIVFloatRecord* lensU = pathTable->GetRecord<RIVFloatRecord>("lens U");
	RIVFloatRecord* lensV = pathTable->GetRecord<RIVFloatRecord>("lens V");
	RIVFloatRecord* timestamp = pathTable->GetRecord<RIVFloatRecord>("timestamp");
	
	size_t row = 0;
	TableIterator* iter = pathTable->GetIterator();
	size_t samplesAdded = 0;
	while(iter->GetNext(row)) {
		//Recreate the spectrum4
		const float RGB[3] = {radianceR->Value(row),radianceG->Value(row),radianceB->Value(row)};
//		printf("Adding RGB = ");
//		printArray(RGB, 3);
		RGBSpectrum spectrum = RGBSpectrum::FromRGB(RGB);
		
		//Recreate the camera sample
		CameraSample cameraSample;
		cameraSample.lensU = lensU->Value(row);
		cameraSample.lensV = lensV->Value(row);
		cameraSample.time = timestamp->Value(row);
		cameraSample.imageX = xRecord->Value(row);
		cameraSample.imageY = yRecord->Value(row);
		
		image->AddSample(cameraSample, spectrum);
		++samplesAdded;
	}
	
	
	//Create the rendered image
	renderedImage.resize(xMax);
	
	float splatScale = 1; //No idea what value to set this to
	// Convert image to RGB and compute final pixel values
	int nPix = image->xPixelCount * image->yPixelCount;
	float *rgb = new float[3*nPix];
	int offset = 0;
	for (int x = 0; x < image->xPixelCount; ++x) {
		renderedImage[x].resize(yMax);
		for (int y = 0; y < image->yPixelCount; ++y) {
			// Convert pixel XYZ color to RGB
			XYZToRGB((*image->pixels)(x, y).Lxyz, &rgb[3*offset]);
//			printf("RGB pre-splat = ");
//			printArray(rgb, 3);
			
			// Normalize pixel with weight sum
			float weightSum = (*image->pixels)(x, y).weightSum;
			if (weightSum != 0.f) {
				float invWt = 1.f / weightSum;
				rgb[3*offset  ] = max(0.f, rgb[3*offset  ] * invWt);
				rgb[3*offset+1] = max(0.f, rgb[3*offset+1] * invWt);
				rgb[3*offset+2] = max(0.f, rgb[3*offset+2] * invWt);
			}
			
			// Add splat value at pixel
			float splatRGB[3];
			XYZToRGB((*image->pixels)(x, y).splatXYZ, splatRGB);
			rgb[3*offset  ] += splatScale * splatRGB[0];
			rgb[3*offset+1] += splatScale * splatRGB[1];
			rgb[3*offset+2] += splatScale * splatRGB[2];
//			printf("RGB post-splat = ");
//			printArray(rgb, 3);
			float r = rgb[3*offset];
			float g = rgb[3*offset+1];
			float b = rgb[3*offset+2];
			++offset;
			Color pixelColor;
			pixelColor.R = r;
			pixelColor.G = g;
			pixelColor.B = b;
			renderedImage[x][renderedImage[x].size() - y - 1] = pixelColor;
		}
	}
	
//	printf("Rendered image:\n ");
//	for(const std::vector<Color>& row : renderedImage) {
//		for(const Color& color : row) {
//			printf("(%f,%f,%f) ",color.R,color.G,color.B);
//		}
//		printf("\n");
//	}
	reporter::stop(taskName);
	printf("%zu samples added to image film\n",samplesAdded);
}

void RIVRenderView::drawImageFilm() {

//
//	// Write RGB image
//	image->WriteImage("/Users/gerardsimons/testImage", rgb, NULL, xPixelCount, yPixelCount,
//				 xResolution, yResolution, xPixelStart, yPixelStart);
//
//	// Release temporary image memory
//	delete[] rgb;
	
	for(ushort x = 0 ; x < renderedImage.size() ; ++x) {
		for(ushort y = 0 ; y < renderedImage[x].size() ; ++y) {
			glBegin(GL_QUADS);
			Color c = renderedImage[x][y];
			glColor3f(c.R, c.G, c.B);
//			glColor3f(1,0,0);
			
//			printf("glColor3f(%f,%f,%f)\n",c.R,c.G,c.B);
//			printf("glVertex2f(%f,%f)\n",x*pixelWidth, y*pixelHeight);
//			printf("glVertex2f(%f,%f)\n",(x+1)*pixelWidth, y*pixelHeight);
//			printf("glVertex2f(%f,%f)\n",(x+1)*pixelWidth, (y+1)*pixelHeight);
//			printf("glVertex2f(%f,%f)\n",x*pixelWidth, (y+1)*pixelHeight);
			
			glVertex2f(x*pixelWidth, y*pixelHeight);
			glVertex2f((x+1)*pixelWidth, y*pixelHeight);
			glVertex2f((x+1)*pixelWidth, (y+1)*pixelHeight);
			glVertex2f(x*pixelWidth, (y+1)*pixelHeight);
			
			glEnd();
		}
	}
}
