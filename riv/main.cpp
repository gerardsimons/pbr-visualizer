#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <exception>

#include "Configuration.h"
#include "Data/Filter.h"
#include "Views/DataView.h"
#include "Views/ParallelCoordsView.h"
#include "Views/UIView.h"
#include "Data/DataSet.h"
#include "Views/ImageView.h"
#include "Views/3DView.h"
#include "Views/SliderView.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/ColorProperty.h"
#include "Graphics/SizeProperty.h"
#include "DataController.h"
#include "Views/HeatMapView.h"
#include "Octree/Octree.h"

//EMBREE STUFF
#include "devices/device_singleray/embree_renderer.h"
#include "devices/device_singleray/dataconnector.h"
//#include "devices/device_singleray_two/embree_renderer_two.h"
//#include "sandbox.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define USE_IMAGEMAGICK

/* window width and height */
//int width = 1650
//int height = 1000;

int padding = 10;

int width = 1650;
int height = 850;

//Resolution of the render
int rendererResolutionX = 256;
int rendererResolutionY = 256;
int spp = 1; //Samples per pixel

bool isDirty = true;

/* window position */
int posX = 0;
int posY = 0;

/* GLUT window handle */
int mainWindow;

/* All the sub window handles of the custom views */
int imageViewWindow;
int sceneViewWindow;
int parallelViewWindow;
int heatMapViewWindow;
int sliderViewWindow;
int uiViewWindow;

/* For debugging purposes, keep track of mouse location */
int lastMouseX,lastMouseY = 0;

RIVImageView *imageView = NULL;
RIV3DView *sceneView = NULL;
ParallelCoordsView *parallelCoordsView = NULL;
RIVHeatMapView *heatMapView = NULL;
RIVSliderView* sliderView = NULL;
RIVUIView* uiView = NULL;

RIVDataSet<float,ushort>** datasetOne = NULL;
RIVDataSet<float,ushort>** datasetTwo = NULL;

/* The dataset, views have pointers to this in order to draw their views consistently */
DataController* dataControllerOne;
DataController* dataControllerTwo = NULL; //It is possible this one will not be used
EMBREERenderer* rendererOne = NULL;
EMBREERenderer* rendererTwo = NULL;

const int maxPaths = 10000;
const int bootstrapRepeat = 5;
const int sliderViewHeight = 50;

bool connectedOne = false;
bool connectedTwo = false;

int currentFrameOne = 0;
int currentFrameTwo = 0;

clock_t startDelay;
bool isDelayed = false;

bool renderingPausedOne = false;
bool renderingPausedTwo = true;
int delayTimeLeft = 0;
int delayTimerInterval = 0;

bool renderOneFinishedFrame = true;
bool renderTwoFinishedFrame = true;

void TogglePause();
void TogglePauseOne();
void TogglePauseTwo();

void display(void)
{
    printf("Main display function called.\n");
    // Clear frame buffer
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //	dataController->Unpause();
}

