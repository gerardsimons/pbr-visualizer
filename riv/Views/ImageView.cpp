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
    yBins = rendererOne->getWidth() / (float)rendererOne->getHeight() * xBins;
}

RIVImageView::RIVImageView(RIVDataSet<float,ushort>** datasetOne, EMBREERenderer* renderer) : RIVDataView(datasetOne) {
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    identifier = "ImageView";
    paintGridOne = new Grid(gridSize);
    
    rendererOne = renderer;
    yBins = rendererOne->getHeight() / (float)rendererOne->getWidth() * xBins;
}

RIVImageView::RIVImageView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo, Histogram2D<float>* throughputDistroOne,Histogram2D<float>* throughputDistroTwo,Histogram2D<float>* trueEnergyDistributionOne,Histogram2D<float>* trueEnergyDistributionTwo) : RIVDataView(datasetOne,datasetTwo), rendererOne(rendererOne), rendererTwo(rendererTwo), throughputDistroOne(throughputDistroOne), throughputDistroTwo(throughputDistroTwo),trueEnergyDistributionOne(trueEnergyDistributionOne),trueEnergyDistributionTwo(trueEnergyDistributionTwo) {
    
    paintGridOne = new Grid(gridSize);
    paintGridTwo = new Grid(gridSize);
    
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    identifier = "ImageView";
    yBins = rendererOne->getHeight() / (float)rendererOne->getWidth() * xBins;
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
Histogram2D<float> RIVImageView::computeRadianceDistribution(RIVDataSet<float,ushort>* dataset, int xBins, int yBins) {
    Histogram2D<float> result(0,1,xBins,yBins);
    RIVTable<float,ushort>* pathsTable = dataset->GetTable(PATHS_TABLE);
    
    RIVRecord<float>* radianceR = pathsTable->GetRecord<float>(PATH_R);
    RIVRecord<float>* radianceG = pathsTable->GetRecord<float>(PATH_G);
    RIVRecord<float>* radianceB = pathsTable->GetRecord<float>(PATH_B);
    RIVRecord<float>* pixelX = pathsTable->GetRecord<float>(PIXEL_X);
    RIVRecord<float>* pixelY = pathsTable->GetRecord<float>(PIXEL_Y);
    
    TableIterator* iterator = pathsTable->GetIterator();
    size_t row;
    int magnitude = 100;
    
    while(iterator->GetNext(row)) {
        float averageRadiance = (radianceR->Value(row) + radianceG->Value(row) + radianceB->Value(row)) / 3.F;
        result.Add(pixelX->Value(row), pixelY->Value(row), std::round(magnitude*averageRadiance));
    }
    return result;
}
void RIVImageView::computeRadianceDistributions() {
    
//    if(r  adianceDistributionOne)
//        radianceDistributionOne->Clear();
//    if(radianceDistributionTwo)
//        delete radianceDistributionTwo;
//    if(radianceDiffDistribution)
//        delete radianceDiffDistribution;
    
    if(datasetTwo && !(*datasetTwo)->IsEmpty()) {
        
        //TODO: Delete pre-existing distributions if present
        
//        radianceDistributionOne = new Histogram2D<float>(computeRadianceDistribution(*datasetOne, xBins, yBins));
//        radianceDistributionTwo = new Histogram2D<float>(computeRadianceDistribution(*datasetTwo, xBins, yBins));
        if(!radianceDiffDistribution) {
            radianceDiffDistribution = new Histogram2D<float>(*trueEnergyDistributionOne - *trueEnergyDistributionTwo);
        }
        else {
//            radianceDiffDistribution->Clear();
            delete radianceDiffDistribution;
            radianceDiffDistribution = new Histogram2D<float>(*trueEnergyDistributionOne - *trueEnergyDistributionTwo);
        }
    }
    else printf("Second dataset not set or empty.\n");
}
//Change what heatmap to show, 
void RIVImageView::ToggleHeatmapToDisplay() {
    printf("Toggle heatmap to display : ");
    switch (heatmapToDisplay) {
        case DISTRIBUTION:
            heatmapToDisplay = THROUGHPUT;
            printf("THROUGHPUT");
            activeHeatmapOne = &throughputDistroOne;
            activeHeatmapTwo = &throughputDistroTwo;
            break;
        case THROUGHPUT:
            heatmapToDisplay = RADIANCE;
            printf("RADIANCE");
            if(!radianceDiffDistribution) {
                //Create radiance difference distribution
                computeRadianceDistributions();
            }
//            activeHeatmapOne = &radianceDistributionOne;
//            activeHeatmapTwo = &radianceDistributionTwo;
            activeHeatmapOne = &trueEnergyDistributionOne;
            activeHeatmapTwo = &trueEnergyDistributionTwo;
            break;
        case RADIANCE:
            heatmapToDisplay = RADIANCE_DIFFERENCE;
            activeHeatmapOne = &radianceDiffDistribution;
            activeHeatmapTwo = &radianceDiffDistribution;
            printf("RADIANCE_DIFFERENCE");
            break;
        case RADIANCE_DIFFERENCE:
            printf("NONE");
            if((*datasetOne)->IsFiltered() || (datasetTwo && (*datasetTwo)->IsFiltered())) {
                heatmapToDisplay = DISTRIBUTION;
                activeHeatmapOne = &pixelDistributionOne;
                activeHeatmapTwo = &pixelDistributionTwo;
                printf("DISTRIBUTION");
            }
            else {
                heatmapToDisplay = THROUGHPUT;
                activeHeatmapOne = &throughputDistroOne;
                activeHeatmapTwo = &throughputDistroTwo;
                printf("THROUGHPUT");
                
            }
            break;
    }
    printf("\n");
    redisplayWindow();
}
void RIVImageView::ToggleHeatmapDisplayMode() {

    redisplayWindow();
    printf("Heatmap display mode is now ");

    switch (displayMode) {
        case NONE:
            displayMode = OPAQUE;
            printf("OPAQUE");
            break;
        case OPAQUE:
            displayMode = HEAT;
            printf("HEAT");
            break;
        case HEAT:
            printf("NONE");
            displayMode = NONE;
            break;
    }
    printf("\n");
}
void RIVImageView::OnDataChanged(RIVDataSet<float,ushort>* source) {
    
    if(source->IsFiltered()) {
        if(source == *datasetOne) {
            computePixelDistribution(*datasetOne, pixelDistributionOne);
            printf("Pixel distribution one = \n");
            pixelDistributionOne->Print();
        }
        else if(datasetTwo && *datasetTwo == source) {
            computePixelDistribution(*datasetTwo, pixelDistributionTwo);

        }
        else {
            throw std::runtime_error("No such dataset");
        }
        redisplayWindow();
    }
}

void RIVImageView::OnFiltersChanged(RIVDataSet<float,ushort>* dataset) {
    if(dataset == *datasetOne) {
        computePixelDistribution(*datasetOne, pixelDistributionOne);
        computeRadianceDistributions();
//        printf("Heatmap one result = \n");
//        pixelDistributionOne->Print();
    }
    else if(datasetTwo && dataset == *datasetTwo) {
        computePixelDistribution(*datasetTwo, pixelDistributionTwo);
        computeRadianceDistributions();
//        printf("Heatmap two result = \n");
//        heatmapTwo->Print();
    }
    else {
        throw std::runtime_error("Unknown dataset");
    }

    int currentWindow = glutGetWindow();
    glutSetWindow(RIVImageView::windowHandle);
    glutPostRedisplay();
    //Return window to given window
    glutSetWindow(currentWindow);
}
void RIVImageView::ClearPixelDistributionOne() {
    if(pixelDistributionOne) {
        delete pixelDistributionOne;
        pixelDistributionOne = NULL;
    }
}
void RIVImageView::ClearPixelDistributionTwo() {
    if(pixelDistributionTwo) {
        delete pixelDistributionTwo;
        pixelDistributionTwo = NULL;
    }
}
void RIVImageView::smoothPixelDistribution(Histogram2D<float>* pixeLDistribution) {
    pixeLDistribution->SmoothRectangular(3, 3);
    redisplayWindow();
}
void RIVImageView::computePixelDistribution(RIVDataSet<float,ushort>* dataset, Histogram2D<float>*& pixelDistribution) {
    if(pixelDistribution) {
        delete pixelDistribution;
    }
    pixelDistribution = new Histogram2D<float>(0, 1, xBins, yBins);
    
    RIVTable<float,ushort>* pathsTable = dataset->GetTable(PATHS_TABLE);
    TableIterator* iterator = pathsTable->GetIterator();
    
    RIVRecord<float>* xPixels = pathsTable->GetRecord<float>(PIXEL_X);
    RIVRecord<float>* yPixels = pathsTable->GetRecord<float>(PIXEL_Y);
    RIVRecord<float>* throughputsR = pathsTable->GetRecord<float>(THROUGHPUT_R);
    RIVRecord<float>* throughputsG = pathsTable->GetRecord<float>(THROUGHPUT_G);
    RIVRecord<float>* throughputsB = pathsTable->GetRecord<float>(THROUGHPUT_B);
    RIVRecord<ushort>* depths = pathsTable->GetRecord<ushort>(DEPTH);
    
    size_t row;
    while(iterator->GetNext(row)) {
        float throughputR = throughputsR->Value(row);
        float throughputG = throughputsG->Value(row);
        float throughputB = throughputsB->Value(row);
        ushort depth = depths->Value(row);
        
//        printf("throughput (r,g,b) = (%f,%f,%f)\n",throughputR,throughputG,throughputB);
        float averageThroughput = 0;
        if(depth) {
            averageThroughput = (throughputR + throughputG + throughputB) / 3.F * depth;
        }
//        printf("averageThroughput = %f\n",averageThroughput);
//        size_t magnitude = 1000 * averageThroughput;
        float xPixel = xPixels->Value(row);
        float yPixel = yPixels->Value(row);
//        pixelDistribution->Add(xPixel,yPixel,1);
        pixelDistribution->Add(xPixel,yPixel,1);
    }
    
//    pixelDistribution->Add(0.5F, 0.5F,90);
    
//    heatmap.Print();
}
void RIVImageView::drawRegularHeatmap(int startX, Histogram2D<float>* heatmap, riv::ColorMap& colors) {
    if(heatmap && heatmap->NumberOfElements()) {
        
        std::pair<unsigned int, unsigned int> binBounds = heatmap->NumberOfBins();
        unsigned int xBins = binBounds.first;
        unsigned int yBins = binBounds.second;
        float binWidth;
        if(datasetTwo) {
            binWidth = ((width / 2.F) - 2 * imagePadding) / (float)xBins;
        }
        else {
            binWidth = ((width) - 2 * imagePadding) / (float)xBins;
        }
        float binHeight = ((height) - 2 * imagePadding) / (float)yBins;
        
        float binX = startX;
        
        float maxValue = heatmap->MaxBinValue();
        
        //        heatmap->Print();
        for(int x = 0 ; x < xBins ; ++x) {
            float binY = height - imagePadding;
            for(int y = 0 ; y < yBins ; ++y) {
                float normalizedValue = heatmap->BinValue(x, y);
                float ratio = normalizedValue / maxValue;
                
                riv::Color color = colors.ComputeColor(ratio);
                
                glColor3f(color.R, color.G, color.B);
                if(normalizedValue > 0.0001) {
                    glBegin(GL_LINE_LOOP);
                    glVertex2f(binX, binY);
                    glVertex2f(binX + binWidth, binY);
                    glVertex2f(binX + binWidth,binY - binHeight);
                    glVertex2f(binX,binY - binHeight);
                    glEnd();
                }
                glColor4f(color.R, color.G, color.B,color.A);
                glRectf(binX, binY, binX + binWidth, binY - binHeight);
                //                }
                //                printf("glRectf(%f,%f,%f,%f)\n",binX, binY, binX + binWidth, binY + binHeight);
                
                //            }
                
                binY -= binHeight;
            }
            binX += binWidth;
        }
    }
    //    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
}
void RIVImageView::drawNormalizedHeatmap(int startX, Histogram2D<float>* heatmap, riv::ColorMap& colors) {
    if(heatmap && heatmap->NumberOfElements()) {
        
        bool scale = false;
        
        std::pair<unsigned int, unsigned int> binBounds = heatmap->NumberOfBins();
        unsigned int xBins = binBounds.first;
        unsigned int yBins = binBounds.second;
        float binWidth;
        if(datasetTwo) {
            binWidth = ((width / 2.F) - 2 * imagePadding) / (float)xBins;
        }
        else {
            binWidth = ((width) - 2 * imagePadding) / (float)xBins;
        }
        float binHeight = ((height) - 2 * imagePadding) / (float)yBins;
        
        float binX = startX;
        
        size_t nrElements = heatmap->NumberOfElements();
        float maxNormalizedValue = 0;
        
        if(nrElements) {
            maxNormalizedValue = heatmap->MaxBinValue() / (float)nrElements;
        }

        float variance = std::pow(heatmap->NormalizedVariance(),1);//Normalized for the number of bins
        float mean = heatmap->NormalizedMean();
        
//        heatmap->Print();
        for(int x = 0 ; x < xBins ; ++x) {
            float binY = height - imagePadding;
            for(int y = 0 ; y < yBins ; ++y) {
                float normalizedValue = heatmap->NormalizedValue(x, y);
                float ratio = normalizedValue / maxNormalizedValue;

                    riv::Color color = colors.ComputeColor(ratio);
                    
                    glColor3f(color.R, color.G, color.B);
                    if(normalizedValue > 0.0001) {
                        glBegin(GL_LINE_LOOP);
                        glVertex2f(binX, binY);
                        glVertex2f(binX + binWidth, binY);
                        glVertex2f(binX + binWidth,binY - binHeight);
                        glVertex2f(binX,binY - binHeight);
                        glEnd();
                    }
                    glColor4f(color.R, color.G, color.B,color.A);
                    glRectf(binX, binY, binX + binWidth, binY - binHeight);
//                }
                //                printf("glRectf(%f,%f,%f,%f)\n",binX, binY, binX + binWidth, binY + binHeight);
                
                //            }
                
                binY -= binHeight;
            }
            binX += binWidth;
        }
    }
    //    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
}
Histogram2D<float>* RIVImageView::GetPixelDistributionOne() {
    return pixelDistributionOne;
}
Histogram2D<float>* RIVImageView::GetPixelDistributionTwo() {
    return pixelDistributionTwo;
}
Histogram2D<float>* RIVImageView::GetActiveDistributionOne() {
    if(activeHeatmapOne && *activeHeatmapOne) {
        if(heatmapToDisplay == RADIANCE_DIFFERENCE) {
            return *activeHeatmapOne;
        }
    }
    if(datasetTwo && (*datasetTwo)->IsFiltered()) {
        return pixelDistributionOne;
    }
    return NULL;
}
Histogram2D<float>* RIVImageView::GetActiveDistributionTwo() {
    if(activeHeatmapTwo && *activeHeatmapTwo) {
        if(heatmapToDisplay == RADIANCE_DIFFERENCE) {
            return *activeHeatmapTwo;
        }
    }
    if(datasetTwo && (*datasetTwo)->IsFiltered()) {
        return pixelDistributionTwo;
    }
    return NULL;
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
    glRasterPos2i(1+startX, imageHeight+startY);
    
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
std::string RIVImageView::enumToString(RIVImageView::HeatmapDisplayMode displayMode) {
    switch (displayMode) {
        case NONE:
            return "NONE";
            break;
        case OPAQUE:
            return "OPAQUE";
        case HEAT:
            return "HEAT";
        default:
            return "<UNKNOWN HEATMAPDISPLAYMODE>";
    }
}
std::string RIVImageView::enumToString(RIVImageView::HeatmapDisplay displayMode) {
    switch (displayMode) {
        case DISTRIBUTION:
            return "DISTRIBUTION";
            break;
        case THROUGHPUT:
            return "THROUGHPUT";
            break;
        case RADIANCE:
            return "RADIANCE";
            break;
        case RADIANCE_DIFFERENCE:
           return "RADIANCE_DIFFERENCE";
            break;
    }
}
void RIVImageView::drawHeatmap(bool leftSet, Histogram2D<float>** heatmap) {
//    bool filteredOne = (*datasetOne)->IsFiltered(); //Only draw the heatmaps of pixels when not filtered
    int startX = imagePadding;
    if(!leftSet) {
        startX += width / 2.F;
    }
    if(displayMode != NONE) {
        if(heatmap == NULL || *heatmap == NULL) {
            printf("Heatmap is NULL\n");
            return;
        }
        printf("DRAW HEATMAP: \n");
        printf("MODE = %s\n",enumToString(displayMode).c_str());
        printf("HEATMAP TYPE = %s\n",enumToString(heatmapToDisplay).c_str());
//        printf("HEATMAP = \n");
//        heatmap->Print();
        
        riv::ColorMap colors;
        if(displayMode == OPAQUE) {
            
            if(leftSet) {
                colors.AddColor(riv::Color(1,0,0,0));
                colors.AddColor(riv::Color(1,0,0,1));
            }
            else {
                colors.AddColor(riv::Color(0,0,1,0));
                colors.AddColor(riv::Color(0,0,1,1));
            }
        }
        else if(displayMode == HEAT){
            colors = colors::hotBodyColorMap();
        }
        else {
            return;
        }
        
        
        if(heatmapToDisplay == RADIANCE || heatmapToDisplay == RADIANCE_DIFFERENCE) {
            drawRegularHeatmap(startX, *heatmap, colors);
        }
        else {
            //Only draw distribution when filtered
            if(heatmapToDisplay == DISTRIBUTION) {
                RIVDataSet<float,ushort>* activeSet;
                if(leftSet) {
                    activeSet = *datasetOne;
                }
                else {
                    activeSet = *datasetTwo;
                }
                if(activeSet->IsFiltered()) {
                    drawNormalizedHeatmap(startX, *heatmap, colors);
                }
            }
            else {
                drawNormalizedHeatmap(startX, *heatmap, colors);
            }
        }
    }
    else {
        printf("Heatmap to display is NONE\n");
    }
}

size_t drawCounter = 0;
void RIVImageView::Draw() {
    needsRedraw = true;
    
    if(needsRedraw) {
        printHeader("IMAGE VIEW DRAW");
        //		printf("\nImageView Draw #%zu\n",++drawCounter);
        glDisable(GL_DEPTH_TEST);
        
        glClearColor(0,1,1,1);
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
        
//        heatmapToDisplay = DISTRIBUTION;
        glColor3f(1,0,0);
        glRectf(0, 0, renderImageWidth, height);
        
        glColor3f(0,0,0);
        glRectf(imagePadding, imagePadding, renderImageWidth - imagePadding, height - imagePadding);
        drawRenderedImage(rendererOne,imagePadding,imagePadding,renderImageWidth - 2 * imagePadding,height - 2 * imagePadding);
        
        if(datasetOne && *datasetOne && !(*datasetOne)->IsEmpty()) {
            drawHeatmap(true, activeHeatmapOne);
        }
        
        if(rendererTwo != NULL) {
            //			glColor3f(0, 0, 1);
            //			glRectf(halfWidth, 0, width, height);
            printf("Draw rendererd image two\n");
            glColor3f(0,0,1);
            glRectf(renderImageWidth, 0, 2 * renderImageWidth, height);
            drawRenderedImage(rendererTwo,renderImageWidth+imagePadding,imagePadding,renderImageWidth - imagePadding * 2,height - imagePadding * 2);
            if(datasetTwo && *datasetTwo && !(*datasetTwo)->IsEmpty()) {
                drawHeatmap(false, activeHeatmapTwo);
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
void RIVImageView::WeightPixelDistributionByThroughput() {
    if(pixelDistributionOne && pixelDistributionOne->NumberOfElements() && throughputDistroOne && throughputDistroOne->NumberOfElements()) {
        printf("Weighing pixel distribution by throughput\n");
        *pixelDistributionOne = *pixelDistributionOne + *throughputDistroOne;
        redisplayWindow();
    }
    if(pixelDistributionTwo && pixelDistributionTwo->NumberOfElements() && throughputDistroTwo && throughputDistroTwo->NumberOfElements()) {
        printf("Weighing pixel distribution two by throughput\n");
        pixelDistributionTwo->Print();
        printf("Throughput distro two");
        throughputDistroTwo->Print();
//        unsigned int minPixel = pixelDistributionTwo->MinBinValue();
//        unsigned int minThroughput = throughputDistroTwo->MinBinValue();
//        unsigned int min = std::min(minPixel,minThroughput);
        auto bins = pixelDistributionTwo->NumberOfBins();
        Histogram2D<float> result(0,1,bins.first,bins.second);
        for(int xBin = 0 ; xBin < bins.first ; ++xBin) {
            for(int yBin = 0 ; yBin < bins.second ; ++yBin ) {
                
//                float product = pixelDistributionTwo->BinValue(xBin, yBin) * throughputDistroTwo->BinValue(xBin, yBin);
                unsigned int pixelValue = pixelDistributionTwo->BinValue(xBin, yBin);
                unsigned int throughputValue = throughputDistroTwo->BinValue(xBin, yBin);
                unsigned int product = pixelValue * throughputValue;
                printf("distro * throughput = %d * %d = %d\n",pixelValue,throughputValue,product);
                result.SetBinValue(xBin, yBin,product);

            }
        }
        *pixelDistributionTwo = result;
        redisplayWindow();
    }
}
void RIVImageView::CombinePixelDistributions() {
    
    if(pixelDistributionOne && pixelDistributionTwo) {
    
        Histogram2D<float> sum = *pixelDistributionOne + *pixelDistributionTwo;
        
        printf("pixelDistributionOne = \n");
        pixelDistributionOne->Print();
        printf("pixelDistributionTwo = \n");
        pixelDistributionTwo->Print();
        printf("sum = \n");
        sum.Print();
        
        *pixelDistributionOne = sum;
        *pixelDistributionTwo = sum;
        
        redisplayWindow();
        
        printf("\nMerged pixel distributions...\n");
    }
}
void RIVImageView::clearSelection() {
    //Set the selection to off
    
    //Clear any filters that may have been applied to the (*dataset)
    
    (*datasetOne)->ClearFilter<ushort>("x");
    (*datasetOne)->ClearFilter<ushort>("y");
    
}
void RIVImageView::SmoothPixelDistributionOne() {
//    if(datasetOne && pixelDistributionOne && pixelDistributionOne->NumberOfElements()) {
//        smoothPixelDistribution(pixelDistributionOne);
//    }
    if(datasetOne && activeHeatmapOne && *activeHeatmapOne && (*activeHeatmapOne)->NumberOfElements()) {
        smoothPixelDistribution(*activeHeatmapOne);
    }
}
void RIVImageView::SmoothPixelDistributionTwo() {
//    if(datasetTwo && pixelDistributionTwo && pixelDistributionTwo->NumberOfElements()) {
//        smoothPixelDistribution(pixelDistributionTwo);
//    }
    if(datasetOne && activeHeatmapTwo && *activeHeatmapTwo && (*activeHeatmapTwo)->NumberOfElements()) {
        smoothPixelDistribution(*activeHeatmapTwo);
    }
}