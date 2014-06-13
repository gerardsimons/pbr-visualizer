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
#include "helper.h"

#include <GLUT/glut.h>

//TODO : Move these to the header file
const float sizeMultiplier = 5.F;

RIV3DView::RIV3DView(int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty) : RIVDataView(x,y,width,height,paddingX,paddingY,colorProperty) {
    identifier = "3DView";
};

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

void RIV3DView::ToggleDrawClusterMembers() {
    drawClusterMembers = !drawClusterMembers;
    isDirty = true;
}

void RIV3DView::Draw() {
    if(isDirty) {
        
//        copy_buffer();
//        copy_buffer_back_to_front();
//        printf("Drawing 3D view.\n");
        glEnable(GL_SCISSOR_TEST);
        glScissor(startX, startY, width, height);
        glClearColor(1.0, 1.0, 1.0, 0.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable(GL_SCISSOR_TEST);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        gluPerspective(45.F,(GLdouble)width/(GLdouble)height,zNear,zFar);
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
//        glEnable(GL_DEPTH);
        
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
//        if(selectionBox.initialized) {
//            selectionBox.Draw();
//        }
        
        glColor3f(.5f,.2f,1.0f);
        
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        
        glBegin(GL_TRIANGLES);
        for(size_t i = 0 ; i < modelData.size() ; i += 3) {
            glVertex3f(modelData[i],modelData[i+1],modelData[i+2]);
        }
        glEnd();
        
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        
        std::string colorTableReferenceName = "path";
        
        glColor3f(1, 0, 0);
        GLUquadric* quadric = gluNewQuadric();
        
        //Draw intersections
        RIVTable *table = dataset->GetTable("intersections");
        //Get the records we want;
        RIVFloatRecord* xRecord = table->GetRecord<RIVFloatRecord>("intersection X");
        RIVFloatRecord* yRecord = table->GetRecord<RIVFloatRecord>("intersection Y");
        RIVFloatRecord* zRecord = table->GetRecord<RIVFloatRecord>("intersection Z");
        TableIterator *iterator = table->GetIterator();
        
        size_t row;
        RIVCluster* cluster = NULL;
        RIVClusterSet* clusterSet = NULL; //The cluster set the cluster belongs to
        
        while(iterator->GetNext(row,cluster,clusterSet,true)) {
//            printf("row = %zu\n",row);
            float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row
            if(color != NULL) {
                float isectX = xRecord->Value(row);
                float isectY = yRecord->Value(row);
                float isectZ = zRecord->Value(row);
                Point3D point(isectX,isectY,isectZ);

                glColor4f(color[0], color[1], color[2], .5F);
                glPushMatrix();
                glTranslatef(isectX, isectY, isectZ);
                float medoidShereSize = sphereSizeDefault;
                if(cluster != NULL && clusterSet != NULL && row == cluster->GetMedoidIndex()) { //This row is a cluster medoid, draw its size according to its number of members
    //                printf("Found cluster of size %zu\n",cluster->MembersSize() + 1);
                    gluSphere(quadric, medoidShereSize * sizeMultiplier * (1 +clusterSet->RelativeSizeOf(cluster)), sizeMultiplier * 4, sizeMultiplier * 4);
    //                                std::cout << *cluster;
                }
                else if(drawClusterMembers) { //also draw the cluster members
    //                glColor4f(color[0],color[1],color[2],.5F);
                    gluSphere(quadric, sphereSizeDefault, 4, 4);
                }
                glPopMatrix();
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
//        isDirty = false;
    }
}

void RIV3DView::OnDataSetChanged() {
    printf("3D View received on filter change.");
    isDirty = true;
}

void RIV3DView::MoveCamera(float x, float y, float z) {
    eye.x += x;
    eye.y += y;
    eye.z += z;
    
    zNear += z;
    zFar += z;
    
//    printf("new eye (x,y,z) = (%f,%f,%f)\n",eye.x,eye.y,eye.z);
    isDirty = true;
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

void RIV3DView::SetModelData(const std::vector<float>& _modelData) {
    if(_modelData.size() == 0 ||_modelData.size() % 3 != 0) {
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
        isDirty = true;
    }
    ToViewSpaceCoordinates(&x, &y);
	if(isDragging || containsPoint(x,y)) {
        ToViewSpaceCoordinates(&x, &y);
        if(state == GLUT_DOWN && !isDragging) {
            isDragging = true;
            lastX = x;
            lastY = y;
            isDirty = true;
            return true;
        } else if(state == GLUT_UP) {
            isDragging = false;
            isDirty = true;
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
        isDirty = true;
        return true;
    }
    return false;
}
