//
//  ImageView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ImageView.h"
#include "../Graphics/loadppm.h"
#include "../Data/Filter.h"

#include <math.h>

std::vector<RIVImageView*> RIVImageView::instances;
int RIVImageView::windowHandle = -1;

RIVImageView::RIVImageView(RIVDataSet* dataset, EMBREERenderer* renderer, RIVColorProperty* color, RIVSizeProperty* size) : RIVDataView(dataset,color,size), renderer(renderer) {
	instances.push_back(this);
    identifier = "ImageView";
	this->renderer = renderer;
    
	selection.start.x = -1;
    selection.start.y = -1;
    selection.end.x = -1;
    selection.end.y = -1;
}

void RIVImageView::InitializeGraphics() {
	
}

void RIVImageView::DrawInstances() {
	for(RIVImageView* instance : instances) {
		if(instance->IsDirty()) {
			instance->Draw();
		}
	}
}

void RIVImageView::ReshapeInstances(int width, int height) {
	float newWidth = width / 2.F;
	for(RIVImageView* instance : instances) {
		instance->Reshape(newWidth, height);
	}
}


void RIVImageView::Mouse(int button, int state, int x, int y) {
	for(RIVImageView* instance : instances) {
		if(instance->HandleMouse(button, state, x, y)) {
			return;
		}
	}
}

void RIVImageView::Motion(int x, int y) {
	for(RIVImageView* instance : instances) {
		if(instance->HandleMouseMotion(x, y)) {
			return;
		}
	}
}

