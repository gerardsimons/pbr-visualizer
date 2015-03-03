//
//  ImageView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ImageView.h"
#include "../Graphics/loadppm.h"


#include <math.h>

typedef unsigned short ushort;

RIVImageView* RIVImageView::instance = NULL;
int RIVImageView::windowHandle = -1;

RIVImageView::RIVImageView(EMBREERenderer* renderer) : RIVDataView(datasetOne) {
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    identifier = "ImageView";
    paintGridOne = new Grid(gridSize);
    rendererOne = renderer;
}

RIVImageView::RIVImageView(RIVDataSet<float,ushort>** datasetOne, EMBREERenderer* renderer) : RIVDataView(datasetOne) {
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    identifier = "ImageView";
    paintGridOne = new Grid(gridSize);
    
    rendererOne = renderer;
}

RIVImageView::RIVImageView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo) : RIVDataView(datasetOne,datasetTwo), rendererOne(rendererOne), rendererTwo(rendererTwo) {
    
    paintGridOne = new Grid(gridSize);
    paintGridTwo = new Grid(gridSize);
    
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    identifier = "ImageView";
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
void RIVImageView::ToggleShowHeatmap() {
    showHeatmap = !showHeatmap;
    redisplayWindow();
    printf("Image heatmap is now ");
    if(showHeatmap) {
        printf("ON\n");
    }
    else {
        printf("OFF\n");
    }
}
void RIVImageView::OnDataChanged(RIVDataSet<float,ushort>* source) {
    
    if(source == *datasetOne) {
        computeHeatmap(*datasetOne, heatmapOne);
    }
    else if(datasetTwo && *datasetTwo == source) {
        computeHeatmap(*datasetTwo, heatmapTwo);
//        heatmapTwo->Print();
    }
    else {
        throw std::runtime_error("No such dataset");
    }
    redisplayWindow();
}

void RIVImageView::OnFiltersChanged(RIVDataSet<float,ushort>* dataset) {
    //Recompute heatmap
    
    if(dataset == *datasetOne) {
            computeHeatmap(*datasetOne, heatmapOne);
    }
    else if(datasetTwo && dataset == *datasetTwo) {
            computeHeatmap(*datasetTwo, heatmapTwo);
    }

    int currentWindow = glutGetWindow();
    glutSetWindow(RIVImageView::windowHandle);
    glutPostRedisplay();
    //Return window to given window
    glutSetWindow(currentWindow);
}

void RIVImageView::computeHeatmap(RIVDataSet<float,ushort>* dataset, Histogram2D<float>*& heatmap) {
    if(heatmap) {
        delete heatmap;
    }
    heatmap = new Histogram2D<float>(0, 1, bins);
    
    RIVTable<float,ushort>* pathsTable = dataset->GetTable(PATHS_TABLE);
    TableIterator* iterator = pathsTable->GetIterator();
    
    RIVRecord<float>* xPixel = pathsTable->GetRecord<float>(PIXEL_X);
    RIVRecord<float>* yPixel = pathsTable->GetRecord<float>(PIXEL_Y);
    RIVRecord<float>* throughputsR = pathsTable->GetRecord<float>(THROUGHPUT_R);
    RIVRecord<float>* throughputsG = pathsTable->GetRecord<float>(THROUGHPUT_G);
    RIVRecord<float>* throughputsB = pathsTable->GetRecord<float>(THROUGHPUT_B);
    
    size_t row;
    while(iterator->GetNext(row)) {
        float throughputR = throughputsR->Value(row);
        float throughputG = throughputsG->Value(row);
        float throughputB = throughputsB->Value(row);
        
//        printf("throughput (r,g,b) = (%f,%f,%f)\n",throughputR,throughputG,throughputB);
        
//        float averageThroughput = (throughputR + throughputG + throughputB) / 3.F;
//        printf("averageThroughput = %f\n",averageThroughput);
//        size_t magnitude = 1000 * averageThroughput;
        heatmap->Add(xPixel->Value(row), yPixel->Value(row),1);
    }
    
//    heatmap.Print();
}
void RIVImageView::drawHeatmap(int startX, Histogram2D<float>* heatmap, float r, float g, float b) {
    //    glEnable(GL_BLEND);
    
    if(heatmap) {
        float binWidth;
        if(datasetTwo) {
            binWidth = ((width / 2.F) - imagePadding * 2) / (float)bins;
        }
        else {
            binWidth = ((width) - imagePadding * 2) / (float)bins;
        }
        float binHeight = ((height) - imagePadding * 2) / (float)bins;
        
        float binX = startX;
        
            float maxNormalizedValue = heatmap->MaxBinValue() / (float)heatmap->NumberOfElements();
        float variance = std::pow(heatmap->NormalizedVariance(),1) * bins * bins; //Normalized for the number of bins
        
        //    printf("maxNormalizedValue = %f\n",maxNormalizedValue);
        //    printf("Variance = %f\n",variance);
        
        //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //    float mean = 1.F / (bins * bins);
        
//        float maxValue = 
        
    //    riv::ColorMap jetColorMap = colors::jetColorMap();
        
        for(int x = 0 ; x < bins ; ++x) {
            float binY = height - 2 * imagePadding;
            for(int y = 0 ; y < bins ; ++y) {
                float normalizedValue = heatmap->NormalizedValue(x, y);
                
                //            printf("Normalized value = %f\n",normalizedValue);
                //            printf("Other normalized value = %f\n",otherNormalizedValue);
                
                //            if(normalizedValue > otherNormalizedValue) {
                //                float alpha = (normalizedValue - otherNormalizedValue) / variance;
                //                float alpha = ((normalizedValue - otherNormalizedValue) / normalizedValue + 1) / 2.F;
                //                float alpha = normalizedValue - mean;
                //                float alpha = std::pow(normalizedValue - otherNormalizedValue,0.5F);
                float alpha = std::pow(normalizedValue * variance,0.5);
                //                printf("alpha = %f\n",alpha);
    //            jetColorMap.ComputeColor(alpha);
                glColor4f(r,g,b,alpha);
                //                glColor3f(x / (float)bins,0,y / (float)bins);
                
                glRectf(binX, binY, binX + binWidth, binY - binHeight);
                //                printf("glRectf(%f,%f,%f,%f)\n",binX, binY, binX + binWidth, binY + binHeight);
                
                //            }
                
                binY -= binHeight;
            }
            binX += binWidth;
        }
    }
    //    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
}
Histogram2D<float>* RIVImageView::GetHeatmapOne() {
    return heatmapOne;
}
Histogram2D<float>* RIVImageView::GetHeatmapTwo() {
    return heatmapTwo;
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
void RIVImageView::drawGrid(float startX, Grid* paintGrid) {
    
    int gridWidth = paintGrid->GetWidth();
    int gridHeight = paintGrid->GetHeight();
    
    float gridPixelWidth = (width / 2.F - imagePadding) / (float)gridWidth;
    float gridPixelHeight = (height - 2 * imagePadding) / (float)gridHeight;
    
    float pixelX = startX;
    for(unsigned int x = 0 ; x < gridWidth ; ++x) {
        float pixelY = 0;
        for(unsigned int y = 0 ; y < gridHeight ; ++y) {
//            glColor3f(x / (float)gridWidth, 0, y / (float)gridHeight);
            bool cellFilled = paintGrid->IsFilled(x, y);
            if(cellFilled) {
                glColor3f(1,0,0);
                glRectf(pixelX, pixelY, pixelX+gridPixelWidth, pixelY+gridPixelHeight);
            }
            pixelY += gridPixelHeight;
        }
        pixelX += gridPixelWidth;
    }
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
        int renderImageWidth = width;
        if(rendererTwo) {
            renderImageWidth = width/2.F;
        }
        //		glColor3f(1,0,0);
        //		glRectf(0, 0, halfWidth, height);
        printf("Draw rendererd image one\n");
        
        drawRenderedImage(rendererOne,imagePadding,imagePadding,renderImageWidth - imagePadding * 2,height - imagePadding * 2);
        
        if(showHeatmap) {
            drawHeatmap(imagePadding, heatmapOne, 1,0,0);
        }
        
        if(rendererTwo != NULL) {
            //			glColor3f(0, 0, 1);
            //			glRectf(halfWidth, 0, width, height);
            printf("Draw rendererd image two\n");
            drawRenderedImage(rendererTwo,renderImageWidth,imagePadding,renderImageWidth - imagePadding * 2,height - imagePadding * 2);
            
            if(showHeatmap) {
                drawHeatmap(width / 2.F, heatmapTwo, 0,0,1);
            }
        }
        
        //Draw grid
        if(isDragging) {
            drawGrid(0,paintGridOne);
            if(paintGridTwo) {
                drawGrid(width / 2.F ,paintGridTwo);
            }
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

void RIVImageView::filterImage(RIVDataSet<float,ushort>* dataset, Grid* activeGrid, riv::RowFilter* previousFilter) {
    dataset->StartFiltering();
    
    if(previousFilter) {
        dataset->ClearRowFilter(previousFilter);
        previousFilter = NULL;
    }
    
    unsigned int gridWidth = activeGrid->GetWidth();
    unsigned int gridHeight = activeGrid->GetHeight();
    
    //                float gridToPixelWidth = width / gridWidth;
    //                float gridToPixelHeight = height / gridHeight;
    
    //                riv::CompoundFilter<float>* pixelFilter = new riv::CompoundFilter<float>();

    
    RIVTable<float,ushort>* pathsTable = dataset->GetTable(PATHS_TABLE);
    
    TableIterator* iterator = pathsTable->GetIterator();
    RIVFloatRecord* pixelXs = pathsTable->GetRecord<float>(PIXEL_X);
    RIVFloatRecord* pixelYs = pathsTable->GetRecord<float>(PIXEL_Y);
    
    size_t row;
    std::map<size_t,bool> filteredRows;
    
    while(iterator->GetNext(row)) {
        float pixelX = pixelXs->Value(row);
        float pixelY = pixelYs->Value(row);
        
        int gridX = gridWidth * pixelX;
        int gridY = gridHeight - gridHeight * pixelY;
        
        if(gridX == gridWidth)
            --gridX;
        
        if(gridY == gridHeight)
            --gridY;
        
        if(!activeGrid->IsFilled(gridX, gridY)) {
            filteredRows[row] = true;
        }
    }
    previousFilter = new riv::RowFilter(PATHS_TABLE,filteredRows);
    dataset->AddFilter(previousFilter);
    dataset->StopFiltering();
}

bool RIVImageView::HandleMouse(int button, int state, int x, int y) {
    //    ToViewSpaceCoordinates(&x, &y);
//    return false;
    y = height - y;
    if(isDragging || containsPoint(x,y)) {
        //If start dragging > init selection
        if(state == GLUT_DOWN) {
            //What grid to use?
            isDragging = true;
            showFillArea = true;
            Grid* outGrid = NULL;
            int gridX,gridY;
            toGridSpace(x, y, outGrid, gridX, gridY);
            
            outGrid->Clear();

            outGrid->FillCell(gridX, gridY);
            
            interactingGrid = outGrid;
            
            redisplayWindow();
        }
        else if(state == GLUT_UP) {
            isDragging = false;
            
            if(interactingGrid) {
                //Get bounds
                RIVRectangle boundingRectangle = interactingGrid->fillBounds();
                
                
                RIVPoint seedPoint;
                
                if(boundingRectangle.start.x > 0) {
                    seedPoint.x = 0;
                }
                else if(boundingRectangle.end.x < interactingGrid->GetWidth() - 1) {
                    seedPoint.x = interactingGrid->GetWidth() - 1;
                }
                if(boundingRectangle.start.y > 0) {
                    seedPoint.y = 0;
                }
                else if(boundingRectangle.end.y < interactingGrid->GetHeight() - 1) {
                    seedPoint.y = interactingGrid->GetHeight() - 1;
                }
                interactingGrid->FloodFill(seedPoint);
                interactingGrid->InvertFill();
                
                showFillArea = false;
                
                if(interactingGrid == paintGridOne) {
                    filterImage(*datasetOne, interactingGrid, pixelFilterOne);
                }
                else if(interactingGrid == paintGridTwo) {
                    filterImage(*datasetTwo, interactingGrid, pixelFilterTwo);
                }
                else {
                    //What?
                }
                
                interactingGrid = NULL; //We are done with the grid
                redisplayWindow();
            }
            
        }
        return true;
    }
    else {
        //Clear any possible selection
        //		clearSelection();
        return false;
    }
}
void RIVImageView::toGridSpace(int x, int y, Grid*& outGrid, int& gridX, int& gridY) {
    if(x > width / 2.F) {
        outGrid = paintGridTwo;
        x -= width / 2.F;
    }
    else {
        outGrid = paintGridOne;
    }
    gridY = y / (float)height * outGrid->GetHeight();
    gridX = x / ((float)width / 2.F) * outGrid->GetWidth();
}
bool RIVImageView::HandleMouseMotion(int x, int y) {
    y = height - y;
    if(isDragging) {
        Grid* outGrid = NULL;
        int gridX,gridY;
        toGridSpace(x, y, outGrid, gridX, gridY);
        
        if(outGrid == interactingGrid) {
            outGrid->FillCell(gridX, gridY);
        }
        
        redisplayWindow();
        return true;
    }
    else return false;
}

void RIVImageView::clearSelection() {
    //Set the selection to off
    
    //Clear any filters that may have been applied to the (*dataset)
    
    (*datasetOne)->ClearFilter<ushort>("x");
    (*datasetOne)->ClearFilter<ushort>("y");
    
}