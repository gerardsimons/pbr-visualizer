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

const int maxPaths = 5000;
const int bootstrapRepeat = 10;
const int sliderViewHeight = 50;

void TogglePause();

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
	imageView->redisplayWindow();
//	uiView->Invalidate();
	
	int previousWindow = glutGetWindow();
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutSetWindow(previousWindow);
}

void testFunctions() {
	
	//	RIVDataSet<float,ushort>* testData = new RIVDataSet<float,ushort>("test_set");
	//
	//	RIVTable<float,ushort>* testTable = testData->CreateTable("test_table");
	//
	//	RIVRecord<float>* floatRecord = testTable->CreateRecord<float>("floats");
	//	RIVRecord<ushort>* shortRecord = testTable->CreateRecord<ushort>("shorts");
	//
	////	RIVMultiReference* multiRef = new RIVMultiReference();
	////	RIVSingleReference* singleRef = new RIVSingleReference();
	//
	//	const int N = 1000;
	//	for(int i = 0 ; i < N ; ++i) {
	//		floatRecord->AddValue(rand());
	//		shortRecord->AddValue(i);
	//	}
	//
	//	auto histogramset = testData->CreateHistogramSet(10);
	//
	//	delete testData;
	//
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
	printf("'%c' key pressed.\n",key);
	
	float camSpeed = .1F;
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
			
			(*datasetOne)->StartFiltering();
			(*datasetOne)->ClearAllFilters();
			(*datasetOne)->StopFiltering();
			break;
		case 45: // - key
			parallelCoordsView->DecreaseLineOpacity();
			break;
		case 43: // + key
			parallelCoordsView->IncreaseLineOpacity();
			break;
		case 61: // = key is on the same physical keyboard button as +, so cut the user some slack and
			parallelCoordsView->IncreaseLineOpacity();
			break;
		case 32: //Space bar
//			dataControllerOne->sTogglePause();
//			dataControllerTwo->TogglePause();
			TogglePause();
			break;
		case 49: //The '1' key, switch to renderer one if not already using it
			parallelCoordsView->toggleDrawDataSetOne();
			sceneView->ToggleDrawDataSetOne();
			break;
		case 50: //The '2' key, switch to renderer two if not already using it
			parallelCoordsView->toggleDrawDataSetTwo();
			sceneView->ToggleDrawDataSetTwo();
			break;
		case 98: // 'b' key
			glutSwapBuffers();
			printf("Manual swap buffers\n");
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
		case 112: //The 'p' key, toggle drawing paths in 3D view
			sceneView->ToggleDrawPaths();
			break;
		case 114: // 'r' key, force next frame rendering {
		{
			rendererOne->RenderNextFrame();
			if(rendererTwo)
				rendererTwo->RenderNextFrame();
			postRedisplay = true;
			break;
		}
		case 111: // 'o' key, optimize clusters (debug feature)
		{
			//            clusters->OptimizeClusters();
			postRedisplay = true;
			break;
		}
		case 122: // 'z' key, save the image
		{
			printf("Key not set.\n");
			break;
		}
		case 116: // 't' key, use as temp key for some to-test function
		{
			invalidateAllViews();
			glutPostRedisplay();
			break;
		}
		case 119: // 'w' key, move camera in Y direction
			sceneView->MoveCamera(0,camSpeed,0);
			break;
		case 115: // 's' kegy
			sceneView->MoveCamera(0, -camSpeed, 0);
			break;
		case GLUT_KEY_UP:
			sceneView->MoveCamera(0,0,camSpeed);
			//            uiView->MoveMenu(0,-10.F);
			break;
		case GLUT_KEY_DOWN:
			sceneView->MoveCamera(0,0,-camSpeed);
			//            uiView->MoveMenu(0,10);
			break;
		case GLUT_KEY_LEFT:
			sceneView->MoveCamera(-camSpeed,0,0);
			//            uiView->MoveMenu(-10.F,0);
			break;
		case GLUT_KEY_RIGHT:
			sceneView->MoveCamera(camSpeed,0,0);
			//            uiView->MoveMenu(10.F,0);
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
	glutSetWindow(parallelViewWindow);
	glutPositionWindow(padding,padding);
	int newWidthPC = width-2*padding;
	int newHeightPC = height/2-2*padding - sliderViewHeight / 2.F;
	glutReshapeWindow(newWidthPC,newHeightPC); //Upper half and full width of the main window
	//
	//    //image view window
	float squareSize = height / 2.F - 2 * padding - sliderViewHeight / 2.F;
	float imageViewWidth = squareSize;
	if(datasetTwo) {
		imageViewWidth += imageViewWidth;
	}
	glutSetWindow(imageViewWindow);
	glutPositionWindow(padding, height/2+padding + sliderViewHeight / 2.F);
	glutReshapeWindow(imageViewWidth,squareSize); //Square bottom left corner
	
	//	glutSetWindow(uiViewWindow);
	//	glutPositionWindow(padding * 3 + squareSize * 3, height/2+padding + sliderViewHeight / 2.F);
	//	glutReshapeWindow(width - squareSize - 2 * padding,squareSize); //Square bottom left corner
	//
	//    //    //3D scene view inspector window
	//    glutSetWindow(sceneViewWindow);
	//    glutPositionWindow(padding + height/2, height/2+padding);
	//    glutReshapeWindow(height / 2 - 2 * padding, height /2 - 2 *padding); //Square bottom right next to imageview window
	//
	//    //Heat map view
	//    glutSetWindow(heatMapViewWindow);
	//    glutPositionWindow(padding + height, height / 2 + padding);
	//    glutReshapeWindow(height / 2 - 2 * padding, height /2 - 2 *padding);
}

