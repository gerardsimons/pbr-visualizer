
#include "DataView.h"
#include "ParallelCoordsView.h"
#include "DataSet.h"
#include "ImageView.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* window width and height */
int width = 640; 
int height = 480;

/* window position */
int posX = 300;
int posY = 300;

int wd;                   /* GLUT window handle */

using namespace std;

/* Contains pointers to all the views to be drawn */
vector<RIVDataView*> views;

/* Callback functions for GLUT */

/* Draw the window - this is where all the GL actions are */
void display(void)
{
	printf("Display function called\n");
  /* clear the screen to white */
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw the views */
  for(size_t i = 0 ; i < views.size() ; i++) {
	  RIVDataView *view = views[i];
	  view->Draw();
  }

  glEnd();
  glFlush();
}

/* Handles mouse input */
void mouse(int button, int state, int x, int y) {
	//TODO: 
	printf("mouse click button=%d state=%d x=%d y=%d\n",button,state,x,y);
	glutPostRedisplay();
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

	RIVRecord recordOne("attribute1",data);
	testDataSet.AddRecord(recordOne);

	data.clear();
	data.push_back(1.F);
	data.push_back(2.F);
	data.push_back(21.F);
	data.push_back(22.F);
	//data.push_back(5.F);

	RIVRecord recordTwo("attribute2",data);
	testDataSet.AddRecord(recordTwo);

	data.clear();
	data.push_back(1.F);
	data.push_back(2.F);
	data.push_back(5.F);
	data.push_back(8.F);

	RIVRecord recordThree("attribute3",data);
    RIVRecord recordFour("attribute4",data);
	testDataSet.AddRecord(recordThree);
    testDataSet.AddRecord(recordFour);
    
    int imageWidth = 100;
    int imageHeight = 100;
    RIVImageView *imageView = new RIVImageView("/Users/gerardsimons/Git/Afstuderen/ufo_small.bmp",10,height / 2.F - imageHeight / 2.F,imageWidth,imageHeight);
    ParallelCoordsView *pview = new ParallelCoordsView(0,0,width, height);
    
	pview->SetData(testDataSet);
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
