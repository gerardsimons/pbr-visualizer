//
//  3DView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 19/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "3DView.h"
#include "helper.h"
#include "Geometry.h"

#include <GLUT/glut.h>

RIV3DView::RIV3DView(int x, int y, int width, int height, int paddingX, int paddingY) : RIVDataView(x,y,width,height,paddingX,paddingY) {};

//TODO : Move these to the header file

GLfloat xRotated = 0.F;
GLfloat yRotated = 0.F;
GLfloat zRotated = 0.F;

Point3D cursorNear;
Point3D cursorFar;

float zNear = 1.F;
float zFar = 20.F;

Box3D selectionBox;

const float perspectiveAngle = 45.0F;

void RIV3DView::ComputeLayout() {
    eye.x = 0.F;
    eye.y = 0.F;
    eye.z = 0.F;
    
    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
    
    cursorNear.x = 0.F;
    cursorNear.x = 0.F;
    cursorNear.z = zNear;
    
    cursorFar.x = 0.F;
    cursorFar.x = 0.F;
    cursorFar.z = zFar;
}

void RIV3DView::Draw() {
    
//    printf("Drawing 3D view.\n");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(perspectiveAngle,(GLdouble)width/(GLdouble)height,zNear,zFar);
//    gluOrtho2D(0, width, 0, height);
//    glOrtho(0, width, 0, height, zNear, zFar);

    glViewport(startX,startY,width,height);

//    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
//    
//    glColor3f(0.F,0.F,1.F);
//    glBegin(GL_QUADS);
//    glVertex3f(cursorNear.x-.1F,cursorNear.y+.1F,cursorNear.z);
//    glVertex3f(cursorNear.x+.1F,cursorNear.y-.1F,cursorNear.z);
//    glVertex3f(cursorNear.x+.1F,cursorNear.y+.1F,cursorNear.z);
//    glVertex3f(cursorNear.x-.1F,cursorNear.y-.1F,cursorNear.z);
//    glColor3f(1,0,0);
//    glVertex3f(cursorFar.x-.1F,cursorFar.y+.1F,cursorFar.z);
//    glVertex3f(cursorFar.x+.1F,cursorFar.y-.1F,cursorFar.z);
//    glVertex3f(cursorFar.x+.1F,cursorFar.y+.1F,cursorFar.z);
//    glVertex3f(cursorFar.x-.1F,cursorFar.y-.1F,cursorFar.z);
//    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 0, -10);
    glPushMatrix();
    //Draw model!
    glRotatef(-xRotated,1.0,0.0,0.0);
    // rotation about Y axis
    glRotatef(-yRotated,0.0,1.0,0.0);
    // rotation about Z axis
    glRotatef(-zRotated,0.0,0.0,1.0);
    glTranslatef(-eye.x,-eye.y,-eye.z); //Camera transformation
    

    
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    //Draw selection box
    if(selectionBox.initialized) {
        selectionBox.Draw();
    }
    
    glColor3f(.5f,.2f,1.0f);
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glBegin(GL_TRIANGLES);
    for(size_t i = 0 ; i < modelData.size() ; i += 3) {
        glVertex3f(modelData[i],modelData[i+1],modelData[i+2]);
        
    }
    glEnd();
    

              
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glColor3f(1, 0, 0);
    GLUquadric* quadric = gluNewQuadric();

    //Draw intersections
    for(RIVTable *table : *dataset->GetTables()) {
        if(table->GetName() == "intersections") { //Only interested in the intersections table
            TableIterator *iterator = table->GetIterator();
            const RIVReference* reference = table->GetReferenceToTable("path");
            size_t row;
            while(iterator->GetNext(row)) {
                float *isectX = 0;
                float *isectY = 0;
                float *isectZ = 0;
                for(RIVRecord *record : table->GetRecords()) {
                    RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(record);
                    if(floatRecord) {
                        if(floatRecord->name == "intersection X") {
                            float x = floatRecord->Value(row);
                            isectX = &x;
                        }
                        else if(floatRecord->name == "intersection Y") {
                            float y = floatRecord->Value(row);
                            isectY = &y;
                        }
                        else if(floatRecord->name == "intersection Z") {
                            float z = floatRecord->Value(row);
                            isectZ=&z;
                        }
                    }
                    if(isectX && isectY && isectZ) {
    //                    printf("Drawn sphere\n");
                        Point3D point(*isectX,*isectY,*isectZ);
                        if((selectionBox.initialized && selectionBox.ContainsPoint(point)) || !selectionBox.initialized) {
                            glColor3f(0,0,1);
                        }
                        else {
                            glColor3f(1,0,0);
                        }
                        glPushMatrix();
                        glTranslatef(*isectX, *isectY, *isectZ);
                        gluSphere(quadric, .1F, 4, 4);
                        glPopMatrix();
                    }
                }
            }
        }
    }
    glPopMatrix();
    
    glFlush();
    
    // Pop rotation matrices
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix(); //Pop eye translation matrix
//    glPopMatrix();
}

