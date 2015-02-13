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

typedef unsigned short ushort;

RIVImageView* RIVImageView::instance = NULL;
int RIVImageView::windowHandle = -1;

RIVImageView::RIVImageView(RIVDataSet<float,ushort>** datasetOne, EMBREERenderer* renderer) : RIVDataView(datasetOne) {
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    identifier = "ImageView";
    
	selection.start.x = -1;
    selection.start.y = -1;
    selection.end.x = -1;
    selection.end.y = -1;
	
	
	rendererOne = renderer;
}

RIVImageView::RIVImageView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo) : RIVDataView(datasetOne,datasetTwo),
	rendererOne(rendererOne), rendererTwo(rendererTwo) {
	if(instance != NULL) {
		throw "Only 1 instance of ImageView allowed.";
	}
	instance = this;
	identifier = "ImageView";
	
	selection.start.x = -1;
	selection.start.y = -1;
	selection.end.x = -1;
	selection.end.y = -1;
}

void RIVImageView::DrawInstance() {
    if(instance != NULL) {
        instance->Draw();
    }
    else {
        printf("No instance to draw.\n");
    }
}

void RIVImageView::ReshapeInstance(int width, int height) {
    if(instance != NULL) {
        instance->Reshape(width,height);
    }
    else printf("No instance to reshape");
}

void RIVImageView::Mouse(int button, int state, int x, int y) {
    if(instance != NULL) {
        instance->HandleMouse(button,state,x,y);
    }
}

void RIVImageView::Motion(int x, int y) {
    if(instance != NULL) {
        instance->HandleMouseMotion(x, y);
    }
}

void RIVImageView::Reshape(int width, int height) {
    
    printf("RIVImageView Reshape\n");
    
    this->width = width;
    this->height = height;
    
    paddingX = 0;
    paddingY = 0;
    
    startX = 0;
    startY = 0;
    
    imageStart.x = startX + paddingX;
	imageStart.y = startY + paddingY;
    
	imageEnd.x = startX + width - paddingX;
	imageEnd.y = startY + height - paddingY;
    
//    imageMagnificationX = (width - 2 * paddingX) / (float)renderedImage->sizeX;
//	imageMagnificationY = (height - 2 * paddingY) / (float)renderedImage->sizeY;
	
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
}

