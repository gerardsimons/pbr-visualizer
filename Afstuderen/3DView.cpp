//
//  3DView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 19/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "3DView.h"

#include <GLUT/glut.h>

std::vector<Point3D> vertices;

Point3D eye;


RIV3DView::RIV3DView(int x, int y, int width, int height, int paddingX, int paddingY) : RIVDataView(x,y,width,height,paddingX,paddingY) {
    
};

GLfloat xRotated = 0.F;
GLfloat yRotated = 0.F;
GLfloat zRotated = 0.F;

void RIV3DView::ComputeLayout() {
    eye.z = - 10.5F;
}

//void RIV3DView::Animate() {
//    
//}

void RIV3DView::Draw() {
    
    printf("Drawing 3D view.\n");
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0
    
    gluPerspective(40.0,(GLdouble)width/(GLdouble)height,0.5,20.0);
//    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glViewport(startX,startY,width,height);  //Use the whole window for rendering
    
    //Color the view port
    glColor3f(0.F,0.F,1.F);
    glBegin(GL_QUADS);
    glVertex3f(0,0,-2.F);
    glVertex3f(width,0,-2.F);
    glVertex3f(width,height,-2.F);
    glVertex3f(0,height,-2.F);
    glEnd();
    
//    return;
    
    // clear the drawing buffer.
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.0,0.0,eye.z); //Camera
//    glRotatef(xRotated,1.0,0.0,0.0);
    // rotation about Y axis
//    glRotatef(yRotated,0.0,1.0,0.0);
    // rotation about Z axis
//    glRotatef(zRotated,0.0,0.0,1.0);
    glBegin(GL_QUADS);        // Draw The Cube Using quads
    glColor3f(0.0f,1.0f,0.0f);    // Color Blue
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Bottom Right Of The Quad (Top)
    glColor3f(1.0f,0.5f,0.0f);    // Color Orange
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Bottom)
    glColor3f(1.0f,0.0f,0.0f);    // Color Red
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Front)
    glColor3f(1.0f,1.0f,0.0f);    // Color Yellow
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Top Right Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Top Left Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Bottom Right Of The Quad (Back)
    glColor3f(0.0f,0.0f,1.0f);    // Color Blue
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Left)
    glColor3f(1.0f,0.0f,1.0f);    // Color Violet
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Right)
    glEnd();            // End Drawing The Cube
    glFlush();
    
    glPopMatrix();
//    glPopMatrix();
}

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
    return false;
}

bool RIV3DView::HandleMouseMotion(int x, int y) {
    return false;
}
