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
int width = 1600;
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
    glClear(GL_COLOR_BUFFER_BIT);
    
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
    

}

/* Handles mouse input */
void mouse(int button, int state, int x, int y) {
	//Check what view catches the interaction
	y = height - y; //Very important to invert Y!
	lastMouseX = x;
	lastMouseY = y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouse(button,state,x,y)) {
			glutPostRedisplay();
			return;
		}
	}
}

void motion(int x, int y) {
	y = height - y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouseMotion(x,y)) {
			glutPostRedisplay();
			return;
		}
	}
}

void idle() {
    //Do animations?
}

void invalidateAllViews() {
//	for(size_t i = 0 ; i < views.size() ; i++) {
//        views[i]->Invalidate();
//    }
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
    
    /* tell OpenGL to use the whole window for drawing */
//    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    
    /* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
}

void initialize(int argc, char* argv[]) {
    std::string fullPath;
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
    }
    
    printf("using fullpath = %s\n",fullPath.c_str());
    
	int imageWidth = 250;
    int imageHeight = 250;
    
    int imageSceneWidth = 250;
    int imageSceneHeight = 250;
    
	//dataset = DataFileReader::ReadBinaryData(fullPath + ".bin");
    dataset = DataFileReader::ReadAsciiData(fullPath + ".txt");
    DataFileReader::ReadModelData(fullPath + ".pbrt");
    /* dataset = &loadData(resourcesPath + fileName + ".bin"); */
    
	//CAUTION: Image should be power of two!
//	RIVImageView *imageView = new RIVImageView(fullPath + ".bmp",0,0,imageWidth,imageHeight,0,0);
//	ParallelCoordsView *parallelCoordsView = new ParallelCoordsView(imageWidth,0,width-imageWidth-imageSceneWidth,height,50,20);
//    RIV3DView *sceneView = new RIV3DView(width-imageSceneWidth,0,imageSceneWidth,imageSceneHeight,0,0);
    
    imageView = new RIVImageView(fullPath + ".bmp",0,0,imageWidth,imageHeight,0,0);
	parallelCoordsView = new ParallelCoordsView(imageWidth,0,width-imageWidth-imageSceneWidth,height,50,20);
    sceneView = new RIV3DView(width-imageSceneWidth,0,imageSceneWidth,imageSceneHeight,0,0);
    
	imageView->SetData(&dataset);
	parallelCoordsView->SetData(&dataset);
    
	imageView->ComputeLayout();
    parallelCoordsView->ComputeLayout();
    sceneView->ComputeLayout();
	
    views.push_back(imageView);
    views.push_back(parallelCoordsView);
    views.push_back(sceneView);
}

int main(int argc, char *argv[])
{
    
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide */
    glutInit(&argc, argv);
    
    /* specify the display to be single
     buffered and color as RGBA values */
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    
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
