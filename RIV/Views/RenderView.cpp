//
//  RenderView.cpp
//  afstuderen_test
//
//  Created by Gerard Simons on 20/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "RenderView.h"

#include "../Data/DataSet.h"
#include "../../pbrt-v2/src/core/filter.h"
#include "../../pbrt-v2/src/filters/box.h"

RIVRenderView* RIVRenderView::instance = NULL;

RIVRenderView::RIVRenderView(RIVDataSet* dataset) : RIVDataView(dataset) , image(createImageFilm()) {
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
}

void RIVRenderView::Draw() {
	//Nothing yet
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
	
}

ImageFilm RIVRenderView::createImageFilm() {
	//If any previous film present, delete it
//	if(image) {
//		delete image;
//	}
	
	RIVTable *pathTable = dataset->GetTable("path");
	
	//Find x and y record
	RIVUnsignedShortRecord* xRecord = pathTable->GetRecord<RIVUnsignedShortRecord>("x");
	RIVUnsignedShortRecord* yRecord = pathTable->GetRecord<RIVUnsignedShortRecord>("y");
//	RIVUnsignedShortRecord* intersections = pathTable->GetRecord<RIVUnsignedShortRecord>("#intersections");
	
	ushort xMax = xRecord->Max() + 1;
	ushort yMax = yRecord->Max() + 1;
	
	float filterSize = 0.5F;
	Filter* boxFilter = new BoxFilter(filterSize,filterSize);
	
	const float crop[4] = {0,1,0,1};
	std::string outputfile = "";
	
	return ImageFilm(xMax, yMax, boxFilter, crop, outputfile, false);
}

void RIVRenderView::drawImageFilm() {
	float splatScale = 1; //No idea what value to set this to
	// Convert image to RGB and compute final pixel values
	int nPix = image.xPixelCount * image.yPixelCount;
	float *rgb = new float[3*nPix];
	int offset = 0;
	for (int y = 0; y < image.yPixelCount; ++y) {
		for (int x = 0; x < image.xPixelCount; ++x) {
			// Convert pixel XYZ color to RGB
			XYZToRGB((*image.pixels)(x, y).Lxyz, &rgb[3*offset]);
			
			// Normalize pixel with weight sum
			float weightSum = (*image.pixels)(x, y).weightSum;
			if (weightSum != 0.f) {
				float invWt = 1.f / weightSum;
				rgb[3*offset  ] = max(0.f, rgb[3*offset  ] * invWt);
				rgb[3*offset+1] = max(0.f, rgb[3*offset+1] * invWt);
				rgb[3*offset+2] = max(0.f, rgb[3*offset+2] * invWt);
			}
			
			// Add splat value at pixel
			float splatRGB[3];
			XYZToRGB((*image.pixels)(x, y).splatXYZ, splatRGB);
			rgb[3*offset  ] += splatScale * splatRGB[0];
			rgb[3*offset+1] += splatScale * splatRGB[1];
			rgb[3*offset+2] += splatScale * splatRGB[2];
			++offset;
		}
	}
//
//	// Write RGB image
////	::WriteImage(filename, rgb, NULL, xPixelCount, yPixelCount,
////				 xResolution, yResolution, xPixelStart, yPixelStart);
//	
//	// Release temporary image memory
//	delete[] rgb;


}
