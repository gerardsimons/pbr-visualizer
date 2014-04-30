#include "ParallelCoordsView.h"
#include "DataView.h"
#include <stdio.h>


#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

ParallelCoordsView::ParallelCoordsView(int x, int y, int width, int height) : RIVDataView(x,y,width,height) {

}


ParallelCoordsView::~ParallelCoordsView(void)
{
	//Additional deleting unique to parallel coords view
}

void ParallelCoordsView::Draw() {

	//Draw the axes
	glLineWidth(2.5);
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(15, 0, 0);
	glEnd();

	//Draw the lines

	//Draw the texts
}