const int maxFrames = 100;
int currentFrame = 0;

clock_t startDelay;
bool isDelayed = false;

bool renderingPaused = false;
int delayTimeLeft = 0;
int delayTimerInterval = 0;

bool renderOneFinishedFrame = true;
bool renderTwoFinishedFrame = true;

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
void TogglePause() {
	printf("Rendering process is now ");
	if(renderingPaused) {
		printf("running.\n");
		renderingPaused = false;
	}
	else {
		printf("paused.\n");
		renderingPaused = true;
	}
}
void idle() {
	bool postRedisplay = false;
	
	checkIsDelayed();
	
	if(!renderingPaused) {
		if(!isDelayed) {
			printf("Rendering frame %d\n",currentFrame);
			++currentFrame;
			rendererOne->RenderNextFrame();
			renderOneFinishedFrame = false;
			postRedisplay = true;
			if(dataControllerTwo) {
				rendererTwo->RenderNextFrame();
				renderTwoFinishedFrame = false;
			}
		}
		else {
			printf("Rendering is still delayed for %d ms.",delayTimeLeft);
		}
	}
	
	if(postRedisplay) {
		glutPostRedisplay();
	}
}

bool processRendererOne(PathData* newPath) {
	//	printf("New path from renderer #1 received!\n");
	return dataControllerOne->ProcessNewPath(currentFrame,newPath);
}

void rendererOneFinishedFrame(size_t numPaths, size_t numRays) {
	//	dataControllerTwo->RendererOneFinishedFrame(numPaths,numRays);
	printf("Renderer one finished a frame...\n");
	dataControllerOne->Reduce();
	
	renderOneFinishedFrame = true;
//	if(renderOneFinishedFrame && renderTwoFinishedFrame) {
//		delayRendering(5000);
		
//		renderingPaused = true;
//	}
}

bool processRendererTwo(PathData* newPath) {
	//	printf("New path from renderer #2 received!\n");
	return dataControllerTwo->ProcessNewPath(currentFrame,newPath);
}

void rendererTwoFinishedFrame(size_t numPaths, size_t numRays) {
	printf("Renderer two finished a frame...\n");
	dataControllerTwo->Reduce();
	renderTwoFinishedFrame = true;
//	if(renderOneFinishedFrame && renderTwoFinishedFrame) {
//		delayRendering(5000);
//		renderingPaused = true;
//	}
	//	dataControllerTwo->Unpause();
}
/* Setup the data controller that manages the rendering data
 * The data controllers start out with 2 times the given max_paths to make sure there is enough to sample from,
 * after which a bootstrap is created and only half of the total data is kept, the other half is used to cache
 * new data. After each frame the new (or candidate) data is joined with the current data and bootstrapping occurs again.
 * If a new and better bootstrapping sample is found the current data is replaced with this one and the candidate data is cleared.
 */
void setupDataController(const int argc, char** argv) {
	//Create the EMBREE renderer
	dataControllerOne = new DataController(argc - 1,2 * maxPaths, bootstrapRepeat);
	datasetOne = dataControllerOne->GetDataSet();
	if(argc == 2) { //Just one renderer
		DataConnector* connector = new DataConnector(processRendererOne,rendererOneFinishedFrame);
		rendererOne = new EMBREERenderer(connector, std::string(argv[1]));
		dataControllerOne->SetAcceptProbability(2.F * maxPaths / (rendererOne->getWidth() * rendererOne->getHeight() * rendererOne->getSamplesPerPixel()));
		printf("1 renderer set up.\n");
	}
	else if(argc == 3) {
		dataControllerTwo = new DataController(argc - 1, 2*maxPaths,bootstrapRepeat);
		DataConnector* dcOne = new DataConnector(processRendererOne,rendererOneFinishedFrame);
		DataConnector* dcTwo = new DataConnector(processRendererTwo,rendererTwoFinishedFrame);
		rendererOne = new EMBREERenderer(dcOne, std::string(argv[1]));
		rendererTwo = new EMBREERenderer(dcTwo, std::string(argv[2]));
		datasetTwo = dataControllerTwo->GetDataSet();
		float acceptProb = 2.F * maxPaths / (rendererOne->getWidth() * rendererOne->getHeight() * rendererOne->getSamplesPerPixel());
		dataControllerOne->SetAcceptProbability(acceptProb);
		dataControllerTwo->SetAcceptProbability(acceptProb);
		printf("2 renderers set up.\n");
	}
	else {
		throw std::runtime_error("Unsupported number of arguments (1 or 2 expected)");
	}
}

