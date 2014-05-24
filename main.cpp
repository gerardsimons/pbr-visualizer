#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "DataView.h"
#include "ParallelCoordsView.h"
#include "DataSet.h"
#include "ImageView.h"
#include "DataFileReader.h"
#include "3DView.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

const float DEG2RAD = 3.14159/180;

/* window width and height */
int width = 1400;
int height = 600;

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
    /* clear the screen to white */
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    /* Draw the views */
//    for(size_t i = 0 ; i < views.size() ; i++) {
//        views[i]->Draw();
//    }

    
    imageView->Draw();
    parallelCoordsView->Draw();
    sceneView->Draw();
    
    glColor3f(1.F,0,0);
    
    float radius = 10.F;
    
    glBegin(GL_LINE_LOOP);
    
    for (int i=0; i < 360; i++)
    {
        float degInRad = i*DEG2RAD;
        glVertex2f(cos(degInRad)*radius + lastMouseX,sin(degInRad)*radius + lastMouseY);
    }
    
    glEnd();
    glFlush();
    
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
//            printf("View %s caught the mouse interaction\n",typeid(views[i]).name());
//            if(state == GLUT_UP)
			glutPostRedisplay();
			return;
		}
	}
}

void motion(int x, int y) {
	y = height - y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouseMotion(x,y)) {
//            printf("View %s caught the motion interaction\n",typeid(views[i]).name());
			glutPostRedisplay();
			return;
		}
	}
}

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
    
    /* Invalidate all views */
    invalidateAllViews();
}

void initialize(int argc, char* argv[]) {
    std::string fullPath;
    std::string pbrtPath;
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
    }
    else if(argc == 3) { //Explicit PBRT file path defined
        fullPath = argv[1];
        pbrtPath = argv[2];
    }
    
    printf("using fullpath = %s\n",fullPath.c_str());
    
//	int imageWidth = (.2F * width);
//    int imageHeight = (.2F * width);
    
    int imageWidth = 0.1F * width;
    int imageHeight = 0.1F * width;
    
    int imageSceneWidth = .3F * width;
    int imageSceneHeight = .3F * width;
    
	//dataset = DataFileReader::ReadBinaryData(fullPath + ".bin");
    dataset = DataFileReader::ReadAsciiData(fullPath + ".txt",100000);
    std::vector<float> modelData;
    
    modelData = DataFileReader::ReadModelData(pbrtPath);
    /* dataset = &loadData(resourcesPath + fileName + ".bin"); */
    
	//CAUTION: Image should be power of two!
//	RIVImageView *imageView = new RIVImageView(fullPath + ".bmp",0,0,imageWidth,imageHeight,0,0);
//	ParallelCoordsView *parallelCoordsView = new ParallelCoordsView(imageWidth,0,width-imageWidth-imageSceneWidth,height,50,20);
//    RIV3DView *sceneView = new RIV3DView(width-imageSceneWidth,0,imageSceneWidth,imageSceneHeight,0,0);
    
    imageView = new RIVImageView(fullPath + ".bmp",0,0,imageWidth,imageHeight,0,0);
	parallelCoordsView = new ParallelCoordsView(imageWidth,0,width-imageWidth-imageSceneWidth,height,50,20);
    sceneView = new RIV3DView(width-imageSceneWidth,0,imageSceneWidth,imageSceneHeight,0,0);
    
//    sceneView = new RIV3DView(0,0,width,height,0,0);
    
	imageView->SetData(&dataset);
	parallelCoordsView->SetData(&dataset);
    sceneView->SetData(&dataset);
    
    sceneView->SetModelData(modelData);
    
	imageView->ComputeLayout();
    parallelCoordsView->ComputeLayout();
    sceneView->ComputeLayout();
	
    views.push_back(imageView);
    views.push_back(parallelCoordsView);
    views.push_back(sceneView);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide */
    glutInit(&argc, argv);
    
    //Use double buffering!
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
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
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glutMotionFunc(motion);
    
    //Enable when animations are required
//    glutIdleFunc(idle);
    
    /* start the GLUT main loop */
    glutMainLoop();
    
    
    return EXIT_SUCCESS;
}
