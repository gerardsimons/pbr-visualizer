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
#include "reporter.h"
#include "trackball.h"

#include "graphics_helper.h"

#include <GLUT/glut.h>

//TODO : Move these to the header file
const float sizeMultiplier = 5.F;

RIV3DView::RIV3DView(int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(x,y,width,height,paddingX,paddingY,colorProperty,sizeProperty) {
    identifier = "3DView";
};

RIV3DView::RIV3DView(RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(colorProperty,sizeProperty) {
    identifier = "3DView";
};

void RIV3DView::ComputeLayout(float startX, float startY, float width, float height, float paddingX, float paddingY) {
    
    this->startX = startX;
    this->startY = startY;
    this->width = width;
    this->height = height;
    
    eye.x = 0.F;
    eye.y = 0.F;
    eye.z = 4.F;
    
    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
    
    cursorNear.x = 0.F;
    cursorNear.x = 0.F;
    cursorNear.z = zNear;
    
    cursorFar.x = 0.F;
    cursorFar.x = 0.F;
    cursorFar.z = zFar;
    
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

    
    tbInitTransform();
    tbHelp();
}

void RIV3DView::ToggleDrawClusterMembers() {
    drawClusterMembers = !drawClusterMembers;
    isDirty = true;
}

void RIV3DView::Draw() {
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(startX, startY, width, height);

    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
    
    glViewport(startX, startY, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho (-1.1, 1.1, -1.1,1.1, -1000.0, 1000.0);
    gluPerspective (50, (float)width/height, 1, 10);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-eye.x,-eye.y,-eye.z);
    
    tbVisuTransform();
    
//    drawArm();
    drawCoordSystem();
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    glColor3f(.5f,.2f,1.0f); //Purple
//    glMatrixMode(GL_MODELVIEW);
    
    /** Draw the model **/
    glBegin(GL_TRIANGLES);
    const std::vector<float>& vertices = modelData.GetVertices();
    for(size_t i = 0 ; i < vertices.size() ; i += 3) {
        glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
    }
    glEnd();
    
    /* Draw the intersection positions */
    GLUquadric* quadric = gluNewQuadric();
    
    //Draw intersections
    RIVTable *table = dataset->GetTable("intersections");
    //Get the records we want;
    RIVFloatRecord* xRecord = table->GetRecord<RIVFloatRecord>("intersection X");
    RIVFloatRecord* yRecord = table->GetRecord<RIVFloatRecord>("intersection Y");
    RIVFloatRecord* zRecord = table->GetRecord<RIVFloatRecord>("intersection Z");
    //Get the iterator, this iterator is aware of what rows are filtered and not
    TableIterator *iterator = table->GetIterator();
    
    size_t row = 0;
    RIVCluster* cluster = NULL;
    RIVClusterSet* clusterSet = NULL; //The cluster set the cluster belongs to
    
    //Draw
    glColor3f(1, 1, 1);
    Point3D modelCenter = modelData.GetCenter();

//    glPushMatrix();
//    glLoadIdentity();
//    glTranslatef(-modelCenter.x, -modelCenter.y, -modelCenter.z);
    glScalef(modelData.GetScale(), modelData.GetScale(), modelData.GetScale());
    glTranslatef(-modelCenter.x, -modelCenter.y, -modelCenter.z);
    gluSphere(quadric, .1F, 8, 8);
    
    while(iterator->GetNext(row,cluster,clusterSet,true)) {
//        printf("row = %zu\n",row);
        float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row

        if(color != NULL) {
            glColor3f(color[0], color[1], color[2]);
            
            float isectX = xRecord->Value(row);
            float isectY = -yRecord->Value(row); //HACK, Cornell has flipped Y coordinates
            float isectZ = zRecord->Value(row);
            Point3D point(isectX,isectY,isectZ);
            
            float size = sizeProperty->ComputeSize(table, row);
            
            glPushMatrix();
            glTranslatef(isectX, isectY, isectZ);
            glScalef(1/modelData.GetScale(), 1/modelData.GetScale(), 1/modelData.GetScale());
            if(cluster != NULL && clusterSet != NULL && row == cluster->GetMedoidIndex()) { //This row is a cluster medoid, draw its size according to its number of members
                gluSphere(quadric, size, 8, 8);
                //                                std::cout << *cluster;
            }
            else if(drawClusterMembers) { //also draw the cluster members
                //                glColor4f(color[0],color[1],color[2],.5F);
                float size = sizeProperty->ComputeSize(table, row);
                //                    printf("\Member sphere size = %f\n",size);
                gluSphere(quadric, size, 4, 4);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
    
    glFlush();
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

Point3D RIV3DView::screenToWorldCoordinates(int screenX, int screenY, float zPlane) {
    
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

void RIV3DView::SetModelData(const MeshModel& model) {
    modelData = model;
}

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
    ToViewSpaceCoordinates(&x, &y);
    printf("isDragging = %d\n",isDragging);
	if(isDragging || containsPoint(x,y)) {
        if(state == GLUT_DOWN) {
            isDragging = true;
            tbMouseFunc(button, state, x, y);
            return true;
        }
    }
    isDragging = false;
    return false;
}

//void RIV3DView::Reshape(float w, float h) {
//;
//}

bool RIV3DView::HandleMouseMotion(int x, int y) {
    ToViewSpaceCoordinates(&x, &y);
    if(isDragging) {
        tbMotionFunc(x, y);
        return true;
    }
    return false;
}

/*
int lastX = -1;
int lastY = -1;

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
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
*/