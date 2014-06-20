//
//  ImageView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ImageView.h"
#include "loadppm.h"
#include "Filter.h"

#include <math.h>



RIVImageView::RIVImageView(const BMPImage &image, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty,RIVSizeProperty* sizeProperty) : RIVDataView(x,y,width,height, paddingX, paddingY,colorProperty,sizeProperty) ,renderedImage(image){

    ComputeLayout(x, y, width, height, paddingX, paddingY);
    
    identifier = "ImageView";

	imageWidth = width;
	imageHeight = height;

	isDragging = false;

    createTextureImage(image);
    
    clearSelection();
}

//This shit aint working, the data gets garbled up?!
void RIVImageView::createTextureImage() {
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    
    std::cout << renderedImage;
    
    printf("image ID = %d\n",renderedImage.ID);
    printf("image data pointer = %p\n",renderedImage.data);
    
    if(renderedImage.hasAlpha) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, renderedImage.sizeX, renderedImage.sizeY,GL_BGRA_EXT, GL_UNSIGNED_BYTE, renderedImage.data); //For BMP images use this
    }
    else {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, renderedImage.sizeX, renderedImage.sizeY,GL_BGR_EXT, GL_UNSIGNED_BYTE, renderedImage.data); //For BMP images use this
    }
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY,GL_RGB, GL_UNSIGNED_BYTE, image.data); //for P6 formatted PPM use this
    
	//Compute magnification
	imageMagnificationX = (width - 2 * paddingX) / (float)renderedImage.sizeX;
	imageMagnificationY = (height - 2 * paddingY) / (float)renderedImage.sizeY;
}

void RIVImageView::createTextureImage(const BMPImage& image) {
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    
    std::cout << image;
    
    printf("image ID = %d\n",image.ID);
    printf("image data pointer = %p\n",image.data);
    
    if(renderedImage.hasAlpha) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.sizeX, image.sizeY,GL_BGRA_EXT, GL_UNSIGNED_BYTE, image.data); //For BMP images use this
    }
    else {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY,GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data); //For BMP images use this
    }
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY,GL_RGB, GL_UNSIGNED_BYTE, image.data); //for P6 formatted PPM use this
    
	//Compute magnification
	imageMagnificationX = (width - 2 * paddingX) / (float)image.sizeX;
	imageMagnificationY = (height - 2 * paddingY) / (float)image.sizeY;
}

void RIVImageView::Draw() {
    
    glViewport(startX,startY,width,height);  //Use the whole window for rendering
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(startX, startY, width, height);
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable(GL_SCISSOR_TEST);
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glDisable(GL_DEPTH_TEST);
    
    //Setup viewport and 2D projection
    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //Color the view port
//    glColor3f(0.F,1.F,0.F);
//    glBegin(GL_QUADS);
//    glVertex3f(0,0,0);
//    glVertex3f(width,0,0);
//    glVertex3f(width,height,0);
//    glVertex3f(0,height,0);
//    glEnd();
//    return;
    
	if(needsRedraw) {
		glBindTexture(GL_TEXTURE_2D, imageTexture);
    
		//remember all states of the GPU
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3f(1,1,1);
		glNormal3d(0, 0, 1);
		glEnable(GL_TEXTURE_2D);
    
		// texture addition
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_NEAREST);
        
//        std::cout << "imageStart.x = " << imageStart.x << std::endl;
//        std::cout << "imageStart.y = " << imageStart.y << std::endl;
//        std::cout << "imageEnd.x = " << imageEnd.x << std::endl;
//        std::cout << "imageEnd.y = " << imageEnd.y << std::endl;
    
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

void RIVImageView::OnDataSetChanged() {
    //Do nothing for imageview
}

void RIVImageView::ComputeLayout(float newStartX, float newStartY, float newWidth, float newHeight, float paddingX, float paddingY) {
    
    this->paddingX = paddingX;
    this->paddingY = paddingY;
    
    startX = newStartX;
    startY = newStartY;
    
    width = newWidth;
    height = newHeight;
    
	imageStart.x = startX + paddingX;
	imageStart.y = startY + paddingY;

	imageEnd.x = startX + width - paddingX * 2;
	imageEnd.y = startY + height - paddingY * 2;
    
    createTextureImage();
}

int round(float d)
{
  return (int)floor(d + 0.5F);
}

bool RIVImageView::HandleMouse(int button, int state, int x, int y) {
    ToViewSpaceCoordinates(&x, &y);
	if(isDragging || RIVDataView::containsPoint(x,y)) {
		//If start dragging > init selection
		if(state == GLUT_DOWN) {
			//init selection
			if(!isDragging) {
				selection.start = viewToPixelSpace(x,y);
                printf("selection (startX,endX) = (%d,%d)\n",selection.start.x,selection.end.x);
				isDragging = true;
			}
		}
		else if(state == GLUT_UP) {
			//Finish selection

			printf("selection (startX,endX) = (%d,%d)\n",selection.start.x,selection.end.x);
            printf("selection (startY,endY) = (%d,%d)\n",selection.start.y,selection.end.y);

			if(selection.end.x != selection.start.x && selection.end.y != selection.start.y) {
				dataset->ClearFilters();

				//Normalize selection
				if(selection.end.x < selection.start.x) {
					int tempX = selection.start.x;
					selection.start.x = selection.end.x;
					selection.end.x = tempX;
				}
				if(selection.end.y < selection.start.y) {
					int tempY = selection.start.y;
					selection.start.y = selection.end.y;
					selection.end.y = tempY;
				}
//
				Filter *xFilter = new RangeFilter("x",selection.start.x,selection.end.x - 1);
				Filter *yFilter = new RangeFilter("y",selection.start.y,selection.end.y - 1);
                
//                Filter *xFilter = new RangeFilter("x",9,22);
//                Filter *yFilter = new RangeFilter("y",5,6);
		
				dataset->AddFilter(xFilter);
				dataset->AddFilter(yFilter);
                
//                dataset->Print();
			}
			else {
				clearSelection(); 
			}
			isDragging = false;
		}
		return true;
	}
	else {
		//Clear any possible selection
//		clearSelection();
		return false;
	}
}

void RIVImageView::clearSelection() {
	//Set the selection to off
	selection.start.x = -1;
    selection.start.y = -1;
    selection.end.x = -1;
    selection.end.y = -1;
    
	//Clear any filters that may have been applied to the dataset
    if(dataset) {
        dataset->ClearFilter("x");
        dataset->ClearFilter("y");
    }
}

Point RIVImageView::viewToPixelSpace(int x, int y) {
	int pixelX = round((float)x / imageMagnificationX);
	int pixelY = round((float)y / imageMagnificationY);

	Point pixel;

	//How many pixels does the original image have?
	int nrOfXPixels = round(imageWidth / imageMagnificationX);
	int nrOfYPixels = round(imageHeight / imageMagnificationY);

	pixel.x = min(max(pixelX,0),nrOfXPixels);
	pixel.y = min(max(pixelY,0),nrOfYPixels);

	return pixel;
}

bool RIVImageView::HandleMouseMotion(int x, int y) {
	if(isDragging) {
		Point pixel = viewToPixelSpace(x,y);
		selection.end = pixel;
		return true;
	}
	else return false;
}