void invalidateAllViews() {
    sceneView->Invalidate();
    parallelCoordsView->Invalidate();
    parallelCoordsView->redisplayWindow();
    imageView->Invalidate();
    //	imageView->redisplayWindow();
    //	uiView->Invalidate();
    
    int previousWindow = glutGetWindow();
    glutSetWindow(mainWindow);
    glutPostRedisplay();
    glutSetWindow(previousWindow);
}
void testOctree() {
    Octree octree(2,0,0,0,1,1);
    octree.Add(-0.25F, -0.25F, -0.25F, 1);
    octree.Add(-0.25F, -0.25F, 0.25F, 1);
    
    OctreeNode* root = octree.GetRoot();
    printf("Aggregate value root = %f\n",root->AggregateValue());
}
void testSampling() {
    RIVDataSet<float,ushort>* testData = new RIVDataSet<float,ushort>("test_set");
    
    RIVTable<float,ushort>* testTable = testData->CreateTable("test_table");
    
    RIVRecord<float>* floatRecord = testTable->CreateRecord<float>("floats");
    RIVRecord<float>* floatRecordTwo = testTable->CreateRecord<float>("floats_two");
    
    //	RIVMultiReference* multiRef = new RIVMultiReference();
    //	RIVSingleReference* singleRef = new RIVSingleReference();
    
    Histogram<float> testHistogram = Histogram<float>("test",0,10,10);
    Histogram<int> testHistogramInt = Histogram<int>("test",0,10,10);
    
    HistogramSet<float,int> testHistogramSet;
    Histogram2D<float> floatHist2D(0,10,2);
    
    const size_t N = 10;
    for(int i = 0 ; i < N ; ++i) {
        size_t M = 10 * (i+1);
        for(int j = 0 ; j < M ; ++j) {
            floatRecord->AddValue(rand());
            floatRecordTwo->AddValue(N-i);
            testHistogramInt.Add(i);
            testHistogram.Add(rand() / RAND_MAX);
        }
    }
    testHistogramSet.AddHistogram(testHistogram);
    testHistogramSet.AddHistogram(testHistogramInt);
    
    testHistogramSet.Clear();
    //    floatHist2D.Add(2, 2);
    floatHist2D.Add(7, 2);
    floatHist2D.Add(2, 7);
    floatHist2D.Add(7, 7);
    floatHist2D.Add(7, 7);
    
    //    for(int x = 0 ; x < 10 ; x++) {
    //        for(int y = 0 ; y < 10 ; y++) {
    //            size_t M = 10 * ((x+1)*(y+1));
    //            for(int j = 0 ; j < M ; ++j) {
    //                floatHist2D.Add(x, y);
    //            }
    //        }
    //    }
    
    Histogram<float> floatHist = floatRecord->CreateHistogram(0,10,10);
    
    floatHist.Print();
    
    //Sample N new values
    Histogram<float> sampledHist = Histogram<float>("sampled",0,10,10);
    
    for(int i = 0 ; i < 450 ; ++i) {
        sampledHist.Add(floatHist.Sample());
    }
    
    sampledHist.Print();
    floatHist2D.Print();
    
    //    sampledHist.Clear();
    
    Histogram2D<float> sampledHist2D = Histogram2D<float>(0,10,2);
    for(int i = 0 ; i < 1000000 ; ++i) {
        auto sample = floatHist2D.Sample2D();
        
        //        printf("Sample %f,%f\n",sample.first,sample.second);
        //        sampledHist.Add(sample.first);
        sampledHist2D.Add(sample);
    }
    //    auto histogramset = testData->CreateHistogramSet(10);
    sampledHist2D.Print();
    //    sampledHist.Print();
    
    delete testData;
}

void testFunctions() {
    
    //Done
    testOctree();
    return;
    
    //Test bootstrap
    const int upperBound = 100;
    int bins = 4;
    
    HistogramSet<int> trueHistograms;
    
    Histogram<int> trueHistogram("one",0,upperBound,bins);
    Histogram<int> trueHistogramTwo("two",0,upperBound,bins);
    
    int size = 1;
    int sizeIncrement = 0;
    for(int j = 0 ; j < upperBound ; ++j) {
        for(int i = 0 ; i < 100*size ; ++i) {
            int bin = trueHistogram.Add(j);
            trueHistogramTwo.Add(j);
            //			printf("Added %d to bin %d\n",j,bin);
        }
        size += sizeIncrement;
    }
    
    trueHistograms.AddHistogram(trueHistogram);
    trueHistograms.AddHistogram(trueHistogramTwo);
    trueHistograms.Print();
    
    HistogramSet<int> bootstrapHistograms;
    
    Histogram<int> bootstrapHistogram("one",0,upperBound,bins);
    Histogram<int> bootstrapHistogramTwo("two",0,upperBound,bins);
    
    for(int j = 0 ; j < upperBound ; ++j) {
        for(int i = 0 ; i < size ; ++i) {
            bootstrapHistogram.Add(1);
            bootstrapHistogramTwo.Add(1);
        }
        for(int i = 0 ; i < size ; ++i) {
            bootstrapHistogram.Add(76);
            //			bootstrapHistogramTwo.Add(1);
        }
    }
    
    bootstrapHistogram.Print();
    
    bootstrapHistograms.AddHistogram(bootstrapHistogram);
    bootstrapHistograms.AddHistogram(bootstrapHistogramTwo);
    
    bootstrapHistogram.Print();
    //	float score = bootstrapHistogram.DistanceTo(&trueHistogram);
    float score = bootstrapHistograms.DistanceTo(trueHistograms);
    
    printf("test score = %f\n",score);
    
    exit(0);
}

