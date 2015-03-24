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

#define BIN_X 30

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

bool linkPixelDistros = false;

const int maxPathsOne = 5000;
const int maxBootstrapRepeatOne = 5;

const int maxPathsTwo = 5000;
const int maxBootstrapRepeatTwo = 5;

const int sliderViewHeight = 40;

bool connectedOne = false;
bool connectedTwo = false;

int currentFrameOne = 0;
int currentFrameTwo = 0;

float swapchainWeight = 10;

clock_t startDelay;
bool isDelayed = false;

bool renderingPausedOne = true;
bool renderingPausedTwo = true;
//bool renderingPausedOne = false;
//bool renderingPausedTwo = false;
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
    Octree octree(2,0,0,0,1,2);
    octree.Add(-0.25F, -0.25F, -0.25F, 1);
    octree.Add(-0.25F, -0.25F, -0.25F, 1);
    octree.Add(-0.25F, -0.25F, -0.25F, 1);
//    octree.Add(-0.25F, -0.25F, 0.25F, 1);
    
    OctreeNode* root = octree.GetRoot();
    printf("Aggregate value root = %f\n",root->AggregateValue());
    printf("Max value = %f\n",octree.MaxValue());
    
    
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
            testHistogram.Add(rand() / RAND_MAX % 10);
        }
    }
    
    ///TEST PLUS
    Histogram<float> one("one",0,10,2);
    Histogram<float> two("two",0,10,2);
    
//    one.Add(3);
    one.Add(7);
    
    two.Add(3);
    
    one.Print();
    two.Print();
    
    Histogram<float> result = one + two;
    
    result.Print();
    
    Histogram<float> copy = testHistogram;
    printf("Original = \n");
    testHistogram.Print();
    printf("Copy = \n");
    copy.Print();
    
    testHistogramSet.AddHistogram(testHistogram);
    testHistogramSet.AddHistogram(testHistogramInt);
    
    testHistogramSet.Clear();
    //    floatHist2D.Add(2, 2);
    floatHist2D.Add(7, 2);
    floatHist2D.Add(2, 7);
    floatHist2D.Add(7, 7);
    floatHist2D.Add(7, 7);
    
    Histogram2D<float> floatHist2Dright = floatHist2D;
    floatHist2Dright.Clear();
    floatHist2Dright.Add(7,2);
    
//        for(int x = 0 ; x < 10 ; x++) {
//            for(int y = 0 ; y < 10 ; y++) {
//                size_t M = 10 * ((x+1)*(y+1));
//                for(int j = 0 ; j < M ; ++j) {
//                    floatHist2D.Add(x, y);
//                }
//            }
//        }
    
    Histogram2D<float> copy2D = floatHist2D;
    Histogram2D<float> sum2D = floatHist2D + floatHist2Dright;
    
    printf("Original = \n");
    floatHist2D.Print();
    printf("Copy = \n");
    copy2D.Print();
    printf("Sum = \n");
    sum2D.Print();
    
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

void testReferenceMemoryLeaks() {
    
//    const int M = 3;
//    const int N = 1;
    
    const int M = 10;
    const int N = 100;
    
    RIVTable<float>* testTable = new RIVTable<float>("test_1");
    RIVTable<float>* testTable2 = new RIVTable<float>("test_2");
    
    RIVSingleReference* singleRef = new RIVSingleReference(testTable,testTable2);
    RIVMultiReference* multiRef = new RIVMultiReference(testTable2,testTable2);
    
    for(int i = 0 ; i < M ; ++i) {
        singleRef->AddReference(i, i);
    
        std::vector<size_t> rows(N);
        for(int j = 0 ; j < N ; ++j) {
            rows[j] = j+i;
        }
        multiRef->AddReferences(i, rows);
    }
    
//    delete singleRef;
    printf("DELETE\n\n");
    
    singleRef->GetReferenceRows(1);
    multiRef->GetReferenceRows(1);
    
    delete multiRef;
    
    printf("Testing reference memory leaks finished...\n");
}

