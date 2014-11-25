	#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <exception>

#include "Data/Filter.h"
#include "Views/DataView.h"
#include "Views/ParallelCoordsView.h"
#include "Data/DataSet.h"
#include "Views/ImageView.h"
#include "Data/DataFileReader.h"
#include "Views/3DView.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/ColorProperty.h"
#include "Graphics/SizeProperty.h"
#include "DataController.h"
#include "Views/HeatMapView.h"
#include "Octree/Octree.h"

//EMBREE STUFF
#include "devices/device_singleray/embree_renderer.h"
#include "devices/device_singleray/dataconnector.h"
//#include "sandbox.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//const float DEG2RAD = 3.14159/180;

/* window width and height */
//int width = 1650;
//int height = 1000;

int padding = 10;

int width = 1450;
int height = 850;

bool isDirty = true;

/* window position */
int posX = 0;
int posY = 0;

int mainWindow;                   /* GLUT window handle */

/* All the sub window handles of the custom views */
int imageViewWindow;
int sceneViewWindow;
int parallelViewWindow;
int heatMapViewWindow;

/* For debugging purposes, keep track of mouse location */
int lastMouseX,lastMouseY = 0;

RIVImageView *imageView = NULL;
RIV3DView *sceneView = NULL;
ParallelCoordsView *parallelCoordsView = NULL;
RIVHeatMapView *heatMapView = NULL;

RIVDataSet* dataset;

/* The dataset, views have pointers to this in order to draw their views consistently */
DataController* dataController;

EMBREERenderer* rendererOne = NULL;
EMBREERenderer* rendererTwo = NULL;

