#include "DataView.h"
#include "ParallelCoordsView.h"
#include "DataSet.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int width = 640; /* window width and height */
int height = 480;
int wd;                   /* GLUT window handle */

using namespace std;

/* Contains pointers to all the views to be drawn */
vector<DataView*> views;

/* Callback functions for GLUT */

/* Draw the window - this is where all the GL actions are */
void display(void)
{

  /* clear the screen to white */
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw the views */


  glEnd();
  glFlush();
}

/* Handles mouse input */
void mouse(int button, int state, int x, int y) {
	//TODO: 
}

/* Called when window is resized,
   also when window is first created,
   before the first call to display(). */
void reshape(int w, int h)
{
  /* save new screen dimensions */
  width = w;
  height = h;

  /* tell OpenGL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) width, (GLsizei) height);

  /* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, width, 0.0, height);
}

void initializeViews() {

	RIVDataSet testDataSet;

	vector<float> data;
	data.push_back(1.F);
	data.push_back(2.F);
	data.push_back(3.F);
	data.push_back(4.F);
	testDataSet.AddData("attribute1",&data);

	vector<float> moreData;
	data.push_back(10.F);
	data.push_back(21.F);
	data.push_back(22.F);
	data.push_back(4.F);
	testDataSet.AddData("attribute2",&moreData);

	ParallelCoordsView pview = ParallelCoordsView(0,0,500,200);
	pview.SetData(testDataSet);
}

int main(int argc, char *argv[])
{

  /* initialize GLUT, let it extract command-line 
     GLUT options that you may provide 
     - NOTE THE '&' BEFORE argc */
  glutInit(&argc, argv);

  /* specify the display to be single 
     buffered and color as RGBA values */
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

  /* set the initial window size */
  glutInitWindowSize(width, height);

  /* create the window and store the handle to it */
  wd = glutCreateWindow("Experiment with line drawing" /* title */ );

  /* Initialize the views */
  initializeViews();


  /* --- register callbacks with GLUT --- */

  /* register function to handle window resizes */
  glutReshapeFunc(reshape);

  /* register function that draws in the window */
  glutDisplayFunc(display);

  /* register function that handles mouse */
  glutMouseFunc(mouse);

  /* start the GLUT main loop */
  glutMainLoop();

  return EXIT_SUCCESS;
}