//Useful to temporarily disable some drawing
void doNothing() {
	//Lalalala
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

void createViews() {
	
	std::vector<riv::Color> colors;
	colors.push_back(colors::BLUE);
	colors.push_back(colors::RED);
	riv::ColorMap redBlue(colors);
	
	//		RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,intersectionsTable->GetRecord("bounce#"),jetColorMap);
	//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(pathTable,"radiance R","radiance G","radiance B");
	RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(2);
	
	parallelViewWindow = glutCreateSubWindow(mainWindow,padding,padding,width-2*padding,height/2.F-2*padding - sliderViewHeight / 2.F);
	ParallelCoordsView::windowHandle = parallelViewWindow;
	glutSetWindow(parallelViewWindow);
	glutDisplayFunc(ParallelCoordsView::DrawInstance);
	//	glutDisplayFunc(doNothing);
	glutReshapeFunc(ParallelCoordsView::ReshapeInstance);
	glutMouseFunc(ParallelCoordsView::Mouse);
	glutMotionFunc(ParallelCoordsView::Motion);
	glutSpecialFunc(keys);
	
	//Load image
	float bottomHalfY = height / 2.f + padding + sliderViewHeight / 2.F;
	float squareSize = height / 2.F - 2 * padding - sliderViewHeight / 2.F;
	float imageViewWidth = squareSize;
	if(datasetTwo) {
		imageViewWidth += imageViewWidth;
	}
	
	glutSetWindow(imageViewWindow);
	imageViewWindow = glutCreateSubWindow(mainWindow,padding,bottomHalfY,imageViewWidth,squareSize);
	RIVImageView::windowHandle = imageViewWindow;
	glutDisplayFunc(RIVImageView::DrawInstance);
	glutReshapeFunc(RIVImageView::ReshapeInstance);
	glutMouseFunc(RIVImageView::Mouse);
	glutMotionFunc(RIVImageView::Motion);
	glutSpecialFunc(keys);
	
	int sceneViewPosX = padding * 3 + imageViewWidth;
	sceneViewWindow = glutCreateSubWindow(mainWindow, sceneViewPosX, bottomHalfY, squareSize, squareSize);
	RIV3DView::windowHandle = sceneViewWindow;
	glutSetWindow(sceneViewWindow);
	glutDisplayFunc(RIV3DView::DrawInstance);
	//	glutDisplayFunc(doNothing);
	glutReshapeFunc(RIV3DView::ReshapeInstance);
	glutMouseFunc(RIV3DView::Mouse);
	glutMotionFunc(RIV3DView::Motion);
	glutSpecialFunc(keys);
	
	//        heatMapViewWindow = glutCreateSubWindow(mainWindow, padding * 5 + squareSize * 2, bottomHalfY, squareSize, squareSize);
	//        glutSetWindow(heatMapViewWindow);
	//        glutReshapeFunc(RIVHeatMapView::ReshapeInstance);
	//        glutDisplayFunc(RIVHeatMapView::DrawInstance);
	//        glutMouseFunc(RIVHeatMapView::Mouse);
	//        glutMotionFunc(RIVHeatMapView::Motion);
	
	sliderViewWindow = glutCreateSubWindow(mainWindow, padding, height/2.F-2*padding, width - 2* padding, sliderViewHeight);
	RIVSliderView::windowHandle = sliderViewWindow;
	glutSetWindow(sliderViewWindow);
	glutDisplayFunc(RIVSliderView::DrawInstance);
	//	glutDisplayFunc(doNothing);
	glutReshapeFunc(RIVSliderView::ReshapeInstance);
	glutMouseFunc(RIVSliderView::Mouse);
	glutMotionFunc(RIVSliderView::Motion);
	glutSpecialFunc(keys);
	
	//	int uiViewWidth = width - 3 * squareSize - 2 * padding;
	//	int uiPosX = sceneViewPosX + squareSize + padding;
	//	uiViewWindow = glutCreateSubWindow(mainWindow, uiPosX, bottomHalfY, uiViewWidth, squareSize);
	//	RIVUIView::windowHandle = uiViewWindow;
	//	glutSetWindow(uiViewWindow);
	//	glutDisplayFunc(RIVUIView::DrawInstance);
	//	//	glutDisplayFunc(doNothing);
	//	glutReshapeFunc(RIVUIView::ReshapeInstance);
	//	glutMouseFunc(RIVUIView::Mouse);
	//	glutMotionFunc(RIVUIView::Motion);
	//	glutSpecialFunc(keys);
	//	uiView = new RIVUIView(datasetOne, uiViewWidth, uiPosX, bottomHalfY, squareSize, padding, padding);
	
	//Create views for two renderers
	auto pathColorOne = createPathColorProperty(*datasetOne);
	auto rayColorOne = createRayColorProperty(*datasetOne);
	if(datasetTwo) {
		
		//Im so lazy ....
		//		auto pathColorTwo = createPathColorProperty(*datasetTwo);
		//		auto rayColorTwo = createRayColorProperty(*datasetTwo);
		
		//Fixed colors for testing
		RIVColorProperty* colorOne = new RIVFixedColorProperty(1, 0, 0);
		RIVColorProperty* colorTwo = new RIVFixedColorProperty(0, 0, 1);
		
		auto isectTable = (*datasetTwo)->GetTable(INTERSECTIONS_TABLE);
		
		//		RIVEvaluatedColorProperty<float>* xLinear = new RIVEvaluatedColorProperty<float>(redBlue, isectTable, isectTable->GetRecord<float>(POS_X));
		
		//		parallelCoordsView = new ParallelCoordsView(datasetOne,datasetTwo,dataControllerOne->GetTrueDistributions(),dataControllerTwo->GetTrueDistributions(),pathColorOne,rayColorOne,pathColorTwo,rayColorTwo);
		
		sceneView = new RIV3DView(datasetOne,datasetTwo,rendererOne,rendererTwo,rayColorOne,sizeProperty);
		imageView = new RIVImageView(datasetOne,datasetTwo,rendererOne,rendererTwo);
		sliderView = new RIVSliderView(datasetOne,datasetTwo,dataControllerOne->GetTrueDistributions(),dataControllerTwo->GetTrueDistributions(),redBlue);
		parallelCoordsView = new ParallelCoordsView(datasetOne,datasetTwo,dataControllerOne->GetTrueDistributions(),dataControllerTwo->GetTrueDistributions(),colorOne,colorOne,colorTwo,colorTwo,sliderView);
		
		(*datasetTwo)->AddDataListener(sceneView);
		(*datasetTwo)->AddDataListener(parallelCoordsView);
		(*datasetTwo)->AddDataListener(imageView);
		
		(*datasetOne)->AddDataListener(sliderView);
		(*datasetTwo)->AddDataListener(sliderView);
	}
	else {
		parallelCoordsView = new ParallelCoordsView(datasetOne,dataControllerOne->GetTrueDistributions(),pathColorOne,rayColorOne,sliderView);
		sceneView = new RIV3DView(datasetOne,rendererOne,rayColorOne,sizeProperty);
		imageView = new RIVImageView(datasetOne,rendererOne);
	}
	//        heatMapView = new RIVHeatMapView(&dataset);
	
	//Add some filter callbacks
	(*datasetOne)->AddDataListener(imageView);
	(*datasetOne)->AddDataListener(sceneView);
	(*datasetOne)->AddDataListener(parallelCoordsView);
}

int main(int argc, char **argv)
{
	printf("Initialising Rendering InfoVis...\n");
	
	//	testFunctions();
	
	srand(time(NULL));
	/* initialize GLUT, let it extract command-line
	 GLUT options that you may provide */
	glutInit(&argc, argv);
	
	//Use double buffering!
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	
	width = glutGet(GLUT_SCREEN_WIDTH);
	height = 0.75*glutGet(GLUT_SCREEN_HEIGHT);
	
	/* set the initial window size */
	glutInitWindowSize(width, height);
	
	/* set the initial window position */
	glutInitWindowPosition(posX,posY);
	
	/* create the window and store the handle to it */
	mainWindow = glutCreateWindow("Rendering InfoVis" /* title */ );
	
	glutIdleFunc(idle);
	
	/* register function to handle window resizes */
	glutReshapeFunc(reshape);
	
	// display and idle function
	glutDisplayFunc(display);
	
	/* --- register callbacks with GLUT ---     */
	
	/* register function that handles mouse */
	
	glutSpecialFunc(keys);
	
	setupDataController(argc, argv);
	
	createViews();
	
	/* Transparency stuff */
	glEnable (GL_BLEND);
	
	glClearColor(1,1,1, 0.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	/* start the GLUT main loop */
	glutMainLoop();
	
	return EXIT_SUCCESS;
}