void keys(int keyCode, int x, int y) {
    //    printf("Pressed %d at (%d,%d)\n",keyCode,x,y);
    bool postRedisplay = true;
    
    char key = (char)keyCode;
    printf("'%c' key pressed.\n",key);
    
    float camSpeed = .25F;
    switch(keyCode) {
        case 27: //ESC key
            printf("Clear filters\n");
            //            invalidateAllViews();
            //			(*datasetOne)->StartFiltering();
            //            (*datasetOne)->ClearFilters();
            //			datasetOne)->StopFiltering();
            //
            (*datasetOne)->StartFiltering();
            (*datasetOne)->ClearAllFilters();
            (*datasetOne)->StopFiltering();
            
            (*datasetTwo)->StartFiltering();
            (*datasetTwo)->ClearAllFilters();
            (*datasetTwo)->StopFiltering();
            
            invalidateAllViews();
            
            break;
        case 45: // - key
            parallelCoordsView->DecreaseLineOpacity();
            break;
        case 43: // + key
            parallelCoordsView->IncreaseLineOpacity();
            break;
        case 44:
            sceneView->DecrementHeatmapDepth();
            break;
        case 46:
            sceneView->IncrementHeatmapDepth();
            break;
        case 61: // = key is on the same physical keyboard button as +, so cut the user some slack
            parallelCoordsView->IncreaseLineOpacity();
            break;
        case 32: //Space bar
            TogglePause();
            break;
        case 49: //The '1' key, switch to renderer one if not already using it
            parallelCoordsView->ToggleDrawDataSetOne();
            sceneView->ToggleDrawDataSetOne();
            break;
        case 50: //The '2' key, switch to renderer two if not already using it
            parallelCoordsView->ToggleDrawDataSetTwo();
            sceneView->ToggleDrawDataSetTwo();
            break;
        case 51: // the '3' key, pause renderer 1
            if(datasetOne) {
                TogglePauseOne();
            }
            break;
        case 52: // the '4' key, pause renderer 2
            if(datasetTwo) {
                TogglePauseTwo();
            }
            break;
        case 53: // the '5' key, copy 1 to 2
            if(datasetTwo && datasetOne) {
                printf("Copying swapchain from renderer 1 to renderer 2!!\n");
                rendererOne->CopySwapChainTo(rendererTwo);
                imageView->redisplayWindow();
            }
            break;
        case 97: // 'a' key
            sceneView->MoveCamera(-camSpeed, 0, 0);
            break;
        case 98: // 'b' key
//            glutSwapBuffers();
            printf("Manual swap buffers\n");
            imageView->redisplayWindow();
            //            copy_buffer();
            postRedisplay = true;
            break;
        case 99: // 'c' key
            if(sceneView) {
                sceneView->ToggleDrawIntersectionPoints();
                postRedisplay = true;
            }
            break;
        case 108 : // 'l' key, toggle lines drawing
            sceneView->CyclePathSegment();
            break;
        case 91: // '[' key, increase path segment
            sceneView->MovePathSegment(-.01F);
            break;
        case 93:
            sceneView->MovePathSegment(.01F);
            break;
        case 104: // the 'h' from heatmap, toggle drawing the octree heatmap
            sceneView->ToggleDrawHeatmap();
            break;
        case 106: // the 'j' key, cause the h is taken for the other heatmap
            imageView->ToggleShowHeatmap();
            break;
        case 109:
            sceneView->ToggleHideMesh();
            break;
        case 112: //The 'p' key, toggle drawing paths in 3D view
            sceneView->ToggleDrawPaths();
            break;
        case 113:
            exit(0);
        case 114: // 'r' key, force next frame rendering {
        {
            rendererOne->RenderNextFrame();
            if(rendererTwo)
                rendererTwo->RenderNextFrame();
            postRedisplay = true;
            break;
        }
        case 111: // 'o' key, output render images
        {
            if(rendererOne) {
                rendererOne->outputMode("renderer1_output_frame=" + std::to_string(currentFrameOne) + ".bmp");
            }
            if(rendererTwo) {
                rendererTwo->outputMode("renderer2_output_frame=" + std::to_string(currentFrameOne) + ".bmp");
            }
            postRedisplay = true;
            break;
        }
        case 122: // 'z' key, save the image
        {
            parallelCoordsView->ToggleSaturationMode();
            break;
        }
        case 116: // 't' key, use as temp key for some to-test function
        {
            invalidateAllViews();
            glutPostRedisplay();
            break;
        }
        case 119: // 'w' key, move camera in Y direction
            //            sceneView->MoveCamera(0,camSpeed,0);
            break;
        case 115: // 's' key
            //            sceneView->MoveCamera(0, -camSpeed, 0);
            
            break;
        case GLUT_KEY_UP:
            //            sceneView->MoveCamera(0,0,camSpeed);
            sceneView->ZoomIn(camSpeed);
            break;
        case GLUT_KEY_DOWN:
            //            sceneView->MoveCamera(0,0,-camSpeed);
            sceneView->ZoomIn(-camSpeed);
            break;
        case GLUT_KEY_LEFT:
            //            sceneView->MoveCamera( camSpeed,0,0);
            break;
        case GLUT_KEY_RIGHT:
            //            sceneView->MoveCamera(-camSpeed,0,0);
            break;
        default:
            postRedisplay = false;
    }
    if(postRedisplay) {
        glutPostRedisplay();
    }
}

