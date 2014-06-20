#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "Filter.h"
#include "DataView.h"
#include "ParallelCoordsView.h"
#include "DataSet.h"
#include "ImageView.h"
#include "DataFileReader.h"
#include "3DView.h"
#include "ColorPallete.h"
#include "ColorProperty.h"
#include "SizeProperty.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <gtest/gtest.h>

const float DEG2RAD = 3.14159/180;

/* window width and height */
//int width = 1680;
//int height = 1000;

int width = 1400;
int height = 800;

RIVClusterSet* clusters; //HERE ONLY FOR DEBUG REASONS
const size_t clusterK = 2;

bool isDirty = true;

/* window position */
int posX = 0;
int posY = 0;

int wd;                   /* GLUT window handle */

/* For debugging purposes, keep track of mouse location */
int lastMouseX,lastMouseY = 0;

/* Contains pointers to all the views to be drawn */
std::vector<RIVDataView*> views;

RIVImageView *imageView;
RIV3DView *sceneView;
ParallelCoordsView *parallelCoordsView;

/* The dataset, views have pointers to this in order to draw their views consistently */
RIVDataSet dataset;

/* Draw the window - this is where all the GL actions are */
void display(void)
{



//
//
    
    
    parallelCoordsView->Draw();
    imageView->Draw();
    sceneView->Draw();
    
//    glColor3f(1.F,0,0);
    
//    float radius = 10.F;
//    
    glBegin(GL_LINE_LOOP);
    float radius = 10.F;
    for (int i=0; i < 360; i++)
    {
        float degInRad = i*DEG2RAD;
        glVertex2f(cos(degInRad)*radius + lastMouseX,sin(degInRad)*radius + lastMouseY);
    }
    
    glEnd();
    glFlush();
    
//    copy_buffer();
    
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
			glutPostRedisplay();
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
			glutPostRedisplay();
			return;
		}
	}
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
    switch(keyCode) {
        case 27: //ESC key
            printf("Clear filters\n");
//            invalidateAllViews();
            dataset.ClearFilters();
            break;
        case 98: // 'b' key
            glutSwapBuffers();
//            copy_buffer();
            postRedisplay = false;
            break;
        case 99: // 'c' key
            if(sceneView) {
                sceneView->ToggleDrawClusterMembers();
                postRedisplay = true;
            }
            break;
        case 114: // 'r' key, recluster {
        {
            RIVTable *intersectTable = dataset.GetTable("intersections");
            intersectTable->Cluster("intersection X","intersection Y","intersection Z",clusterK,1);
            postRedisplay = true;
            break;
        }
        case 111: // 'o' key, optimize clusters (debug feature)
        {
            clusters->OptimizeClusters();
            postRedisplay = true;
            break;
        }
        case 116: // 't' key, use as temp key for some to-test function
        {
            imageView->createTextureImage();
            break;
        }
        case GLUT_KEY_UP:
            sceneView->MoveCamera(0,0,1.F);
            break;
        case GLUT_KEY_DOWN:
            sceneView->MoveCamera(0,0,-1.F);
            break;
        case GLUT_KEY_LEFT:
            sceneView->MoveCamera(-1.F,0,0);
            break;
        case GLUT_KEY_RIGHT:
            sceneView->MoveCamera(1.F,0,0);
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
    printf("reshape called.\n");
    /* save new screen dimensions */
    width = w;
    height = h;
    
    //Determine dimension and positions of other views
    
    //PCV goes on top spanning the whole width, half of the height
    if(parallelCoordsView != NULL) {
        parallelCoordsView->ComputeLayout(0,height / 2.F,width, height / 2.F,50,50);
    }
    //Image view goes bottom left, square height / 2 x height / 2
    //TODO: when the view is very narrow the width should be used
    if(imageView != NULL) {
//        imageView->ComputeLayout(0,0,height / 2.F, height / 2.F,0,0);
    }
    if(sceneView != NULL) {
        sceneView->ComputeLayout(height / 2.F,0,height / 2.F, height / 2.F,0,0);
    }
    
    /* Invalidate all views */
    invalidateAllViews();
}