void testFunctions() {
    
    testReferenceMemoryLeaks();
    return;
    
    
    testOctree();
    return;
    
    testSampling();

    //Done

    
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
    printf("'%c' key (code = %d) pressed.\n",keyCode,key);
    
    float camSpeed = .25F;
    switch(keyCode) {
        case 9: //Tab key
            sceneView->CycleSelectionMode();
            break;
        case 27: //ESC key
            printf("Clear filters\n");
            //            invalidateAllViews();
            //			(*datasetOne)->StartFiltering();
            //            (*datasetOne)->ClearFilters();
            //			datasetOne)->StopFiltering();
            //
            imageView->ClearPixelDistributionOne();
            imageView->ClearPixelDistributionTwo();
            
            (*datasetOne)->StartFiltering();
            (*datasetOne)->ClearAllFilters();
            (*datasetOne)->StopFiltering();
            dataControllerOne->Reset();
            
            if(datasetTwo) {
                (*datasetTwo)->StartFiltering();
                (*datasetTwo)->ClearAllFilters();
                (*datasetTwo)->StopFiltering();
                dataControllerTwo->Reset();
            }

            
//            invalidateAllViews();
            
            break;
        case 39:// ' key
            sceneView->CycleSelectedLights();
            break;
        case 92: // \ key
            sceneView->CycleSelectedLights();
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
        case 47: // '/' key, increment bounce count 3d view
            sceneView->IncrementBounceNrPath(1);
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
                rendererOne->CopySwapChainTo(rendererTwo,swapchainWeight);
                imageView->redisplayWindow();
                
//                int newMaxPaths = 1000;
//                dataControllerTwo->SetMaxPaths(newMaxPaths);
//                float acceptProbTwo = 2.F * newMaxPaths / (rendererTwo->getWidth() * rendererTwo->getHeight() * rendererTwo->getSamplesPerPixel());
//                dataControllerTwo->SetAcceptProbability(acceptProbTwo);
            }
            break;
        case 54: // 6 key, discconnect data processing for renderer 1
            connectedOne = !connectedOne;
            printf("Renderer one is now ");
            if(!connectedOne) printf("NOT ");
            printf("connected to data processing\n");
            break;
        case 55: // 7 key, discconnect data processing for renderer 2
            connectedTwo = !connectedTwo;
            printf("Renderer two is now ");
            if(!connectedTwo) printf("NOT ");
            printf("connected to data processing\n");
            break;
        case 56: // the '8' key, average pixel distro 1

//                printf("Copying swapchain from renderer 1 to renderer 2!!\n");
                imageView->SmoothPixelDistributionOne();
            
            break;
        case 57: // the '9' key, average pixel distro 2

                //                printf("Copying swapchain from renderer 1 to renderer 2!!\n");
                imageView->SmoothPixelDistributionTwo();
            break;
        case 48: // the '0' key, link average the two heatmaps with eachother
            if(datasetOne && datasetTwo) {
                linkPixelDistros = true;
                imageView->CombinePixelDistributions();
            }
            break;
        case 97: // 'a' key
            sceneView->MoveCamera(-camSpeed, 0, 0);
            break;
        case 98: // 'b' key, bootstrap
//            glutSwapBuffers();
            dataControllerOne->Reduce();
            break;
        case 99: // 'c' key
            if(sceneView) {
                sceneView->ToggleDrawIntersectionPoints();
                postRedisplay = true;
            }
            break;
        case 108 : // 'l' key, toggle lines drawing
            ++swapchainWeight;
            printf("Swapchain weight is now %f\n",swapchainWeight);
            postRedisplay = false;
            break;
        case 107 : // 'k' key, toggle lines drawing
            if(swapchainWeight > 0) {
                --swapchainWeight;
                printf("Swapchain weight is now %f\n",swapchainWeight);
                postRedisplay = false;
            }
            break;
//        case 108 : // 'l' key, toggle lines drawing
//            sceneView->CyclePathSegment();
//            break;
        case 91: // '[' key, increase path segment
            sceneView->MovePathSegment(-.01F);
            break;
        case 93:
            sceneView->MovePathSegment(.01F);
            break;
        case 96: // '`' key, change background color of 3D view
            sceneView->ToggleBackgroundColor();
            break;
        case 104: // the 'h' from heatmap, toggle drawing the octree heatmap
            sceneView->ToggleDrawHeatmap();
            break;
        case 106: // the 'j' key, cause the h is taken for the other heatmap
            imageView->ToggleHeatmapDisplayMode();
            postRedisplay = false;
            break;
        case 110: // the 'n' key
            imageView->ToggleHeatmapToDisplay();
            postRedisplay = false;
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
                rendererTwo->outputMode("renderer2_output_frame=" + std::to_string(currentFrameTwo) + ".bmp");
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
        case 120: // 'x' key
            imageView->WeightPixelDistributionByThroughput();
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
        float imageViewWidth = std::min(squareSize * ratio,.3333F*width);
        float imageViewHeight = imageViewWidth / ratio;
        if(datasetTwo) {
            imageViewWidth += imageViewWidth;
        }
        glutSetWindow(imageViewWindow);
        //        glutInitDisplayMode(GLUT_SINGLE);
        glutPositionWindow(padding, height/2+padding + sliderViewHeight / 2.F);
        glutReshapeWindow(imageViewWidth,imageViewHeight); //Square bottom left corner
        
        glutSetWindow(sceneViewWindow);
        
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

void UnpauseRendererOne() {
    renderingPausedOne = false;
    printf("Rendering process ONE is now running...\n");
    //If dataset one has some filters the rendering will be guided, which means the true distributions
    //should be reset in order to speed up convergence
    if(datasetOne && (*datasetOne)->IsFiltered()) {
        dataControllerOne->Reset();
    }
}
void UnpauseRendererTwo() {
    renderingPausedTwo = false;
    printf("Rendering process TWO is now running...\n");
    //If dataset one has some filters the rendering will be guided, which means the true distributions
    //should be reset in order to speed up convergence
    if(datasetTwo && (*datasetTwo)->IsFiltered()) {
        dataControllerTwo->Reset();
    }
}
void TogglePauseOne() {
    printf("Rendering process ONE is now ");
    if(renderingPausedOne) {
        UnpauseRendererOne();
    }
    else {
        printf("paused.\n");
        renderingPausedOne = true;
    }
    //    glutPostRedisplay();
}
void TogglePauseTwo() {
    
    if(renderingPausedTwo) {
        UnpauseRendererTwo();
    }
    else {
        printf("Rendering process TWO is now paused...\n");
        renderingPausedTwo = true;
    }
}
//When 1 is running it pauses this renderer, when both are in the same state it toggles both
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
        UnpauseRendererOne();
        UnpauseRendererTwo();
    }
    else {
        renderingPausedTwo = true;
    }
}
void idle() {
    bool postRedisplay = false;
    
    checkIsDelayed();
//    if(linkPixelDistros) {
//        imageView->AveragePixelDistributions();
//    }
    int maxFrameOne = 52;
    if(!renderingPausedOne && currentFrameOne < maxFrameOne) {
        ++currentFrameOne;
        printf("Rendering renderer #1 frame %d\n",currentFrameOne);
//        Histogram2D<float>* pixelDistributionOne = imageView->GetPixelDistributionOne();
        Histogram2D<float>* pixelDistributionOne = imageView->GetActiveDistributionOne();
        if(pixelDistributionOne && pixelDistributionOne->NumberOfElements()) {
            //                heatmapOne->Print();
            rendererOne->RenderNextFrame(pixelDistributionOne);
        }
        else {
            rendererOne->RenderNextFrame();
        }
        renderOneFinishedFrame = false;
        postRedisplay = true;
    }
    int maxFrameTwo = 52;
    if(!renderingPausedTwo && currentFrameTwo < maxFrameTwo) {
        if(dataControllerTwo) {
//            if(currentFrameTwo == maxFrameTwo) {
//                dataControllerTwo->SetMaxPaths(5000);
//                renderingPausedTwo = true;
//                connectedTwo = false;
//                return;
//            }
            ++currentFrameTwo;
            auto pixelDistributionTwo = imageView->GetActiveDistributionTwo();
            printf("Rendering renderer #2 frame %d\n",currentFrameTwo);
            if(pixelDistributionTwo && pixelDistributionTwo->NumberOfElements()) {
//                printf("Active distribution = \n");
//                pixelDistributionTwo->Print();
                rendererTwo->RenderNextFrame(pixelDistributionTwo);
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
    if(connectedOne) {
        return dataControllerOne->ProcessNewPath(currentFrameOne,newPath);
    }
    return false;
}

void rendererOneFinishedFrame(size_t numPaths, size_t numRays) {
    //	dataControllerTwo->RendererOneFinishedFrame(numPaths,numRays);
    printf("\n*** Renderer one finished frame %d...\n",currentFrameOne);
    
    if(connectedOne) {
        dataControllerOne->Reduce();
        renderOneFinishedFrame = true;
    }
    imageView->redisplayWindow();

    //    renderingPausedOne = true;
}

bool processRendererTwo(PathData* newPath) {
    //	printf("New path from renderer #2 received!\n");
    if(connectedTwo) {
        return dataControllerTwo->ProcessNewPath(currentFrameTwo,newPath);
    }
    return true;
}
void rendererTwoFinishedFrame(size_t numPaths, size_t numRays) {
    printf("\n*** Renderer two finished frame %d...\n",currentFrameTwo);
    if(connectedTwo) {
        dataControllerTwo->Reduce();
    }
    imageView->redisplayWindow();
    renderTwoFinishedFrame = true;
//    renderingPausedTwo = true;
}
TriangleMeshGroup getSceneData(EMBREERenderer* renderer) {
    //Get the shapes and see what are trianglemeshes that we can draw
    std::vector<Shape*>* shapes = renderer->GetShapes();
    std::vector<TriangleMeshFull*> embreeMeshes;
    for(size_t i = 0 ; i < shapes->size() ; ++i) {
        Shape* rawShape = shapes->at(i);
        TriangleMeshFull* t = dynamic_cast<TriangleMeshFull*>(rawShape);
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
            dataControllerOne = new DataController(2 * maxPathsOne, maxBootstrapRepeatOne,sceneDataOne.xBounds,sceneDataOne.yBounds,sceneDataOne.zBounds,sceneDataOne.NumberOfMeshes(),rendererOne->GetNumLights(),rendererOne->getWidth(),rendererOne->getHeight());
            dataControllerOne->SetAcceptProbability(2.F * maxPathsOne / (rendererOne->getWidth() * rendererOne->getHeight() * rendererOne->getSamplesPerPixel()));
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
        
        dataControllerOne = new DataController(2 * maxPathsOne,maxBootstrapRepeatOne,xBounds,yBounds,zBounds,sceneDataOne.NumberOfMeshes(),rendererOne->GetNumLights(),rendererOne->getWidth(),rendererOne->getHeight());
        dataControllerTwo = new DataController(2 * maxPathsTwo,maxBootstrapRepeatTwo,xBounds,yBounds,zBounds,sceneDataTwo.NumberOfMeshes(),rendererTwo->GetNumLights(),rendererOne->getWidth(),rendererOne->getHeight());
        datasetOne = dataControllerOne->GetDataSet();
        datasetTwo = dataControllerTwo->GetDataSet();
        float acceptProbOne = 2.F * maxPathsOne / (rendererOne->getWidth() * rendererOne->getHeight() * rendererOne->getSamplesPerPixel());
        float acceptProbTwo = 2.F * maxPathsTwo / (rendererTwo->getWidth() * rendererTwo->getHeight() * rendererTwo->getSamplesPerPixel());
        dataControllerOne->SetAcceptProbability(acceptProbOne);
        dataControllerTwo->SetAcceptProbability(acceptProbTwo);
        printf("2 renderers set up.\n");
        
    }
    else {
        throw std::runtime_error("Unsupported number of arguments (2 or 4 expected)");
    }
    

    //The imageview should display two rendered images
    
    int nrConnected = (int)connectedOne + (int)connectedTwo;
    float bottomHalfY = height / 2.f + padding + sliderViewHeight / 2.F;
    float squareSize = height / 2.F - 2 * padding - sliderViewHeight / 2.F;
    float ratio = rendererOne->getWidth() / (float)rendererOne->getHeight();
//    float imageViewWidth = squareSize * ratio;
    float imageViewWidth = std::min(squareSize * ratio,.333F*width);
    float imageViewHeight = imageViewWidth / ratio;
    
    std::vector<riv::Color> colors;
    colors.push_back(colors::BLUE);
    colors.push_back(colors::RED);
    riv::ColorMap redBlue(colors);
    
    if(nrConnected) {

        
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
        imageViewWindow = glutCreateSubWindow(mainWindow,padding,bottomHalfY,imageViewWidth,imageViewHeight);
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
        sceneViewWindow = glutCreateSubWindow(mainWindow, sceneViewPosX, bottomHalfY, sceneViewWidth, imageViewHeight);
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
        riv::Color red(1.F,0.F,0.F);
        riv::Color white(0.9F,0.9F,0.9F);
        riv::Color purple(0.5F,0.F,0.5F);
        riv::Color blue(0.F,0.F,1.F);
        
        std::vector<riv::Color> colors = {red,white,blue};
        
        riv::ColorMap binColorMap(colors,0,1);
        
        sceneView = new RIV3DView(datasetOne,datasetTwo,rendererOne,rendererTwo,sceneDataOne, sceneDataTwo, dataControllerOne->GetEnergyDistribution3D(),dataControllerTwo->GetEnergyDistribution3D(),pathColorOne,rayColorOne,pathColorTwo,rayColorTwo);
        //		sceneView = new RIV3DView(datasetOne,datasetTwo,rendererOne,rendererTwo,colorOne,colorTwo,sizeProperty);
        imageView = new RIVImageView(datasetOne,datasetTwo,rendererOne,rendererTwo,dataControllerOne->GetPixelThroughputDistribution(),dataControllerTwo->GetPixelThroughputDistribution(),dataControllerOne->GetEnergyDistribution2D(),dataControllerTwo->GetEnergyDistribution2D());
        
        sliderView = new RIVSliderView(datasetOne,datasetTwo,dataControllerOne->GetTrueDistributions(),dataControllerTwo->GetTrueDistributions());
//        parallelCoordsView = new ParallelCoordsView(datasetOne,datasetTwo,binColorMap,pathColorOne,rayColorOne,pathColorTwo,rayColorTwo,sliderView);
        parallelCoordsView = new ParallelCoordsView(datasetOne,datasetTwo,binColorMap,colorOne,colorOne,colorTwo,colorTwo,sliderView);
        
        
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
//        RIVColorProperty* colorTwo = new RIVFixedColorProperty(0, 0, 1);
//        RIVColorProperty* colorOne = new RIVFixedColorProperty(1, 0, 0);
        auto pathColorOne = createPathColorProperty(*datasetOne);
        auto rayColorOne = createRayColorProperty(*datasetOne);
        parallelCoordsView = new ParallelCoordsView(datasetOne,redBlue, pathColorOne,rayColorOne,sliderView);
        sceneView = new RIV3DView(datasetOne,rendererOne,sceneDataOne,dataControllerOne->GetEnergyDistribution3D(), pathColorOne, rayColorOne);
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
    
//            testFunctions();
    
    
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