/* Called when window is resized,
 also when window is first created,
 before the first call to display(). */
void reshape(int w, int h)
{
    printf("MAIN reshape called.\n");
    /* save new screen dimensions */
    width = w;
    height = h;
    
    //Reshape and reposition all windows according to new dimensions
    
    //Parallel view window
    if(connectedOne || connectedTwo) {
        glutSetWindow(parallelViewWindow);
        glutPositionWindow(padding,padding);
        int newWidthPC = width-2*padding;
        int newHeightPC = height/2-2*padding - sliderViewHeight / 2.F;
        glutReshapeWindow(newWidthPC,newHeightPC); //Upper half and full width of the main window
        //
        //    //image view window
        float bottomHalfY = height / 2.f + padding + sliderViewHeight / 2.F;
        float squareSize = height / 2.F - 2 * padding - sliderViewHeight / 2.F;
        float ratio = rendererOne->getWidth() / (float)rendererOne->getHeight();
        float imageViewWidth = squareSize * ratio;
        if(datasetTwo) {
            imageViewWidth += imageViewWidth;
        }
        glutSetWindow(imageViewWindow);
        //        glutInitDisplayMode(GLUT_SINGLE);
        glutPositionWindow(padding, height/2+padding + sliderViewHeight / 2.F);
        glutReshapeWindow(imageViewWidth,squareSize); //Square bottom left corner
        
        if(sliderView) {
            //            glutSetWindow(sliderViewWindow);
            //            glut
            //            glutReshapeWindow(<#int width#>, <#int height#>)
        }
    }
    else {
        glutSetWindow(imageViewWindow);
        glutPositionWindow(0, 0);
        
        EMBREERenderer* activeRenderer = rendererOne;
        if(!activeRenderer) {
            activeRenderer = rendererTwo;
        }
        
        glutReshapeWindow(width,(float)rendererOne->getHeight() / rendererOne->getWidth() * width); //Square bottom left corner
    }
    
}

//Delay rendering
void delayRendering(size_t delayTimeMs) {
    printf("Delay rendering....\n");
    startDelay = clock();
    delayTimerInterval = delayTimeMs;
    delayTimeLeft = delayTimeMs;
}
void checkIsDelayed() {
    if(isDelayed) {
        int currentTime = clock();
        int timeDelayed = (currentTime - startDelay) / (float)CLOCKS_PER_SEC * 1000;
        delayTimeLeft -= timeDelayed;
        if(delayTimeLeft <= 0) {
            isDelayed = false;
        }
        else {
            printf("delayTimeLeft = %d\n",delayTimeLeft);
            isDelayed = true;
        }
    }
}
void TogglePauseOne() {
    printf("Rendering process ONE is now ");
    if(renderingPausedOne) {
        
        printf("running.\n");
        renderingPausedOne = false;
        
        //If dataset one has some filters the rendering will be guided, which means the true distributions
        //should be reset in order to speed up convergence
        if(datasetOne && (*datasetOne)->IsFiltered()) {
            dataControllerOne->Reset();
        }
        
        
    }
    else {
        printf("paused.\n");
        renderingPausedOne = true;
    }
    //    glutPostRedisplay();
}
void TogglePauseTwo() {
    printf("Rendering process TWO is now ");
    if(renderingPausedTwo) {
        printf("running.\n");
        renderingPausedTwo = false;
        
        //If dataset one has some filters the rendering will be guided, which means the true distributions
        //should be reset in order to speed up convergence
        if(datasetTwo && (*datasetTwo)->IsFiltered()) {
            dataControllerTwo->Reset();
        }
        
        //        glutPostRedisplay();
    }
    else {
        printf("paused.\n");
        renderingPausedTwo = true;
    }
}
void TogglePause() {
    if(!renderingPausedOne) {
        if(!renderingPausedTwo) {
            renderingPausedOne = true;
            renderingPausedTwo = true;
        }
        else {
            renderingPausedOne = true;
        }
    }
    else if(renderingPausedTwo) {
        renderingPausedOne = false;
        renderingPausedTwo = false;
    }
    else {
        renderingPausedTwo = true;
    }
}
void idle() {
    bool postRedisplay = false;
    
    checkIsDelayed();
    
    if(!renderingPausedOne) {
        printf("Rendering frame %d\n",currentFrameOne);
        ++currentFrameOne;
        Histogram2D<float>* heatmapOne = imageView->GetHeatmapOne();
        if(heatmapOne && heatmapOne->NumberOfElements() && (*datasetOne)->IsFiltered()) {
            //                heatmapOne->Print();
            rendererOne->RenderNextFrame(heatmapOne);
        }
        else {
            rendererOne->RenderNextFrame();
        }
        renderOneFinishedFrame = false;
        postRedisplay = true;
    }
    if(!renderingPausedTwo) {
        if(dataControllerTwo) {
            ++currentFrameTwo;
            if((*datasetTwo)->IsFiltered()) {
                auto heatmapTwo = imageView->GetHeatmapTwo();
                //                    heatmapTwo->Print();
                rendererTwo->RenderNextFrame(heatmapTwo);
            }
            else {
                rendererTwo->RenderNextFrame();
            }
            renderTwoFinishedFrame = false;
        }
        postRedisplay = true;
    }
    
    if(postRedisplay) {
        glutPostRedisplay();
    }
}