/* Draw the window - this is where all the GL actions are */
void display(void)
{
    printf("Main display function called.\n");
    // Clear frame buffer
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void testFunctions() {
    //    RangeFilter *bounceNrs = new RangeFilter("bounce#",0,1);
    //    RIVTable *intersectionstTable = dataset.GetTable("intersections");
    //    intersectionstTable->AddFilter(bounceNrs);
    
    //    RangeFilter *throughputFilter = new RangeFilter("throughput 3",0.17,0.30);
//    riv::RangeFilter *intersectionsNr = new riv::RangeFilter("#intersections",0.5,1.5);
//    RIVTable *intersectionstTable = dataset.GetTable("path");
//    intersectionstTable->AddFilter(intersectionsNr);
	
//	RIVFloatRecord *testRecord = new RIVFloatRecord("test");
//	
//	const int N = 1000;
//	for(int i = 0 ; i < N ; ++i) {
//		testRecord->AddValue(rand());
//	}
//	
	
	//Done 
}

void keys(int keyCode, int x, int y) {
    //    printf("Pressed %d at (%d,%d)\n",keyCode,x,y);
    bool postRedisplay = true;
    float camSpeed = .1F;
    switch(keyCode) {
        case 27: //ESC key
            printf("Clear filters\n");
            //            invalidateAllViews();
			dataset->StartFiltering();
            dataset->ClearFilters();
			dataset->StopFiltering();
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
        case 114: // 'r' key, recluster {
        {
			printf("Key not set.\n");
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
//			riv::Filter* objectFilter = new riv::DiscreteFilter("object ID",1);
//			dataset.AddFilter("path",objectFilter);
			
			std::vector<riv::Filter*> allFilters;
			std::vector<ushort> selectedObjectIDs;
			selectedObjectIDs.push_back(3);
//			selectedObjectIDs.push_back(1);
//			selectedObjectIDs.push_back(1);
//			selectedObjectIDs.push_back(1);
//			selectedObjectIDs.push_back(1);
			
			for(int i = 0 ; i < selectedObjectIDs.size() ; ++i) {
				riv::Filter* objectFilter = new riv::DiscreteFilter("object ID",selectedObjectIDs[i]);
				riv::Filter* bounceFilter = new riv::DiscreteFilter("bounce#",i+1);
				std::vector<riv::Filter*> fs;
				fs.push_back(objectFilter);
				fs.push_back(bounceFilter);
				allFilters.push_back(new riv::ConjunctiveFilter(fs));
			}
			riv::GroupFilter* pathCreationFilter = new riv::GroupFilter(allFilters,dataset->GetTable("path"));
			riv::DiscreteFilter* bounceOneFilter = new riv::DiscreteFilter("bounce#",1);
			pathCreationFilter->Print();
			printf("\n");
			dataset->StartFiltering();
			dataset->AddFilter("path", pathCreationFilter);
			dataset->StopFiltering();
			dataset->StartFiltering();
			dataset->AddFilter("intersections", bounceOneFilter);
			dataset->StopFiltering();
			postRedisplay = false;
            break;
        }
        case 119: // 'w' key, move camera in Y direction
            sceneView->MoveCamera(0,camSpeed,0);
            break;
        case 115: // 's' key
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
	int newHeightPC = height/2-2*padding;
    glutReshapeWindow(newWidthPC,newHeightPC); //Upper half and full width of the main window
    //
    //    //image view window
    glutSetWindow(imageViewWindow);
    glutPositionWindow(padding, height/2+padding);
    glutReshapeWindow(height / 2 - 2 * padding, height /2 - 2 *padding); //Square bottom left corner
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

bool finished = false;

void idle() {

	if(currentFrame < maxFrames && !dataController->IsPaused()) {
		++currentFrame;
		printf("Rendering frame %d.\n",currentFrame);
		rendererOne->RenderNextFrame();
		glutPostRedisplay();
	}
	else if(!finished) {
		printf("Done......\n");
		finished = true;
	}
}

void processRendererOne(PathData* newPath) {
//	printf("New path from renderer #1 received!\n");
	dataController->ProcessNewPath(1,newPath);
}

void processRendererTwo(PathData* newPath) {
	printf("New path from renderer #2 received!\n");
	dataController->ProcessNewPath(2,newPath);
}

void setupDataController(const int argc, char** argv) {
	//Create the EMBREE renderer
	dataController = new DataController();
	dataset = dataController->GetDataSet();
	if(argc == 2) { //Just one renderer
		DataConnector* connector = new DataConnector(processRendererOne);
		rendererOne = new EMBREERenderer(connector, std::string(argv[1]));
		printf("1 renderer set up.\n");
	}
	else if(argc == 3) {
		rendererOne = new EMBREERenderer(new DataConnector(processRendererOne), std::string(argv[1]));
		rendererTwo = new EMBREERenderer(new DataConnector(processRendererTwo), std::string(argv[2]));
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

void createViews() {

	RIVSizeProperty *defaultSizeProperty = new RIVFixedSizeProperty(0.1);
	RIVColorProperty *defaultColorProperty = new RIVFixedColorProperty(1,1,1);
	
	RIVTable *intersectionsTable = dataset->GetTable("intersections");
	RIVTable *pathsTable = dataset->GetTable("paths");
	
	riv::ColorMap jetColorMap = colors::jetColorMap();
	RIVColorProperty *intersectionColor = new RIVColorRGBProperty<float>(intersectionsTable,"R","G","B");
	RIVColorProperty *pathColor = new RIVColorRGBProperty<float>(pathsTable,"R","G","B");
	//		RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,intersectionsTable->GetRecord("bounce#"),jetColorMap);
	//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(pathTable,"radiance R","radiance G","radiance B");
	RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(2);
	
	
	parallelViewWindow = glutCreateSubWindow(mainWindow,padding,padding,width-2*padding,height/2.F-2*padding);
	ParallelCoordsView::windowHandle = parallelViewWindow;
	glutSetWindow(parallelViewWindow);
	glutDisplayFunc(ParallelCoordsView::DrawInstance);
//	glutDisplayFunc(doNothing);
	glutReshapeFunc(ParallelCoordsView::ReshapeInstance);
	glutMouseFunc(ParallelCoordsView::Mouse);
	glutMotionFunc(ParallelCoordsView::Motion);
	glutSpecialFunc(keys);
	
	//Load image
	float bottomHalfY = height / 2.f + padding;
	float squareSize = height / 2.F - 2 * padding;
	//		image = new BMPImage(bmpPath.c_str(),false);
	glutSetWindow(imageViewWindow);
	imageViewWindow = glutCreateSubWindow(mainWindow,padding,bottomHalfY,squareSize,squareSize);
	imageView = new RIVImageView(dataset,rendererOne,defaultColorProperty,defaultSizeProperty);
	RIVImageView::windowHandle = imageViewWindow;
	glutDisplayFunc(RIVImageView::DrawInstance);
	glutReshapeFunc(RIVImageView::ReshapeInstance);
	glutMouseFunc(RIVImageView::Mouse);
	glutMotionFunc(RIVImageView::Motion);
	glutSpecialFunc(keys);
	
	sceneViewWindow = glutCreateSubWindow(mainWindow, padding * 3 + squareSize, bottomHalfY, squareSize, squareSize);
	RIV3DView::windowHandle = sceneViewWindow;
	glutSetWindow(sceneViewWindow);
	glutDisplayFunc(RIV3DView::DrawInstance);
//	glutDisplayFunc(doNothing);
	glutReshapeFunc(RIV3DView::ReshapeInstance);
	glutMouseFunc(RIV3DView::Mouse);
	glutMotionFunc(RIV3DView::Motion);
	glutSpecialFunc(keys);
	//
	//        heatMapViewWindow = glutCreateSubWindow(mainWindow, padding * 5 + squareSize * 2, bottomHalfY, squareSize, squareSize);
	//        glutSetWindow(heatMapViewWindow);
	//        glutReshapeFunc(RIVHeatMapView::ReshapeInstance);
	//        glutDisplayFunc(RIVHeatMapView::DrawInstance);
	//        glutMouseFunc(RIVHeatMapView::Mouse);
	//        glutMotionFunc(RIVHeatMapView::Motion);
	
	//Create views
	parallelCoordsView = new ParallelCoordsView(dataset,pathColor,intersectionColor,sizeProperty);
	sceneView = new RIV3DView(dataset,rendererOne,intersectionColor,sizeProperty);
	//        heatMapView = new RIVHeatMapView(&dataset);
	
	//Add some filter callbacks
	dataset->AddFilterListener(sceneView);
	dataset->AddFilterListener(parallelCoordsView);
}

int main(int argc, char **argv)
{
    printf("Initialising Rendering InfoVis...\n");

	testFunctions();
	
    srand(time(NULL));
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide */
    glutInit(&argc, argv);
    
    //Use double buffering!
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    
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
    //    glutIdleFunc(renderSceneAll);
    
    /* --- register callbacks with GLUT ---     */
    
    /* register function that handles mouse */
	
    glutSpecialFunc(keys);
	
	setupDataController(argc, argv);
	
//    loadData();
	
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