void initialize(int argc, char* argv[]) {
    std::string fullPath;
    std::string pbrtPath;
    std::string bmpPath;
    printf("%d additional arguments given\n", argc - 1);
    if(argc <= 1) {
        //Default values
#ifdef _WIN32
        std::string resourcesPath = "../RenderingInfoVis/Resources/";
#elif __APPLE__
        std::string resourcesPath = "../../../Resources/";
#endif
        std::string fileName = "teapot32x32x1.exr";
        
        fullPath = resourcesPath + fileName;
    } else if(argc == 2) {
        fullPath = argv[1];
        pbrtPath = fullPath + ".pbrt";
        bmpPath = fullPath + ".bmp";
    }
    else if(argc == 3) { //Explicit PBRT file path defined
        fullPath = argv[1];
        pbrtPath = argv[2];
    }
    else if(argc == 4) {
        fullPath = argv[1];
        pbrtPath = argv[2];
        bmpPath = argv[3];
    }
    
    printf("using fullpath = %s\n",fullPath.c_str());
    
    BMPImage image = BMPImage((bmpPath).c_str(),false);
    
//    std::cout << image;
    
    dataset = DataFileReader::ReadAsciiData(fullPath + ".txt",image,0);
    
    //clustering

    //Set the vertices data from the PBRT file

    MeshModel model = DataFileReader::ReadModelData(pbrtPath);
    
	//CAUTION: Image should be power of two!
//	RIVImageView *imageView = new RIVImageView(fullPath + ".bmp",0,0,imageWidth,imageHeight,0,0);
//	ParallelCoordsView *parallelCoordsView = new ParallelCoordsView(imageWidth,0,width-imageWidth-imageSceneWidth,height,50,20);
//    RIV3DView *sceneView = new RIV3DView(width-imageSceneWidth,0,imageSceneWidth,imageSceneHeight,0,0);
    
//    RIVColorProperty *colorProperty = new RIVColorLinearProperty("path");
    
//    RIVRecord* redRecord = dataset.FindRecord("R");
//    RIVRecord* greenRecord = dataset.FindRecord("G");
//    RIVRecord* blueRecord = dataset.FindRecord("B");
//    
//    RIVColorProperty *colorProperty = new RIVColorRGBProperty("image",redRecord,greenRecord,blueRecord);
    RIVTable *imageTable = dataset.GetTable("image");
    RIVTable *pathTable = dataset.GetTable("path");
    RIVTable *intersectionstTable = dataset.GetTable("intersections");
    
//    imageTable->FilterRowsUnlinkedTo(pathTable);
    
    RIVRecord* xIsectRecord = intersectionstTable->GetRecord("intersection X");
    
//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty(pathTable,colors::GREEN,colors::RED);
    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionstTable,xIsectRecord,colors::GREEN,colors::RED);
    RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(.05F);

    //Declare views
//	parallelCoordsView = new ParallelCoordsView(0,imageHeight,width,height - imageHeight,50,20,colorProperty,sizeProperty);
//    imageView = new RIVImageView(image,0,0,imageWidth,imageHeight,0,0,colorProperty,sizeProperty);
//    sceneView = new RIV3DView(imageWidth,0,imageSceneWidth,imageSceneHeight,0,0,colorProperty,sizeProperty);
    
    parallelCoordsView = new ParallelCoordsView(colorProperty,sizeProperty);
//    parallelCoordsView->SetPadding(50,50);
    imageView = new RIVImageView(image, 0, 0, height / 2.F, height / 2.F, 0, 0, colorProperty, sizeProperty);
    sceneView = new RIV3DView(colorProperty,sizeProperty);
    
    //Set data source of views
	imageView->SetData(&dataset);
	parallelCoordsView->SetData(&dataset);
    sceneView->SetData(&dataset);
    
    sceneView->SetModelData(model);
    
    dataset.AddFilterListener(sceneView);
    dataset.AddFilterListener(parallelCoordsView);
	
    views.push_back(sceneView);
    views.push_back(imageView);
    views.push_back(parallelCoordsView);
}


void clusterAndColor() {
    dataset.ClusterTable("intersections","intersection X","intersection Y","intersection Z",clusterK,1);
    std::vector<size_t> medoidIndices = dataset.GetClusterSet()->GetMedoidIndices();
    RIVTable *intersectionsTable = dataset.GetTable("intersections");
    RIVEvaluatedColorProperty<size_t>* colorByCluster = new RIVEvaluatedColorProperty<size_t>(intersectionsTable);
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
        
        colorByCluster->AddEvaluationScheme(members, eval);
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
        Evaluator<size_t, float>* clusterSizeEval = new FixedEvaluator<size_t, float>((1-relativeSize) * minSize + relativeSize * maxSize);
        sizeByCluster->AddEvaluationScheme(cluster->GetMedoidIndex(), clusterSizeEval);
    }
    
    parallelCoordsView->SetColorProperty(colorByCluster);
    parallelCoordsView->SetSizeProperty(sizeByCluster);
    
    sceneView->SetColorProperty(colorByCluster);
    sceneView->SetSizeProperty(sizeByCluster);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide */
    glutInit(&argc, argv);
    
    //Use double buffering!
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    
    /* set the initial window size */
    glutInitWindowSize(width, height);
    
    /* set the initial window position */
    glutInitWindowPosition(posX,posY);
    
    /* create the window and store the handle to it */
    wd = glutCreateWindow("Rendering InfoVis" /* title */ );
    
    /* Initialize the data and the views */
    initialize(argc, argv);
    
    /* --- register callbacks with GLUT --- */
    
    /* register function to handle window resizes */
    glutReshapeFunc(reshape);
    
    /* register function that draws in the window */
    glutDisplayFunc(display);
    
    /* register function that handles mouse */
    glutMouseFunc(mouse);
    
    glutSpecialFunc(keys);
    
    /* Transparency stuff */
    glEnable (GL_BLEND);
    
    /* Enable depth for 3D view */
    glEnable(GL_DEPTH_TEST);
    
    glDrawBuffer(GL_FRONT); //Front is red
//    glClearColor(1,0,0, 0.0);
    glClearColor(1,1,1, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glDrawBuffer(GL_BACK);
    glClearColor(1,1,1, 0.0); //Back is green
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glutMotionFunc(motion);

    /* start the GLUT main loop */
    glutMainLoop();
    
    
    return EXIT_SUCCESS;
}