bool processRendererOne(PathData* newPath) {
    //	printf("New path from renderer #1 received!\n");
    return dataControllerOne->ProcessNewPath(currentFrameOne,newPath);
}

void rendererOneFinishedFrame(size_t numPaths, size_t numRays) {
    //	dataControllerTwo->RendererOneFinishedFrame(numPaths,numRays);
    printf("Renderer one finished a frame...\n");
    dataControllerOne->Reduce();
    imageView->redisplayWindow();
    renderOneFinishedFrame = true;
    //	if(renderOneFinishedFrame && renderTwoFinishedFrame) {
    //		delayRendering(5000);
    
    //		renderingPaused = true;
    //	}
    //    renderingPausedOne = true;
}

bool processRendererTwo(PathData* newPath) {
    //	printf("New path from renderer #2 received!\n");
    return dataControllerTwo->ProcessNewPath(currentFrameTwo,newPath);
}
void rendererTwoFinishedFrame(size_t numPaths, size_t numRays) {
    printf("Renderer two finished a frame...\n");
    dataControllerTwo->Reduce();
    imageView->redisplayWindow();
    renderTwoFinishedFrame = true;
//    renderingPausedTwo = true;
}
TriangleMeshGroup getSceneData(EMBREERenderer* renderer) {
    //Get the shapes and see what are trianglemeshes that we can draw
    std::vector<Shape*>* shapes = renderer->GetShapes();
    std::vector<TriangleMeshFull*> embreeMeshes;
    for(size_t i = 0 ; i < shapes->size() ; ++i) {
        TriangleMeshFull* t = dynamic_cast<TriangleMeshFull*>(shapes->at(i));
        if(t) {
            embreeMeshes.push_back(t);
        }
    }
    return TriangleMeshGroup(embreeMeshes);
}

//Helper functions to create color property for a given dataset
RIVColorRGBProperty<float>* createPathColorProperty(RIVDataSet<float,ushort>* dataset) {
    RIVTable<float,ushort> *pathsTable = (*datasetOne)->GetTable(PATHS_TABLE);
    
    RIVRecord<float>* pathRRecord = pathsTable->GetRecord<float>(PATH_R);
    RIVRecord<float>* pathGRecord = pathsTable->GetRecord<float>(PATH_G);
    RIVRecord<float>* pathBRecord = pathsTable->GetRecord<float>(PATH_B);
    
    //	riv::ColorMap jetColorMap = colors::jetColorMap();
    return new RIVColorRGBProperty<float>(pathsTable,pathRRecord,pathGRecord,pathBRecord);
}

RIVColorRGBProperty<float>* createRayColorProperty(RIVDataSet<float,ushort>* dataset) {
    RIVTable<float,ushort> *intersectionsTable = (*datasetOne)->GetTable(INTERSECTIONS_TABLE);
    
    RIVRecord<float>* isectRRecord = intersectionsTable->GetRecord<float>(INTERSECTION_R);
    RIVRecord<float>* isectGRecord = intersectionsTable->GetRecord<float>(INTERSECTION_G);
    RIVRecord<float>* isectBrRecord = intersectionsTable->GetRecord<float>(INTERSECTION_B);
    
    //	riv::ColorMap jetColorMap = colors::jetColorMap();
    return new RIVColorRGBProperty<float>(intersectionsTable,isectRRecord,isectGRecord,isectBrRecord);
}

