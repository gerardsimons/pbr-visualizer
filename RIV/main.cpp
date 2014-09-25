#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

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
//#include "UIView.h" //
#include "Views/HeatMapView.h"

#include "Octree/Octree.h"
//#include "sandbox.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//const float DEG2RAD = 3.14159/180;

/* window width and height */
int width = 1600;
int height = 1000;

int padding = 10;

//int width = 1400;
//int height = 800;

RIVClusterSet* clusters; //HERE ONLY FOR DEBUG REASONS
const size_t clusterK = 2;

bool isDirty = true;

/* window position */
int posX = 0;
int posY = 0;

int mainWindow;                   /* GLUT window handle */

/* All the sub window handles of the custom views */
int imageViewWindow;
int sceneViewWindow;
int parallelViewWindow;
int uiViewWindow;
int heatMapViewWindow;

/* For debugging purposes, keep track of mouse location */
int lastMouseX,lastMouseY = 0;

/* Contains pointers to all the views to be drawn */
std::vector<RIVDataView*> views;

RIVImageView *imageView = NULL;
RIV3DView *sceneView = NULL;
ParallelCoordsView *parallelCoordsView = NULL;
RIVHeatMapView *heatMapView = NULL;
//UIView *uiView = NULL;

/* The dataset, views have pointers to this in order to draw their views consistently */
RIVDataSet dataset;

/* Image loaded as texture for the image view */
BMPImage *image;

/* The 3D model */
PBRTConfig* config;

std::string bmpPath = "";
std::string dataPath = "";
std::string pbrtPath = "";

/* Draw the window - this is where all the GL actions are */
void display(void)
{
    printf("Main display function called.\n");
    // Clear frame buffer
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Present frame buffer
    glutSwapBuffers();
}

/* Handles mouse input */
void mouse(int button, int state, int x, int y) {
	//Check what view catches the interaction
	y = height - y; //Very important to invert Y!
	lastMouseX = x;
	lastMouseY = y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouse(button,state,x,y)) {
            printf("View %s caught the MOUSE interaction\n",views[i]->identifier.c_str());
            //            if(state == GLUT_UP)
			return;
		}
	}
}

std::string lastMotionCatch = "";

void motion(int x, int y) {
	y = height - y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouseMotion(x,y)) {
            if(views[i]->identifier != lastMotionCatch) {
                printf("View %s caught the MOTION interaction\n",views[i]->identifier.c_str());
                lastMotionCatch = views[i]->identifier;
            }
			return;
		}
	}
}

void testFunctions() {
    //    RangeFilter *bounceNrs = new RangeFilter("bounce#",0,1);
    //    RIVTable *intersectionstTable = dataset.GetTable("intersections");
    //    intersectionstTable->AddFilter(bounceNrs);
    
    //    RangeFilter *throughputFilter = new RangeFilter("throughput 3",0.17,0.30);
    riv::RangeFilter *intersectionsNr = new riv::RangeFilter("#intersections",0.5,1.5);
    RIVTable *intersectionstTable = dataset.GetTable("path");
    intersectionstTable->AddFilter(intersectionsNr);
}

///* Handles mouse input */
//void mouse(int button, int state, int x, int y) {
//    sceneView->HandleMouse(button, state, x, y);
//}

//void motion(int x, int y) {
//	sceneView->HandleMouseMotion(x, y);
//}

void invalidateAllViews() {
	for(size_t i = 0 ; i < views.size() ; i++) {
        views[i]->Invalidate();
    }
}

