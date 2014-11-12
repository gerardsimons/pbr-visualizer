//
//  graphics_helper.h
//  Afstuderen
//
//  Created by Gerard Simons on 19/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

//Some useful methods for debugging, drawing of cubes and faces

#ifndef Afstuderen_graphics_helper_h
#define Afstuderen_graphics_helper_h



//function to draw coordinate axes with a certain length (1 as a default)
void drawCoordSystem(float length=1)
{
	//draw simply colored axes
	
	//remember all states of the GPU
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//deactivate the lighting state
	glDisable(GL_LIGHTING);
	//draw axes
	glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(length,0,0);
    
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,length,0);
    
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,length);
	glEnd();
	
	//reset to previous state
	glPopAttrib();
}

void drawUnitFace()
{
	//1) draw a unit quad in the x,y plane oriented along the z axis
	//2) make sure the orientation of the vertices is positive (counterclock wise)
	//3) What happens if the order is inversed?
	
	//Define the color
	glBegin(GL_QUADS);
    glNormal3d(0, 0, 1);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    glVertex3f(1,1,0);
    glVertex3f(0,1,0);
	glEnd();
}

void drawUnitCube()
{
	//1) draw a cube using your function drawUnitFace
	//rely on glTranslate, glRotate, glPushMatrix, and glPopMatrix
	//the latter two influence the model matrix, as seen during the course.
	//glPushMatrix stores the current matrix and puts a copy on
	//the top of a stack.
	//glPopMatrix pops the top matrix on the stack
	
	//Red
	//glColor3f(1.0,0.0,0.0);
	drawUnitFace();
	glPushMatrix();
	//Rotate over X-axis
	glRotatef(90.0,1.0,0.0,0.0);
    
	glTranslatef(0.0,-1.0,0.0);
	//Green
	//glColor3f(0.0,1.0,0.0);
	drawUnitFace();
    
	glRotatef(90.0,1.0,0.0,0.0);
    
	glTranslatef(0.0,-1.0,0.0);
	//Blue
	//glColor3f(0.0,0.0,1.0);
	drawUnitFace();
    
	//Return to origin
	glPopMatrix();
	
	//Yellow
	glPushMatrix();
	//glColor3f(1.0,1.0,0.0);
	glRotatef(90.0,-1.0,0.0,0.0);
	glTranslatef(0.0,0.0,1.0);
	drawUnitFace();
    
	glPopMatrix();
    
	//Cyan
	glPushMatrix();
	//glColor3f(0.0,1.0,1.0);
	//Rotate over y-axis
 	//glPushMatrix();
	glRotatef(90.0,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,1.0);
	drawUnitFace();
	glPopMatrix();
    
    
	glPushMatrix();
	glTranslatef(0.0,0.0,-1.0);
	glRotatef(90.0,0.0,-1.0,0.0);
	//White
	//glColor3f(1.0,1.0,1.0);
	drawUnitFace();
	
	glPopMatrix();
}

void drawArm()
{
	//produce a three-unit arm (upperarm, forearm, hand) making use of your
	//function drawUnitCube to define each of them
	//1) define 3 global variables that control the angles
	glScalef(0.5,0.5,0.5);
	//Generate 3 blocks
	glColor3f(1.0,0.0,0.0);
	glPushMatrix();
    
	glScalef(1.0,1.5,1.0); //Make it elongated
	drawUnitCube();
	glPopMatrix();
	glRotatef(0,0.0,0.0,1.0);
	glPushMatrix();
	float widthDifference = 0.05; //Do this to prevent artifacts.
	glTranslatef(0.9,0.0,-widthDifference / 2.0);
	glScalef(1.5,1.0,1.0 - widthDifference); //Make it elongated
	glColor3f(0.0,1.0,0.0);
	drawUnitCube();
	glPopMatrix();
    
	//DRAW HAND
	glPushMatrix();
	glRotatef(0,0.0,0.0,1.0);
	glTranslatef(2.25,0.0,-widthDifference);
	glScalef(0.5,0.8,1.0 - 2 * widthDifference);
	glColor3f(0.0,0.0,1.0);
	drawUnitCube();
	glPopMatrix();
    
	//between the arm parts
	//and add cases to the keyboard function to control these values
	
	//2) use these variables to define your arm
	//use glScalef to achieve different arm length
	//use glRotate/glTranslate to correctly place the elements
	
	//3 optional) make an animated snake out of these boxes
	//(an arm with 10 joints that moves using the animate function)
	
	
}


#endif