void setup(int argc, char** argv) {
    
    //Create the EMBREE renderer
    DataConnector* dcOne = new DataConnector(processRendererOne,rendererOneFinishedFrame);
    TriangleMeshGroup sceneDataTwo;
    TriangleMeshGroup sceneDataOne;
    if(argc == 3) { //Just one renderer
        char* type = argv[1];
        if(strcmp(type,"-connect") == 0) {
            rendererOne = new EMBREERenderer(dcOne, std::string(argv[2]),1);
            sceneDataOne = getSceneData(rendererOne);
            dataControllerOne = new DataController(2 * maxPaths, bootstrapRepeat,sceneDataOne.xBounds,sceneDataOne.yBounds,sceneDataOne.zBounds,sceneDataOne.NumberOfMeshes());
            dataControllerOne->SetAcceptProbability(2.F * maxPaths / (rendererOne->getWidth() * rendererOne->getHeight() * rendererOne->getSamplesPerPixel()));
            datasetOne = dataControllerOne->GetDataSet();
            printf("datsetone** = %p --> datasetone* = %p\n",datasetOne,*datasetOne);
            connectedOne = true;
            printf("1 renderer set up.\n");
        }
        else if(strcmp(argv[1],"-renderonly") == 0) { // No connect, render only
            rendererOne = new EMBREERenderer(std::string(argv[2]));
            printf("1 renderer set up for rendering only.\n");
        }
        else {
            char buffer[60];
            sprintf(buffer,"Unknown command option %s\n",argv[1]);
            throw std::runtime_error(buffer);
        }
    }
    else if(argc == 5) {
        if(strcmp(argv[1],"-connect") == 0) {
            rendererOne = new EMBREERenderer(dcOne, std::string(argv[2]),1);
            connectedOne = true;
            sceneDataOne = getSceneData(rendererOne);
        }
        else if(strcmp(argv[1],"-renderonly") == 0) { // No connect, render only
            rendererOne = new EMBREERenderer(std::string(argv[3]));
            printf("1 renderer set up for rendering only.\n");
        }
        else {
            char buffer[60];
            sprintf(buffer,"Unknown command option %s\n",argv[1]);
            throw std::runtime_error(buffer);
        }
        if(strcmp(argv[3],"-connect") == 0) {
            connectedTwo = true;
            DataConnector* dcTwo = new DataConnector(processRendererTwo,rendererTwoFinishedFrame);
            rendererTwo = new EMBREERenderer(dcTwo, std::string(argv[4]),1);
            //            float acceptProbTwo = 2.F * maxPaths / (rendererTwo->getWidth() * rendererTwo->getHeight() * rendererTwo->getSamplesPerPixel());
            //            dataControllerOne->SetAcceptProbability(acceptProbTwo);
            sceneDataTwo = getSceneData(rendererTwo);
        }
        else if(strcmp(argv[3],"-renderonly") == 0) { // No connect, render only
            rendererOne = new EMBREERenderer(std::string(argv[3]));
            printf("1 renderer set up for rendering only.\n");
        }
        else {
            char buffer[60];
            sprintf(buffer,"Unknown command option %s\n",argv[1]);
            throw std::runtime_error(buffer);
        }
        //find the largest bounds
        Vec2f xBounds,yBounds,zBounds;
        xBounds = Vec2f(std::min(sceneDataOne.xBounds[0],sceneDataTwo.xBounds[0]),std::max(sceneDataOne.xBounds[1],sceneDataTwo.xBounds[1]));
        yBounds = Vec2f(std::min(sceneDataOne.yBounds[0],sceneDataTwo.yBounds[0]),std::max(sceneDataOne.yBounds[1],sceneDataTwo.yBounds[1]));
        zBounds = Vec2f(std::min(sceneDataOne.zBounds[0],sceneDataTwo.zBounds[0]),std::max(sceneDataOne.zBounds[1],sceneDataTwo.zBounds[1]));
        
        dataControllerOne = new DataController(2 * maxPaths, bootstrapRepeat,xBounds,yBounds,zBounds,sceneDataOne.NumberOfMeshes());
        dataControllerTwo = new DataController(2 * maxPaths,bootstrapRepeat,xBounds,yBounds,zBounds,sceneDataTwo.NumberOfMeshes());
        datasetOne = dataControllerOne->GetDataSet();
        datasetTwo = dataControllerTwo->GetDataSet();
        float acceptProbOne = 2.F * maxPaths / (rendererOne->getWidth() * rendererOne->getHeight() * rendererOne->getSamplesPerPixel());
        float acceptProbTwo = 2.F * maxPaths / (rendererTwo->getWidth() * rendererTwo->getHeight() * rendererTwo->getSamplesPerPixel());
        dataControllerOne->SetAcceptProbability(acceptProbOne);
        dataControllerTwo->SetAcceptProbability(acceptProbTwo);
        printf("2 renderers set up.\n");
        
    }
    else {
        throw std::runtime_error("Unsupported number of arguments (1 or 2 expected)");
    }
    
    
    
    //The imageview should display two rendered images
    
    int nrConnected = (int)connectedOne + (int)connectedTwo;
    float bottomHalfY = height / 2.f + padding + sliderViewHeight / 2.F;
    float squareSize = height / 2.F - 2 * padding - sliderViewHeight / 2.F;
    float ratio = rendererOne->getWidth() / (float)rendererOne->getHeight();
    float imageViewWidth = squareSize * ratio;
    
    if(nrConnected) {
        std::vector<riv::Color> colors;
        colors.push_back(colors::BLUE);
        colors.push_back(colors::RED);
        riv::ColorMap redBlue(colors);
        
        parallelViewWindow = glutCreateSubWindow(mainWindow,padding,padding,width-2*padding,height/2.F-2*padding - sliderViewHeight / 2.F);
        ParallelCoordsView::windowHandle = parallelViewWindow;
        glutSetWindow(parallelViewWindow);
        glEnable( GL_LINE_SMOOTH );
        glEnable( GL_POLYGON_SMOOTH );
        glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
        glutDisplayFunc(ParallelCoordsView::DrawInstance);
        glutReshapeFunc(ParallelCoordsView::ReshapeInstance);
        glutMouseFunc(ParallelCoordsView::Mouse);
        glutMotionFunc(ParallelCoordsView::Motion);
        glutSpecialFunc(keys);
        if(nrConnected == 2) {
            imageViewWidth += imageViewWidth;
        }
        imageViewWindow = glutCreateSubWindow(mainWindow,padding,bottomHalfY,imageViewWidth,squareSize);
        glutSetWindow(imageViewWindow);
        RIVImageView::windowHandle = imageViewWindow;
        glutSpecialFunc(keys);
        glutDisplayFunc(RIVImageView::DrawInstance);
        glutReshapeFunc(RIVImageView::ReshapeInstance);
        glutMouseFunc(RIVImageView::Mouse);
        glutMotionFunc(RIVImageView::Motion);
    }
    else { //Take up the full screen
        imageViewWidth = width;
        float imageViewHeight = ratio * imageViewWidth;
        imageViewWindow = glutCreateSubWindow(mainWindow,0,height - (imageViewHeight / 2.F),imageViewWidth,imageViewHeight);
        glutSetWindow(imageViewWindow);
        RIVImageView::windowHandle = imageViewWindow;
        glutSpecialFunc(keys);
        glutDisplayFunc(RIVImageView::DrawInstance);
        glutReshapeFunc(RIVImageView::ReshapeInstance);
        
        //        glutDisplayFunc(RIVImageView::DrawInstance);
        //        glutReshapeFunc(RIVImageView::ReshapeInstance);
        imageViewWidth = width;
        imageView = new RIVImageView(rendererOne);
        
        glutInitWindowSize(rendererOne->getWidth(),rendererOne->getHeight());
    }
    
    if(datasetOne || datasetTwo) {
        int sceneViewPosX = padding + imageViewWidth;
        int sceneViewWidth = width - padding * 3 - imageViewWidth;
        sceneViewWindow = glutCreateSubWindow(mainWindow, sceneViewPosX, bottomHalfY, sceneViewWidth, squareSize);
        RIV3DView::windowHandle = sceneViewWindow;
        glutSetWindow(sceneViewWindow);
        glEnable( GL_LINE_SMOOTH );
        //    glEnable( GL_POLYGON_SMOOTH );
        glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        //    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
        glutDisplayFunc(RIV3DView::DrawInstance);
        //	glutDisplayFunc(doNothing);
        glutReshapeFunc(RIV3DView::ReshapeInstance);
        glutMouseFunc(RIV3DView::Mouse);
        glutMotionFunc(RIV3DView::Motion);
        glutSpecialFunc(keys);
    }
    
    if(datasetTwo && datasetOne) {
        
        sliderViewWindow = glutCreateSubWindow(mainWindow, padding, height/2.F-2*padding, width - 2* padding, sliderViewHeight);
        RIVSliderView::windowHandle = sliderViewWindow;
        glutSetWindow(sliderViewWindow);
        glutDisplayFunc(RIVSliderView::DrawInstance);
        //	glutDisplayFunc(doNothing);
        glutReshapeFunc(RIVSliderView::ReshapeInstance);
        glutMouseFunc(RIVSliderView::Mouse);
        glutMotionFunc(RIVSliderView::Motion);
        glutSpecialFunc(keys);
        
        RIVColorProperty* colorTwo = new RIVFixedColorProperty(0, 0, 1);
        auto pathColorOne = createPathColorProperty(*datasetOne);
        auto rayColorOne = createRayColorProperty(*datasetOne);
        RIVColorProperty* colorOne = new RIVFixedColorProperty(1, 0, 0);
        
        
        auto rayColorTwo = createRayColorProperty(*datasetTwo);
        auto pathColorTwo = createPathColorProperty(*datasetTwo);
        
        //Fixed colors for testing
        auto isectTable = (*datasetTwo)->GetTable(INTERSECTIONS_TABLE);
        
        //		RIVEvaluatedColorProperty<float>* xLinear = new RIVEvaluatedColorProperty<float>(redBlue, isectTable, isectTable->GetRecord<float>(POS_X));
        
        //		parallelCoordsView = new ParallelCoordsView(datasetOne,datasetTwo,dataControllerOne->GetTrueDistributions(),dataControllerTwo->GetTrueDistributions(),pathColorOne,rayColorOne,pathColorTwo,rayColorTwo);
        
        sceneView = new RIV3DView(datasetOne,datasetTwo,rendererOne,rendererTwo,sceneDataOne, sceneDataTwo, dataControllerOne->GetEnergyDistribution(),dataControllerTwo->GetEnergyDistribution(),pathColorOne,pathColorTwo);
        //		sceneView = new RIV3DView(datasetOne,datasetTwo,rendererOne,rendererTwo,colorOne,colorTwo,sizeProperty);
        imageView = new RIVImageView(datasetOne,datasetTwo,rendererOne,rendererTwo);
        
        sliderView = new RIVSliderView(datasetOne,datasetTwo,dataControllerOne->GetTrueDistributions(),dataControllerTwo->GetTrueDistributions());
        parallelCoordsView = new ParallelCoordsView(datasetOne,datasetTwo,dataControllerOne->GetTrueDistributions(),dataControllerTwo->GetTrueDistributions(),colorOne,colorOne,colorTwo,colorTwo,sliderView);
        
        
        (*datasetTwo)->AddDataListener(sceneView);
        (*datasetTwo)->AddDataListener(parallelCoordsView);
        (*datasetTwo)->AddDataListener(imageView);
        (*datasetOne)->AddDataListener(imageView);
        (*datasetOne)->AddDataListener(sceneView);
        (*datasetOne)->AddDataListener(parallelCoordsView);
        
        (*datasetOne)->AddDataListener(sliderView);
        (*datasetTwo)->AddDataListener(sliderView);
    }
    else if(datasetOne) {
        RIVColorProperty* colorTwo = new RIVFixedColorProperty(0, 0, 1);
        RIVColorProperty* colorOne = new RIVFixedColorProperty(1, 0, 0);
        auto pathColorOne = createPathColorProperty(*datasetOne);
        auto rayColorOne = createRayColorProperty(*datasetOne);
        parallelCoordsView = new ParallelCoordsView(datasetOne,dataControllerOne->GetTrueDistributions(),colorOne,colorOne,sliderView);
        sceneView = new RIV3DView(datasetOne,rendererOne,sceneDataOne,dataControllerOne->GetEnergyDistribution(), pathColorOne);
        imageView = new RIVImageView(datasetOne,rendererOne);
        (*datasetOne)->AddDataListener(imageView);
        (*datasetOne)->AddDataListener(sceneView);
        (*datasetOne)->AddDataListener(parallelCoordsView);
    }
    //        heatMapView = new RIVHeatMapView(&dataset);
    //	uiView = new RIVUIView(datasetOne, parallelCoordsView, sceneView, imageView,  uiViewWidth, uiPosX, bottomHalfY, squareSize, padding, padding);
    
    //Add some filter callbacks
    printf("Finished setting up...");
}