void RIVImageView::OnDataChanged(RIVDataSet<float,ushort>* source) {
    computeHeatmap(*datasetOne, heatmapOne);
    computeHeatmap(*datasetTwo, heatmapTwo);
    
	int currentWindow = glutGetWindow();
	glutSetWindow(RIVImageView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
}

void RIVImageView::OnFiltersChanged(RIVDataSet<float,ushort>* dataset) {
    //Recompute heatmap
    
    computeHeatmap(*datasetOne, heatmapOne);
    computeHeatmap(*datasetTwo, heatmapTwo);
    
    int currentWindow = glutGetWindow();
    glutSetWindow(RIVImageView::windowHandle);
    glutPostRedisplay();
    //Return window to given window
    glutSetWindow(currentWindow);
}

void RIVImageView::computeHeatmap(RIVDataSet<float,ushort>* dataset, Histogram2D<float>& heatmap) {
    heatmap = Histogram2D<float>(0, 1, bins);
    
    RIVTable<float,ushort>* pathsTable = dataset->GetTable(PATHS_TABLE);
    TableIterator* iterator = pathsTable->GetIterator();
    
    RIVRecord<float>* xPixel = pathsTable->GetRecord<float>(PIXEL_X);
    RIVRecord<float>* yPixel = pathsTable->GetRecord<float>(PIXEL_Y);
    
    size_t row;
    while(iterator->GetNext(row)) {
        heatmap.Add(xPixel->Value(row), yPixel->Value(row));
    }
    
    heatmap.Print();
}
void RIVImageView::drawHeatmap(int startX, Histogram2D<float>& heatmap, Histogram2D<float>& otherHeatmap, float r, float g, float b) {
//    glEnable(GL_BLEND);
    
    float binWidth = ((width / 2.F) - imagePadding * 2) / (float)bins;
    float binHeight = ((height) - imagePadding * 2) / (float)bins;
    
    float binX = startX;
    
    float maxNormalizedValue = heatmap.MaxBinValue() / (float)heatmap.NumberOfElements();
    float variance = std::pow(heatmap.NormalizedVariance(),1) * bins * bins;
    
//    printf("maxNormalizedValue = %f\n",maxNormalizedValue);
//    printf("Variance = %f\n",variance);
    
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    float mean = 1.F / (bins * bins);
    
    for(int x = 0 ; x < bins ; ++x) {
        float binY = height - 2 * imagePadding;
        for(int y = 0 ; y < bins ; ++y) {
            float normalizedValue = heatmap.NormalizedValue(x, y);
            float otherNormalizedValue = otherHeatmap.NormalizedValue(x, y);
            
//            printf("Normalized value = %f\n",normalizedValue);
//            printf("Other normalized value = %f\n",otherNormalizedValue);
            
//            if(normalizedValue > otherNormalizedValue) {
//                float alpha = (normalizedValue - otherNormalizedValue) / variance;
//                float alpha = ((normalizedValue - otherNormalizedValue) / normalizedValue + 1) / 2.F;
//                float alpha = normalizedValue - mean;
//                float alpha = std::pow(normalizedValue - otherNormalizedValue,0.5F);
            float alpha = std::pow(normalizedValue * variance,0.4);
//                printf("alpha = %f\n",alpha);
                glColor4f(r,g,b,alpha);
//                glColor3f(x / (float)bins,0,y / (float)bins);

                glRectf(binX, binY, binX + binWidth, binY - binHeight);
                printf("glRectf(%f,%f,%f,%f)\n",binX, binY, binX + binWidth, binY + binHeight);

//            }

            binY -= binHeight;
        }
        binX += binWidth;
    }
//    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
}
void RIVImageView::drawRenderedImage(EMBREERenderer *renderer, int startX, int startY, int imageWidth, int imageHeight) {
//    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	void* ptr = renderer->MapFrameBuffer();
	std::string format = renderer->GetFormat();
	Vec2<size_t> dimensions = renderer->GetDimensions();
    
	int g_width = dimensions[0];
	int g_height = dimensions[1];
    
//    glDisable(GL_BLEND);
	
	//Make sure it is scaled according to the available space as well flipped vertically
	glPixelZoom((float)imageWidth / g_width, -((float)imageHeight / g_height));
	//Because we flip it we have to translate it back to the top
	glRasterPos2i(1+startX, imageHeight);
    
//    char *array;
//    array = (char*)ptr;
//    int byteI = 0;
//    for(int x = 0 ; x < g_width ; ++x) {
//        for(int y = 0 ; y < g_height ; ++y) {
//            int c = (int)array[byteI];
//            printf("%d",c);
//            byteI += 4;
//        }
//    }
//    printf("%s\n",array);
	
	if (format == "RGB_FLOAT32")
		glDrawPixels((GLsizei)g_width,(GLsizei)g_height,GL_RGB,GL_FLOAT,ptr);
	else if (format == "RGBA8")
		glDrawPixels((GLsizei)g_width,(GLsizei)g_height,GL_RGBA,GL_UNSIGNED_BYTE,ptr);
	else if (format == "RGB8")
		glDrawPixels((GLsizei)g_width,(GLsizei)g_height,GL_RGB,GL_UNSIGNED_BYTE,ptr);
	else
		throw std::runtime_error("unknown framebuffer format: "+format);
	
	renderer->UnmapFrameBuffer();
}
void RIVImageView::redisplayWindow() {
	int currentWindow = glutGetWindow();
	glutSetWindow(RIVImageView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
}
size_t drawCounter = 0;
void RIVImageView::Draw() {
    needsRedraw = true;
	
	if(needsRedraw) {
		printHeader("IMAGE VIEW DRAW");
//		printf("\nImageView Draw #%zu\n",++drawCounter);
		glDisable(GL_DEPTH_TEST);
		
		glClearColor(1,1,1,1);
		glClear( GL_COLOR_BUFFER_BIT );
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		int halfWidth = width/2.F;
//		glColor3f(1,0,0);
//		glRectf(0, 0, halfWidth, height);
        printf("Draw rendererd image one\n");
		drawRenderedImage(rendererOne,imagePadding,imagePadding,halfWidth - imagePadding * 2,height - imagePadding * 2);
        drawHeatmap(imagePadding, heatmapOne, heatmapTwo, 1,0,0);
        
		if(rendererTwo != NULL) {
//			glColor3f(0, 0, 1);
//			glRectf(halfWidth, 0, width, height);
            printf("Draw rendererd image two\n");
			drawRenderedImage(rendererTwo,halfWidth,imagePadding,halfWidth - imagePadding * 2,height - imagePadding * 2);
            drawHeatmap(width / 2.F, heatmapTwo, heatmapOne, 0,0,1);
		}
		
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
	return false;
    y = height - y;
	if(isDragging || containsPoint(x,y)) {
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

				(*datasetOne)->ClearFilter<ushort>("x");
				(*datasetOne)->ClearFilter<ushort>("y");
				
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

//				riv::SingularFilter<ushort> *xFilter = new riv::RangeFilter<ushort>("x",selection.start.x,selection.end.x - 1);
				//Be sure to invert the Y coordinates!
//				riv::SingularFilter<ushort> *yFilter = new riv::RangeFilter<ushort>("y", renderedImage->sizeY - selection.start.y,renderedImage->sizeY - selection.end.y - 1);
		
//				(*datasetOne)->StartFiltering();
//				(*datasetOne)->AddFilter("path",xFilter);
//				(*datasetOne)->AddFilter("path",yFilter);
//				(*datasetOne)->StopFiltering();
					
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
    
	//Clear any filters that may have been applied to the (*dataset)

	(*datasetOne)->ClearFilter<ushort>("x");
	(*datasetOne)->ClearFilter<ushort>("y");
	
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