void RIV3DView::MoveCamera(float x, float y, float z) {
    eye.x += x;
    eye.y += y;
    eye.z += z;
    
    zNear += z;
    zFar += z;
    
    printf("new eye (x,y,z) = (%f,%f,%f)\n",eye.x,eye.y,eye.z);
}

Point3D RIV3DView::ScreenToWorldCoordinates(int screenX, int screenY, float zPlane) {
    
    Point3D worldPos;
    
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

    double x,y,z;
    gluUnProject((GLdouble) screenX, (GLdouble) screenY, zPlane, mvmatrix, projmatrix, viewport, &x,&y,&z);
    
    double distanceEyeToPlane = fabs(eye.z - zNear);
    
//    printf("Distance near plane to camera = %f\n",distanceEyeToPlane);
    if(distanceEyeToPlane == 0) {
        distanceEyeToPlane = 1;
    }
//    worldPos.x = x * distanceEyeToPlane;
//    worldPos.y = y * distanceEyeToPlane
    worldPos.x = x;
    worldPos.y = y;
    worldPos.z = z;
//    printf("worldPos = [%f,%f,%f]\n",worldPos.x,worldPos.y,worldPos.z);
    
    return worldPos;
}

void RIV3DView::SetModelData(std::vector<float> _modelData) {
    if(_modelData.size() % 3 != 0) {
        throw "Malformed vertex data.";
    }
    modelData = _modelData;
}

int lastX = -1;
int lastY = -1;

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
    if(state == GLUT_DOWN) {
        cursorNear = ScreenToWorldCoordinates(x, y, 0.001);
        cursorFar = ScreenToWorldCoordinates(x, y, 0.999);
        
        float widthBox = .1F;
        float heightBox = .1F;
        Point3D startBox;
        startBox.x = cursorNear.x - .5F * widthBox;
        startBox.y = cursorNear.y - .5F * heightBox;
        startBox.z = cursorNear.z;
        
        selectionBox = Box3D(startBox.x,startBox.y,startBox.z,widthBox,heightBox,-1.F);
//        selectionBox = new Box3D(-.5F,-.5F,0,1.F,1.F,1.F);
        
//        printf ("zNearCursor : ");=
//        cursorNear.Print();
//        printf("New selection box pointStart=");
//        selectionBox.points[0].Print();
//        return true;
    }
    ToViewSpaceCoordinates(&x, &y);
	if(isDragging || containsPoint(x,y)) {
        ToViewSpaceCoordinates(&x, &y);
        if(state == GLUT_DOWN && !isDragging) {
            isDragging = true;
            lastX = x;
            lastY = y;
            return true;
        } else if(state == GLUT_UP) {
            isDragging = false;
            return true;
        }
    }
    return false;
}

bool RIV3DView::HandleMouseMotion(int x, int y) {
    if(isDragging) {
        int deltaX = x - startX;
        int deltaY = y - startY;
        
        float modifier = .5F;
        
        yRotated = -modifier * deltaX;
        xRotated = -modifier * deltaY;
        
        return true;
    }
    return false;
}