void RIVImageView::Reshape(int width, int height) {
    
    printf("RIVImageView Reshape\n");
    
    this->width = width;
    this->height = height;
    
    paddingX = 0;
    paddingY = 0;
	
	x = 0;
	y = 0;
    
    imageStart.x = x + paddingX;
	imageStart.y = y + paddingY;
    
	imageEnd.x = x + width - paddingX;
	imageEnd.y = y + height - paddingY;
    
//    imageMagnificationX = (width - 2 * paddingX) / (float)renderedImage->sizeX;
//	imageMagnificationY = (height - 2 * paddingY) / (float)renderedImage->sizeY;
	
    glViewport(x, y, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
}

void RIVImageView::OnDataChanged() {
	int currentWindow = glutGetWindow();
	glutSetWindow(RIVImageView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
}

void RIVImageView::OnFiltersChanged() {
	//Nothing to do for imageview
}

size_t drawCounter = 0;
void RIVImageView::Draw() {
    dirty = true;
	

	
	if(dirty) {
		printf("\nImageView Draw #%zu\n",++drawCounter);
		glDisable(GL_DEPTH_TEST);
		
		glClearColor(1,1,1,0);
		glClear( GL_COLOR_BUFFER_BIT );
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		//remember all states of the GPU
//		glPushAttrib(GL_ALL_ATTRIB_BITS);
//		glColor3f(1,1,1);
		
		void* ptr = renderer->MapFrameBuffer();
		std::string format = renderer->GetFormat();
		Vec2<size_t> dimensions = renderer->GetDimensions();
		int g_width = dimensions[0];
		int g_height = dimensions[1];

		//Make sure it is scaled according to the available space as well flipped vertically
		glPixelZoom((float)width / g_width, -((float)height / g_height));
		//Because we flip it we have to translate it back to the top
		glRasterPos2i(1, height);
		
		if (format == "RGB_FLOAT32")
			glDrawPixels((GLsizei)g_width,(GLsizei)g_height,GL_RGB,GL_FLOAT,ptr);
		else if (format == "RGBA8")
			glDrawPixels((GLsizei)g_width,(GLsizei)g_height,GL_RGBA,GL_UNSIGNED_BYTE,ptr);
		else if (format == "RGB8")
			glDrawPixels((GLsizei)g_width,(GLsizei)g_height,GL_RGB,GL_UNSIGNED_BYTE,ptr);
		else
			throw std::runtime_error("unknown framebuffer format: "+format);
		
		glFlush();
		glutSwapBuffers();
		
		renderer->UnmapFrameBuffer();
		
		renderer->outputMode("imageview_out_" + std::to_string(drawCounter) + ".tga");
		
//		void* ptr = renderer->MapFrameBuffer();
//		char* bytes = (char*)ptr;

//		int size = width*height;
//		float* pixels = new float[size*3];
//		for(int i=0;i<size;i++) {
//			pixels[i] = i / (float)size;
//		}
//		for(int x = 0 ; x < width ; x++) {
//			for(int y = 0 ; y < height ; y++) {
//				pixels[x*width+y] = x / (float)height;
//				printf("bytes[%d]=%d\n",x*width+y,(int)bytes[x*width+y]);
//				bytes[x*width+y] = 128;
//			}
//			printf("\n");
//		}
		
//		glRasterPos2i(1, 100);
//		glPixelZoom(1.0f, -1.0f);
//		glDrawPixels((GLsizei)512,(GLsizei)512,GL_RGBA,GL_UNSIGNED_BYTE,ptr);
//		std::string g_format = "RGB8";
//		if (g_format == "RGB_FLOAT32")
//			glDrawPixels((GLsizei)width,(GLsizei)height,GL_RGB,GL_FLOAT,ptr);
//		else if (g_format == "RGBA8")
//			glDrawPixels((GLsizei)width,(GLsizei)height,GL_RGBA,GL_UNSIGNED_BYTE,ptr);
//		else if (g_format == "RGB8")
//			glDrawPixels((GLsizei)width,(GLsizei)height,GL_RGB,GL_FLOAT,ptr);
		
		renderer->UnmapFrameBuffer();

		//reset to previous state
//		glPopAttrib();
		
		//if selection, draw
//		if(selection.start.x != -1) {
//			glColor4f(0.317F,.553F, .741F,.5F); //Nice light blue color for selection
//			glBegin(GL_QUADS);
//				glVertex3f(selection.start.x * imageMagnificationX,selection.start.y  * imageMagnificationY,1);			
//				glVertex3f(selection.end.x  * imageMagnificationX,selection.start.y  * imageMagnificationY,1);
//				glVertex3f(selection.end.x  * imageMagnificationX,selection.end.y  * imageMagnificationY,1);
//				glVertex3f(selection.start.x  * imageMagnificationX,selection.end.y  * imageMagnificationY,1);
//			glEnd();
//		}
		glFlush();
		glutSwapBuffers();
	}
}

#ifdef __APPLE__
int round(float d)
{
  return (int)floor(d + 0.5F);
}
#endif

bool RIVImageView::HandleMouse(int button, int state, int x, int y) {
//    ToViewSpaceCoordinates(&x, &y);
    y = height - y;
	if(isDragging || containsPoint(x,y)) {
		//If start dragging > init selection
		if(state == GLUT_DOWN) {
			//init selection
			if(!isDragging) {
				selection.start = viewToPixelSpace(x,y);
                printf("selection (x,endX) = (%d,%d)\n",selection.start.x,selection.end.x);
				isDragging = true;
			}
		}
		else if(state == GLUT_UP) {
			//Finish selection

			printf("selection (x,endX) = (%d,%d)\n",selection.start.x,selection.end.x);
            printf("selection (y,endY) = (%d,%d)\n",selection.start.y,selection.end.y);

			if(selection.end.x != selection.start.x && selection.end.y != selection.start.y) {

				dataset->ClearFilter("x");
				dataset->ClearFilter("y");
				
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

//				riv::Filter *xFilter = new riv::RangeFilter("x",selection.start.x,selection.end.x - 1);
				//Be sure to invert the Y coordinates!
//				riv::Filter *yFilter = new riv::RangeFilter("y", renderedImage->sizeY - selection.start.y,renderedImage->sizeY - selection.end.y - 1);
		
				dataset->StartFiltering();
//				dataset->AddFilter("path",xFilter);
//				dataset->AddFilter("path",yFilter);
				dataset->StopFiltering();
					
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

bool RIVImageView::HandleMouseMotion(int x, int y) {
    y = height - y;
	if(isDragging) {
		RIVPoint pixel = viewToPixelSpace(x,y);
		selection.end = pixel;
        glutPostRedisplay();
		return true;
	}
	else return false;
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

RIVPoint RIVImageView::viewToPixelSpace(int x, int y) {
	int pixelX = round((float)x / imageMagnificationX);
	int pixelY = round((float)y / imageMagnificationY);

	RIVPoint pixel;

	//How many pixels does the original image have?
	int nrOfXPixels = round(imageWidth / imageMagnificationX);
	int nrOfYPixels = round(imageHeight / imageMagnificationY);

	pixel.x = std::min(std::max(pixelX,0),nrOfXPixels);
	pixel.y = std::min(std::max(pixelY,0),nrOfYPixels);

	return pixel;
}