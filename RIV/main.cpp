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
			dataset.StartFiltering();
            dataset.ClearFilters();
			dataset.StopFiltering();
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
			printf("Don't press that button!\n");
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
			selectedObjectIDs.push_back(1);
			selectedObjectIDs.push_back(1);
			selectedObjectIDs.push_back(1);
			selectedObjectIDs.push_back(1);
			for(int i = 0 ; i < selectedObjectIDs.size() ; ++i) {
				riv::Filter* objectFilter = new riv::DiscreteFilter("object ID",selectedObjectIDs[i]);
				riv::Filter* bounceFilter = new riv::DiscreteFilter("bounce#",i+1);
				std::vector<riv::Filter*> fs;
				fs.push_back(objectFilter);
				fs.push_back(bounceFilter);
				allFilters.push_back(new riv::ConjunctiveFilter(fs));
			}
			riv::GroupFilter* pathCreationFilter = new riv::GroupFilter(allFilters,dataset.GetTable("path"));
			pathCreationFilter->Print();
			printf("\n");
			dataset.StartFiltering();
			dataset.AddFilter("path", pathCreationFilter);
			dataset.StopFiltering();
			postRedisplay = false;
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
//    glutSetWindow(parallelViewWindow);
//    glutPositionWindow(padding,padding);
//	int newWidthPC = width-2*padding;
//	int newHeightPC = height/2-2*padding;
//	printf("New width = %d\n",newWidthPC);
//	printf("New height = %d\n",newHeightPC);
//    glutReshapeWindow(newWidthPC,newHeightPC); //Upper half and full width of the main window
    //
    //    //image view window
//    glutSetWindow(imageViewWindow);
//    glutPositionWindow(padding, height/2+padding);
//    glutReshapeWindow(height / 2 - 2 * padding, height /2 - 2 *padding); //Square bottom left corner
//    //
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
		
		RIVTable *intersectionsTable = dataset.GetTable("intersections");
		
		ColorMap jetColorMap = colors::jetColorMap();
		RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(intersectionsTable,"spectrum R","spectrum G","spectrum B");
//		RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,intersectionsTable->GetRecord("bounce#"),jetColorMap);
		//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(pathTable,"radiance R","radiance G","radiance B");
		RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(2);
		
		
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
        image = new BMPImage(bmpPath.c_str(),false);
        glutSetWindow(imageViewWindow);
        float bottomHalfY = height / 2.f + padding;
        float squareSize = height / 2.F - 2 * padding;
		imageViewWindow = glutCreateSubWindow(mainWindow,padding,bottomHalfY,squareSize,squareSize);
		imageView = new RIVImageView(&dataset,image,defaultColorProperty,defaultSizeProperty);
		imageView->InitializeGraphics();
        glutDisplayFunc(RIVImageView::DrawInstance);
        glutReshapeFunc(RIVImageView::ReshapeInstance);
        glutMouseFunc(RIVImageView::Mouse);
        glutMotionFunc(RIVImageView::Motion);
        glutSpecialFunc(keys);

        sceneViewWindow = glutCreateSubWindow(mainWindow, padding * 3 + squareSize, bottomHalfY, squareSize, squareSize);
		RIV3DView::windowHandle = sceneViewWindow;
        glutSetWindow(sceneViewWindow);
        glutDisplayFunc(RIV3DView::DrawInstance);
        glutReshapeFunc(RIV3DView::ReshapeInstance);
        glutMouseFunc(RIV3DView::Mouse);
        glutMotionFunc(RIV3DView::Motion);
        glutSpecialFunc(keys);
//
        heatMapViewWindow = glutCreateSubWindow(mainWindow, padding * 5 + squareSize * 2, bottomHalfY, squareSize, squareSize);
        glutSetWindow(heatMapViewWindow);
        glutReshapeFunc(RIVHeatMapView::ReshapeInstance);
        glutDisplayFunc(RIVHeatMapView::DrawInstance);
        glutMouseFunc(RIVHeatMapView::Mouse);
        glutMotionFunc(RIVHeatMapView::Motion);
		
        //Create views
        parallelCoordsView = new ParallelCoordsView(&dataset,colorProperty,sizeProperty);
        sceneView = new RIV3DView(&dataset,config,colorProperty,sizeProperty);
        heatMapView = new RIVHeatMapView(&dataset);
		
        //Add some filter callbacks
        dataset.AddFilterListener(sceneView);
        dataset.AddFilterListener(parallelCoordsView);
        
        //Add the views to the view vector
        views.push_back(sceneView);
        views.push_back(imageView);
        views.push_back(parallelCoordsView);
        views.push_back(heatMapView);

    }
    else {
        throw "Data must be loaded first.";
    }
}

void clusterAndColor() {
    dataset.ClusterTable("intersections","intersection X","intersection Y","intersection Z",clusterK,1);
    std::vector<size_t> medoidIndices = dataset.GetClusterSet()->GetMedoidIndices();
    RIVTable *intersectionsTable = dataset.GetTable("intersections");
//    RIVEvaluatedColorProperty<size_t>* colorByCluster = new RIVEvaluatedColorProperty<size_t>(intersectionsTable);
    RIVClusterSet& clusterSet = intersectionsTable->GetClusterSet();
    
//    float nrOfClusters = (float)clusterSet.Size();
	
    std::map<size_t,float> indexToClusterSize;
    std::map<size_t,float> indexToClusterMembership;
    
    for(size_t j = 0 ; j < clusterSet.Size() ; ++j) {
//        float clusterRatio = j / (nrOfClusters  - 1.F);
        //        printf("clusterRatio = %f\n",clusterRatio);
//        Evaluator<size_t,float>* eval = new FixedEvaluator<size_t, float>(clusterRatio);
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
	
    glutSpecialFunc(keys);
	
    loadData();
    
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

