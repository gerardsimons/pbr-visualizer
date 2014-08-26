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

//Init instance to draw
RIV3DView* RIV3DView::instance = NULL;

//TODO : Move these to the header file
const float sizeMultiplier = 5.F;

RIV3DView::RIV3DView() {
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    sizeProperty = new RIVFixedSizeProperty(0.1F);
    const float black[] = {0,0,0};
    colorProperty = new RIVFixedColorProperty(black);
    instance = this;
    identifier = "3DView";
}

RIV3DView::RIV3DView(int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(x,y,width,height,paddingX,paddingY,colorProperty,sizeProperty) {
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    instance = this;
    identifier = "3DView";
    
};

RIV3DView::RIV3DView(RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(colorProperty,sizeProperty) {
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    instance = this;
    identifier = "3DView";
};

void RIV3DView::Reshape(int newWidth, int newHeight) {
    width = newWidth;
    this->height = newHeight;
    
//    eye.x = 0.F;
//    eye.y = 0.F;
//    eye.z = 4.F;
    
//    Translate -278.000000 -273.000000 500.000000
    eye.x = 0;
    eye.y = 0;
    eye.z = 0;
    
//    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
    
//    cursorNear.x = 0.F;
//    cursorNear.x = 0.F;
//    cursorNear.z = zNear;
//    
//    cursorFar.x = 0.F;
//    cursorFar.x = 0.F;
//    cursorFar.z = zFar;
    
    tbInitTransform();
    tbHelp();
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double)width/height, -10000, 10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

//void RIV3DView::Reshape(float startX, float startY, float width, float height, float paddingX, float paddingY) {
//    
//    this->startX = startX;
//    this->startY = startY;
//    this->width = width;
//    this->height = height;
//    
//    eye.x = 0.F;
//    eye.y = 0.F;
//    eye.z = 4.F;
//    
//    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
//    
//    cursorNear.x = 0.F;
//    cursorNear.x = 0.F;
//    cursorNear.z = zNear;
//    
//    cursorFar.x = 0.F;
//    cursorFar.x = 0.F;
//    cursorFar.z = zFar;
//    
////    glMatrixMode(GL_MODELVIEW);
////    glLoadIdentity();
//    printf("**************** RESHAPE 3D VIEW ****************\n");
//    printVar("startX",startX);
//    printVar("startY",startY);
//    printVar("width",width);
//    printVar("height",height);
//    printf("*************************************************\n");
//
//    glutSetWindow(windowHandle);
//    glViewport(startX, startY, width, height);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(40, (double)width/height, 1, 10);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    
//
//}

void RIV3DView::ToggleDrawClusterMembers() {
    drawClusterMembers = !drawClusterMembers;
    isDirty = true;
}

void RIV3DView::Draw() {
//    printf("3DView Draw!\n");
    glEnable(GL_DEPTH_TEST);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-eye.x,-eye.y,-eye.z);
//    glScalef(modelData.GetScale(), modelData.GetScale(), modelData.GetScale());
    
//    printf("eye (x,y,z) * modelScale = (%f,%f,%f)\n",-eye.x * modelData.GetScale(),-eye.y * modelData.GetScale(),-eye.z * modelData.GetScale());
    
    tbVisuTransform();
    
    drawCoordSystem();
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    glColor3f(.5f,.2f,1.0f); //Purple
    /** Draw the model **/
    glBegin(GL_QUADS);
    const std::vector<float>& vertices = modelData.GetVertices();
    for(size_t i = 0 ; i < vertices.size() ; i += 3) {
        glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
    }
    glEnd();
    
    /* Draw the intersection positions */
//    GLUquadric* quadric = gluNewQuadric();
    
    //Draw
    glColor3f(1, 1, 1);
    Point3D modelCenter = modelData.GetCenter();

//    glPushMatrix();
//    glLoadIdentity();
    glScalef(modelData.GetScale(), modelData.GetScale(), modelData.GetScale());
    glTranslatef(-modelCenter.x, -modelCenter.y, -modelCenter.z);

//    glTranslatef(-modelCenter.x, -modelCenter.y, -modelCenter.z);
//    gluSphere(quadric, .1F, 2, 2);
    
//    std::string drawTask = "3D View Drawing";
//    reporter::startTask(drawTask);
    printf("\n");
    for(size_t i = 0 ; i < pointsX.size() ; ++i) {
        glPushMatrix();
        glTranslatef(pointsX[i], pointsY[i], pointsZ[i]);
//        printf("Draw point (%f,%f,%f)\n",pointsX[i],pointsY[i],pointsZ[i]);
        glScalef(1/modelData.GetScale(), 1/modelData.GetScale(), 1/modelData.GetScale());
        if(drawClusterMembers) { //also draw the cluster members
            //                glColor4f(color[0],color[1],color[2],.5F);

            //                    printf("\Member sphere size = %f\n",size);
//            gluSphere(quadric, 5, 2, 2);
            glColor3f(pointsR[i], pointsG[i], pointsB[i]);

            glPointSize(pointsSize[i]);
            glBegin(GL_POINTS);
            glVertex3f(0, 0, 0);
            glEnd();
        }
        glPopMatrix();
        
    }
    
//    reporter::stop(drawTask);
    glPopMatrix();
    
    //Draw some lines
    drawPaths(1);
    
    glFlush();
    
    glutSwapBuffers();
}

void RIV3DView::createPoints() {
    
    //Clear buffer data
    pointsX.clear();
    pointsY.clear();
    pointsZ.clear();
    pointsR.clear();
    pointsG.clear();
    pointsB.clear();
    pointsSize.clear();
    
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
    
    while(iterator->GetNext(row,cluster,clusterSet,true)) {
        
        float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row
        
        if(color != NULL) {
            pointsX.push_back(xRecord->Value(row));
            pointsY.push_back(yRecord->Value(row));
            pointsZ.push_back(zRecord->Value(row));
            
            pointsSize.push_back(sizeProperty->ComputeSize(table, row));
            pointsR.push_back(color[0]);
            pointsG.push_back(color[1]);
            pointsB.push_back(color[2]);
        }
    }
    
    printf("%zu points created.\n",pointsX.size());
}

void RIV3DView::drawPaths(int startBounce) {
    RIVTable *table = dataset->GetTable("intersections");
    //Get the records we want;
    RIVFloatRecord* xRecord = table->GetRecord<RIVFloatRecord>("intersection X");
    RIVFloatRecord* yRecord = table->GetRecord<RIVFloatRecord>("intersection Y");
    RIVFloatRecord* zRecord = table->GetRecord<RIVFloatRecord>("intersection Z");
    RIVUnsignedShortRecord *bounceRecord = table->GetRecord<RIVUnsignedShortRecord>("bounce#");
    //Get the iterator, this iterator is aware of what rows are filtered and not
    TableIterator *iterator = table->GetIterator();
    
    size_t row = 0;
    size_t lastRow = 0;
    ushort lastBounceNr = 0;
    float lastColor[3] = {0};
    
    bool pathStartFound = false;
    size_t linesDrawn = 0;
    
    glBegin(GL_LINES);
    while(iterator->GetNext(row)) {
        float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row
        if(color != NULL) {
            ushort bounceNr = bounceRecord->Value(row);
            if(!pathStartFound && bounceNr == startBounce) {
//                printf("Row %zu is valid start point for path\n",row);
                pathStartFound = true;
            }
            else if(pathStartFound && row == lastRow + 1 && bounceNr == lastBounceNr + 1) { //Valid path found, draw
//                printf("Row %zu is valid end point for path\n",row);
//                printf("Last color = ");
//                printArray(lastColor, 3);
                glColor3fv(lastColor);
                glVertex3f(xRecord->Value(lastRow), yRecord->Value(lastRow), zRecord->Value(lastRow));
//                printf("current color = ");
//                printArray(color, 3);
                glColor3fv(color);
                glVertex3f(xRecord->Value(row), yRecord->Value(row), zRecord->Value(row));
                
//                printf("Drawn path line #%zu\n",linesDrawn);
                
                linesDrawn++;
                pathStartFound = false;
            }
//            else if(bounceNr == startBounce) {
////                printf("Last row was not valid after all, it had no successor.\n");
//                
//            }
            lastRow = row;
            lastBounceNr = bounceNr;
            lastColor[0] = color[0];
            lastColor[1] = color[1];
            lastColor[2] = color[2];
        }
    }
    glEnd();
}

void RIV3DView::DrawInstance() {
    if(instance != NULL) {
        instance->Draw();
    }
    else {
        printf("No instance to draw.\n");
    }
}

void RIV3DView::Mouse(int button, int state, int x, int y) {
    if(instance != NULL) {
        instance->HandleMouse(button,state,x,y);
    }
}

void RIV3DView::Motion(int x, int y) {
    if(instance != NULL) {
        instance->HandleMouseMotion(x, y);
    }
}

void RIV3DView::ReshapeInstance(int width, int height) {
    if(instance != NULL) {
        instance->Reshape(width,height);
    }
    else printf("No instance to reshape");
}

void RIV3DView::OnDataSetChanged() {
//    printf("3D View received on filter change.");
    createPoints();
    glutPostRedisplay();
    isDirty = true;
}

void RIV3DView::MoveCamera(float x, float y, float z) {
    eye.x += x;
    eye.y += y;
    eye.z += z;
    
//    zNear += z;
//    zFar += z;
    
    printf("new eye (x,y,z) = (%f,%f,%f)\n",eye.x,eye.y,eye.z);
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
    
//    double distanceEyeToPlane = fabs(eye.z - zNear);
    
    //    printf("Distance near plane to camera = %f\n",distanceEyeToPlane);
//    if(distanceEyeToPlane == 0) {
//        distanceEyeToPlane = 1;
//    }
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
//    printf("RIV3DView HandleMouse\n");
    y = height - y;
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

bool RIV3DView::HandleMouseMotion(int x, int y) {
//    printf("RIV3DView HandleMouseMotion\n");
//    ToViewSpaceCoordinates(&x, &y);
    y = height - y;
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