void keys(int keyCode, int x, int y) {
    //    printf("Pressed %d at (%d,%d)\n",keyCode,x,y);
    bool postRedisplay = true;
    float camSpeed = .1F;
    switch(keyCode) {
        case 27: //ESC key
            printf("Clear filters\n");
            //            invalidateAllViews();
            dataset.ClearFilters();
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
			printf("Don't press that button!");
//            RIVTable *intersectTable = dataset.GetTable("intersections");
//            intersectTable->Cluster("intersection X","intersection Y","intersection Z",clusterK,1);
//            postRedisplay = true;
//            break;
        }
        case 111: // 'o' key, optimize clusters (debug feature)
        {
            clusters->OptimizeClusters();
            postRedisplay = true;
            break;
        }
        case 116: // 't' key, use as temp key for some to-test function
        {
//			riv::Filter* objectFilter = new riv::DiscreteFilter("object ID",1);
//			dataset.AddFilter("path",objectFilter);
			
			std::vector<riv::Filter*> allFilters;
			std::vector<ushort> selectedObjectIDs;
			selectedObjectIDs.push_back(1);
			selectedObjectIDs.push_back(2);
			for(int i = 0 ; i < selectedObjectIDs.size() ; ++i) {
				riv::Filter* objectFilter = new riv::DiscreteFilter("object ID",selectedObjectIDs[i]);
				riv::Filter* bounceFilter = new riv::DiscreteFilter("bounce#",i+1);
				std::vector<riv::Filter*> fs;
				fs.push_back(objectFilter);
				fs.push_back(bounceFilter);
				allFilters.push_back(new riv::ConjunctiveFilter(fs));
			}
			riv::Filter* pathCreationFilter = new riv::DisjunctiveFilter(allFilters,true);
			pathCreationFilter->Print();
			printf("\n");
			dataset.StartFiltering();
			dataset.AddFilter("path", pathCreationFilter);
			dataset.StopFiltering();
            break;
        }
        case 119: // 'w' key, move camera in Y direction
            sceneView->MoveCamera(0,camSpeed,0);
            break;
        case 115:
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

void idle() {
    //Do animations?
    
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
    glutReshapeWindow(width-2*padding,height/2-2*padding); //Upper half and full width of the main window
    //
    //    //image view window
    glutSetWindow(imageViewWindow);
    glutPositionWindow(padding, height/2+padding);
    glutReshapeWindow(height / 2 - 2*padding, height /2 - 2 *padding); //Square bottom left corner
    //
    //    //3D scene view inspector window
    glutSetWindow(sceneViewWindow);
    glutPositionWindow(padding + height/2, height/2+padding);
    glutReshapeWindow(height / 2 - 2*padding, height /2 - 2 *padding); //Square bottom right next to imageview window

    //Heat map view
    glutSetWindow(heatMapViewWindow);
    glutPositionWindow(padding + height, height / 2 + padding);
    glutReshapeWindow(height / 2 - 2*padding, height /2 - 2 *padding);
    
    //UI view window
    glutSetWindow(uiViewWindow);
    glutPositionWindow(padding + 1.5f * height, height / 2 + padding);
    glutReshapeWindow(height /2 - 2*padding, height/2 - 2 *padding);
    

}

void generatePaths(int argc, char* argv[]) {
    if(argc < 4) {
        throw "Too few arguments given, 3 are expected.";
    }
    if(argc == 4) {
        dataPath = argv[1];
        pbrtPath = argv[2];
        bmpPath = argv[3];
    }
}

void loadData() {
    if(!dataPath.empty() && !pbrtPath.empty()) {
        image = new BMPImage(bmpPath.c_str(),false);
        //        dataset = DataFileReader::ReadAsciiData(dataPath + ".txt",image,0);
        dataset = DataFileReader::ReadBinaryData(dataPath,image,0);
        config = new PBRTConfig(DataFileReader::ReadPBRTFile(pbrtPath));
		
		//Extra data processing
//		DataFileReader::AssignShapeIDsToPrimitives(dataset.GetTable("intersections"), model);
//		testPBRTParser(pbrtPath);
    }
    else throw "Data paths not generated.";
}

void createViews() {
    if(dataset.IsSet() && bmpPath.size() > 0) {
        RIVSizeProperty *defaultSizeProperty = new RIVFixedSizeProperty(0.1);
        RIVColorProperty *defaultColorProperty = new RIVFixedColorProperty(1,1,1);
        
        parallelViewWindow = glutCreateSubWindow(mainWindow,padding,padding,width-2*padding,height/2.F-2*padding);
		ParallelCoordsView::windowHandle = parallelViewWindow;
        glutSetWindow(parallelViewWindow);
        glutDisplayFunc(ParallelCoordsView::DrawInstance);
//        glutDisplayFunc(idle);
        glutReshapeFunc(ParallelCoordsView::ReshapeInstance);
        glutMouseFunc(ParallelCoordsView::Mouse);
        glutMotionFunc(ParallelCoordsView::Motion);
        glutSpecialFunc(keys);
        
        //Load image
//        image = new BMPImage(bmpPath.c_str(),false);
        //image view window
        imageViewWindow = glutCreateSubWindow(mainWindow,0,0,width,height / 2.F);
        glutSetWindow(imageViewWindow);
        float bottomHalfY = height / 2.f + 2.F * padding;
        float squareSize = height / 2.F - 2 * padding;
        imageView = new RIVImageView(&dataset,image, padding, bottomHalfY, squareSize, squareSize, 0, 0, defaultColorProperty, defaultSizeProperty); //If this is not supplied on constructor, the texture becomes garbled
        glutDisplayFunc(RIVImageView::DrawInstance);
//        glutDisplayFunc(idle);
        glutReshapeFunc(RIVImageView::ReshapeInstance);
        glutMouseFunc(RIVImageView::Mouse);
        glutMotionFunc(RIVImageView::Motion);
        glutSpecialFunc(keys);
        //
//        sceneViewWindow = glutCreateSubWindow(mainWindow, padding * 3 + squareSize * 2, bottomHalfY, squareSize, squareSize);
		sceneViewWindow = glutCreateSubWindow(mainWindow, 0,0,0,0);
		RIV3DView::windowHandle = sceneViewWindow;
        glutSetWindow(sceneViewWindow);
        glutDisplayFunc(RIV3DView::DrawInstance);
//        glutDisplayFunc(idle);
        glutReshapeFunc(RIV3DView::ReshapeInstance);
        glutMouseFunc(RIV3DView::Mouse);
        glutMotionFunc(RIV3DView::Motion);
        glutSpecialFunc(keys);
        //
        uiViewWindow = glutCreateSubWindow(mainWindow, padding * 7 + squareSize * 4, bottomHalfY, squareSize, squareSize);
        glutSetWindow(uiViewWindow);
//        glutReshapeFunc(UIView::ReshapeInstance);
//        glutDisplayFunc(UIView::DrawInstance);
        glutDisplayFunc(idle);
//        glutMouseFunc(UIView::Mouse);
//        glutMotionFunc(UIView::Motion);
//        glutEntryFunc(UIView::Entry);
        
        heatMapViewWindow = glutCreateSubWindow(mainWindow, padding * 5 + squareSize * 3, bottomHalfY, squareSize, squareSize);
        glutSetWindow(heatMapViewWindow);
        glutReshapeFunc(RIVHeatMapView::ReshapeInstance);
        glutDisplayFunc(RIVHeatMapView::DrawInstance);
//        glutDisplayFunc(RIV3DView::DrawInstance);
        glutMouseFunc(RIVHeatMapView::Mouse);
        glutMotionFunc(RIVHeatMapView::Motion);
        
        //Create views
        parallelCoordsView = new ParallelCoordsView(&dataset);
        sceneView = new RIV3DView(&dataset,config);
//        uiView = new UIView(defaultColorProperty,defaultSizeProperty);
        heatMapView = new RIVHeatMapView(&dataset);
        
        //Set data
        imageView->SetData(&dataset);
        parallelCoordsView->SetData(&dataset);
        sceneView->SetData(&dataset);
        heatMapView->SetData(&dataset);
        
        //Set the 3D model loaded from the PBRT file
//        sceneView->SetModelData(model);
        
        //Add some filter callbacks
        dataset.AddFilterListener(sceneView);
        dataset.AddFilterListener(parallelCoordsView);
        
        //Add the views to the view vector
        views.push_back(sceneView);
        views.push_back(imageView);
        views.push_back(parallelCoordsView);
        views.push_back(heatMapView);
		
		sceneView->InitializeGraphics();
    }
    else {
        throw "Data must be loaded first.";
    }
}

void initializeViewProperties() {
//    RIVTable *imageTable = dataset.GetTable("image");
    RIVTable *pathTable = dataset.GetTable("path");
    RIVTable *intersectionsTable = dataset.GetTable("intersections");
    
    //    imageTable->FilterRowsUnlinkedTo(pathTable);
    
    RIVRecord* bounceRecord = intersectionsTable->GetRecord("bounce#");
    RIVRecord* xRecord = intersectionsTable->GetRecord("intersection X");
	
	RIVRecord* throughputOne = pathTable->GetRecord("throughput 1");
	RIVRecord* throughputTwo = pathTable->GetRecord("throughput 2");
	RIVRecord* throughputThree = pathTable->GetRecord("throughput 3");
	
	RIVRecord* spectrumR = intersectionsTable->GetRecord("spectrum 1");
	RIVRecord* spectrumG = intersectionsTable->GetRecord("spectrum 2");
	RIVRecord* spectrumB = intersectionsTable->GetRecord("spectrum 3");
    
//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty(pathTable,colors::GREEN,colors::RED);
    
    ColorMap jetColorMap = colors::jetColorMap();
    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,bounceRecord,jetColorMap);
//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,xRecord,jetColorMap);
//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(pathTable,throughputOne,throughputTwo,throughputThree);
//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(intersectionsTable,spectrumR,spectrumG,spectrumB);
    RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(2);
    
    parallelCoordsView->SetColorProperty(colorProperty);
    sceneView->SetSizeProperty(sizeProperty);
    sceneView->SetColorProperty(colorProperty);
    
	//Create the graphics primitives from the data
	sceneView->InitializeGraphics();
	imageView->InitializeGraphics();
}



