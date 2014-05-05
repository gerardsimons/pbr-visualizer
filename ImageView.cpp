//
//  ImageView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ImageView.h"
#include "BMPImage.h"
#include "loadppm.h"
#include "Filter.h"

#include <math.h>



RIVImageView::RIVImageView(char* filename, int x, int y, int width, int height, int paddingX, int paddingY) : RIVDataView(x,y,width,height, paddingX, paddingY) {

    //PPMImage image(filename);
	BMPImage image(filename,true);

    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

	imageWidth = width;
	imageHeight = height;

	isDragging = false;

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.sizeX, image.sizeY,GL_BGRA_EXT, GL_UNSIGNED_BYTE, image.data); //For BMP images use this
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY,GL_RGB, GL_UNSIGNED_BYTE, image.data); //for P6 formatted PPM use this

	//Compute magnification
	imageMagnificationX = (width - 2 * paddingX) / (float)image.sizeX;
	imageMagnificationY = (height - 2 * paddingY) / (float)image.sizeY;
}

void RIVImageView::Draw() {
	if(needsRedraw) {
		glBindTexture(GL_TEXTURE_2D, imageTexture);
    
		//remember all states of the GPU
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3f(1,1,1);
		glNormal3d(0, 0, 1);
		glEnable(GL_TEXTURE_2D);
    
		// texture addition
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(imageStart.x,imageEnd.y,1);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(imageStart.x,imageStart.y,1);
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(imageEnd.x,imageStart.y,1);
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(imageEnd.x,imageEnd.y,1);
		glEnd();
		//reset to previous state
		glPopAttrib();
    
		glDisable(GL_TEXTURE_2D);
		//if selection, draw
		if(selection.start.x != -1) {
			glColor4f(0.317F,.553F, .741F,.5F); //Nice light blue color for selection
			glBegin(GL_QUADS);
				glVertex3f(selection.start.x * imageMagnificationX,selection.start.y  * imageMagnificationY,1);			
				glVertex3f(selection.end.x  * imageMagnificationX,selection.start.y  * imageMagnificationY,1);
				glVertex3f(selection.end.x  * imageMagnificationX,selection.end.y  * imageMagnificationY,1);
				glVertex3f(selection.start.x  * imageMagnificationX,selection.end.y  * imageMagnificationY,1);
			glEnd();
		}
		//needsRedraw = false; //TODO: This does not work when losing and regaining focus!
	}
}

void RIVImageView::ComputeLayout() { 		
	imageStart.x = startX + paddingX;
	imageStart.y = startY + paddingY;

	imageEnd.x = startX + width - paddingX * 2;
	imageEnd.y = startY + height - paddingY * 2;
}


int round(float d)
{
  return (int)floor(d + 0.5F);
}

bool RIVImageView::HandleMouse(int button, int state, int x, int y) {
	if(RIVDataView::containsPoint(x,y)) {
		//If start dragging > init selection
		if(state == GLUT_DOWN) {
			//init selection
			if(!isDragging) {
				selection.start = screenToPixelSpace(x,y);

				isDragging = true;
			}
		}
		else if(state == GLUT_UP) {
			//Finish selection

			printf("selection (x,y) = (%d,%d)\n",selection.start.x,selection.start.y);
			printf("selection (endX,endY) = (%d,%d)\n",selection.end.x,selection.end.y);

			dataset->ClearFilters();

			Filter *xFilter = new RangeFilter("x",selection.start.x,selection.end.x);
			Filter *yFilter = new RangeFilter("y",selection.start.y,selection.end.y);
		
			dataset->AddFilter(xFilter);
			dataset->AddFilter(yFilter);

			isDragging = false;
		}
		return true;
	}
	else {
		//Clear any possible selection
		selection.start.x = -1;
		return false;
	}
}

Point RIVImageView::screenToPixelSpace(int x, int y) {
	int pixelX = round((float)x / imageMagnificationX);
	int pixelY = round((float)y / imageMagnificationY);

	Point pixel;
	pixel.x = min(max(pixelX,0),imageWidth);
	pixel.y = min(max(pixelY,0),imageHeight);

	return pixel;
}

bool RIVImageView::HandleMouseMotion(int x, int y) {
	
	if(isDragging) {
		Point pixel = screenToPixelSpace(x,y);
		selection.end = pixel;
		return true;
	}
	else return false;
}
