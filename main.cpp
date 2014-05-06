#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "DataView.h"
#include "ParallelCoordsView.h"
#include "DataSet.h"
#include "ImageView.h"
#include "DataFileReader.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



const float DEG2RAD = 3.14159/180;

/* window width and height */
int width = 1200; 
int height = 600;

/* window position */
int posX = 0;
int posY = 400;

int wd;                   /* GLUT window handle */

/* For debugging purposes, keep track of mouse location */
int lastMouseX,lastMouseY = 0;

using namespace std;

/* Contains pointers to all the views to be drawn */
vector<RIVDataView*> views;

/* The dataset, views have pointers to this in order to draw their views consistently */
RIVDataSet dataset;

/* Callback functions for GLUT */

/* Draw the window - this is where all the GL actions are */
void display(void)
{
  /* clear the screen to white */
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw the views */
  for(size_t i = 0 ; i < views.size() ; i++) {
	  views[i]->Draw();
  }

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

void invalidateAllViews() {
	for(size_t i = 0 ; i < views.size() ; i++) {
	  views[i]->Invalidate();
  }
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

  /* tell OpenGL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) width, (GLsizei) height);

  /* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, width, 0.0, height);

  invalidateAllViews();
}



void initialize() {

    #ifdef _WIN32
        std::string resourcesPath = "../RenderingInfoVis/Resources/";
    #elif __APPLE__
        std::string resourcesPath = "../../../Resources/";
    #endif
    
    std::string fileName = "teapot2x2x512.exr";
    
    DataFileReader::LoadData(resourcesPath + fileName + ".bin");


	vector<float> data;
	data.push_back(1.F);
	data.push_back(2.F);
	data.push_back(3.F);
	data.push_back(4.F);

	RIVRecord recordOne("# intersections",data);
	dataset.AddRecord(recordOne);

	data.clear();
	data.push_back(1.F);
	data.push_back(2.F);
	data.push_back(21.F);
	data.push_back(22.F);
	//data.push_back(5.F);

	RIVRecord recordTwo("throughput 1",data);
	dataset.AddRecord(recordTwo);

	data.clear();
	data.push_back(1.F);
	data.push_back(2.F);
	data.push_back(5.F);
	data.push_back(8.F);

	RIVRecord recordThree("throughput 2",data);
    RIVRecord recordFour("throughput 3",data);
	dataset.AddRecord(recordThree);
    dataset.AddRecord(recordFour);

	//Pixel x data
	data.clear();
	data.push_back(0.F);
	data.push_back(1.F);
	data.push_back(0.F);
	data.push_back(1.F);

	RIVRecord recordX("x",data);
	dataset.AddRecord(recordX);

	//Pixel y data
	data.clear();
	data.push_back(0.F);
	data.push_back(0.F);
	data.push_back(1.F);
	data.push_back(1.F);

	RIVRecord recordY("y",data);
	dataset.AddRecord(recordY);

    int imageWidth = 500;
    int imageHeight = 500;

	//CAUTION: Below path is Windows / Visual Studio Specific
	//CAUTION: Image should be power of two!
    //RIVImageView *imageView = new RIVImageView("../RenderingInfoVis/teapot128x128x512.ppm",0,0,imageWidth,imageHeight,0,0);     //PPM IMAGE
	RIVImageView *imageView = new RIVImageView(resourcesPath + fileName + ".bmp",0,0,imageWidth,imageHeight,0,0);     //BMP IMAGE
	ParallelCoordsView *pview = new ParallelCoordsView(imageWidth,0,width-imageWidth,height,50,20);

	imageView->SetData(&dataset);
	pview->SetData(&dataset);

	imageView->ComputeLayout();
    pview->ComputeLayout();
	
	views.push_back(pview);
    views.push_back(imageView);
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
  wd = glutCreateWindow("Experiment with line drawing" /* title */ );

  /* Initialize the data and the views */
  initialize();

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

  /* start the GLUT main loop */
  glutMainLoop();

  return EXIT_SUCCESS;
}