void clusterAndColor() {
    dataset.ClusterTable("intersections","intersection X","intersection Y","intersection Z",clusterK,1);
    std::vector<size_t> medoidIndices = dataset.GetClusterSet()->GetMedoidIndices();
    RIVTable *intersectionsTable = dataset.GetTable("intersections");
//    RIVEvaluatedColorProperty<size_t>* colorByCluster = new RIVEvaluatedColorProperty<size_t>(intersectionsTable);
    RIVClusterSet& clusterSet = intersectionsTable->GetClusterSet();
    
    float nrOfClusters = (float)clusterSet.Size();
    
    std::map<size_t,float> indexToClusterSize;
    std::map<size_t,float> indexToClusterMembership;
    
    for(size_t j = 0 ; j < clusterSet.Size() ; ++j) {
        float clusterRatio = j / (nrOfClusters  - 1.F);
        //        printf("clusterRatio = %f\n",clusterRatio);
        Evaluator<size_t,float>* eval = new FixedEvaluator<size_t, float>(clusterRatio);
        RIVCluster* cluster = clusterSet.GetCluster(j);
        std::vector<size_t> members = cluster->GetMemberIndices();
        members.push_back(cluster->GetMedoidIndex());
        
//        colorByCluster->AddEvaluationScheme(members, eval);
    }
    
    //    DiscreteEvaluator<size_t, float> colorByClusterEvaluator = new DiscreteEvaluator<size_t, float>(indexToClusterMembership);
    
    RIVEvaluatedSizeProperty<size_t> *sizeByCluster = new RIVEvaluatedSizeProperty<size_t>(intersectionsTable,.05F);
    std::vector<float> relativeSizes;
    float minSize = .4F;
    float maxSize = .8F;
    for(RIVCluster* cluster : clusterSet.GetClusters()) {
        float relativeSize = clusterSet.RelativeSizeOf(cluster);
        printf("\nTotal size of cluster : %zu\n",cluster->Size());
        printf("Relative size of cluster : %f\n",relativeSize);
        //        relativeSizes.push_back(clusterSet.RelativeSizeOf(cluster));
        Evaluator<size_t, float>* clusterSizeEval = new FixedEvaluator<size_t, float>((1 - relativeSize) * minSize + relativeSize * maxSize);
        sizeByCluster->AddEvaluationScheme(cluster->GetMedoidIndex(), clusterSizeEval);
    }
    
//    parallelCoordsView->SetColorProperty(colorByCluster);
    parallelCoordsView->SetSizeProperty(sizeByCluster);
    
//    sceneView->SetColorProperty(colorByCluster);
    sceneView->SetSizeProperty(sizeByCluster);
}

int main(int argc, char **argv)
{
    generatePaths(argc, argv);
	
//	if(!Octree::Test()) {
//		return 0;
//	}

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
    
    /* register function to handle window resizes */
    glutReshapeFunc(reshape);
    
    // display and idle function
    glutDisplayFunc(display);
    //    glutIdleFunc(renderSceneAll);
    
    /* --- register callbacks with GLUT ---     */
    
    /* register function that handles mouse */
    glutMouseFunc(mouse);
    
    glutSpecialFunc(keys);
    
    glutMotionFunc(motion);
    
    loadData();
    
//    testFunctions();
    
    createViews();
    
    initializeViewProperties();
    
    /* Transparency stuff */
    glEnable (GL_BLEND);
    
    glClearColor(1,1,1, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* start the GLUT main loop */
    glutMainLoop();
    
    
    return EXIT_SUCCESS;
}