int main(int argc, char **argv)
{
    printf("Initialising Rendering InfoVis...\n");
    
    //        testFunctions();
    
    
    srand(time(NULL));
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide */
    glutInit(&argc, argv);
    
    //Use double buffering!
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    
    width = glutGet(GLUT_SCREEN_WIDTH);
    //	height = 0.75*glutGet(GLUT_SCREEN_HEIGHT);
    height = .85 * glutGet(GLUT_SCREEN_HEIGHT);
    
    /* set the initial window size */
    glutInitWindowSize(width, height);
    
    /* set the initial window position */
    glutInitWindowPosition(posX,posY);
    
    /* create the window and store the handle to it */
    mainWindow = glutCreateWindow("Rendering InfoVis" /* title */ );
    
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    
    glutIdleFunc(idle);
    
    /* register function to handle window resizes */
    glutReshapeFunc(reshape);
    
    // display and idle function
    glutDisplayFunc(display);
    
    //    renderingPaused = true;
    
    /* register function that handles mouse */
    
    glutSpecialFunc(keys);
    
    setup(argc,argv);
    
    /* Transparency stuff */
    glEnable (GL_BLEND);
    
    glClearColor(1,1,1, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* start the GLUT main loop */
    glutMainLoop();
    
    return EXIT_SUCCESS;
}

