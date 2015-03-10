    //
//  3DView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 19/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "3DView.h"
#include "../helper.h"
#include "../Geometry/Geometry.h"
#include "../reporter.h"
#include "../trackball.h"
#include "../Graphics/ColorPalette.h"
#include "../Graphics/graphics_helper.h"
#include "../Configuration.h"
#include "devices/device_singleray/embree_renderer.h"
#include "devices/device_singleray/shapes/shape.h"
//#include "devices/device_singleray/api/instance.h"

#if __APPLE__
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

//Init instance to draw
RIV3DView* RIV3DView::instance = NULL;
int RIV3DView::windowHandle = -1;

RIV3DView::RIV3DView(RIVDataSet<float,ushort>** dataset,EMBREERenderer* renderer,const TriangleMeshGroup& sceneDataOne, Octree* energyDistribution, RIVColorProperty* pathColor, RIVColorProperty* rayColor) : RIVDataView(dataset),
    rendererOne(renderer),
    energyDistributionOne(energyDistribution),
    pathColorOne(pathColor),
    rayColorOne(rayColor) {
    
    if(instance != NULL) {
        throw std::runtime_error("Only 1 instance of RIV3DView allowed.");
    }
    instance = this;
    identifier = "3DView";
    
    this->meshesOne = sceneDataOne;
    
//    scale = 5 * meshesOne.GetScale();
    modelScale = meshesOne.GetScale();
    modelCenter = meshesOne.GetCenter();
    drawDataSetTwo = false;
    
    drawHeatmapDepth = energyDistributionOne->Depth();
    
    cameraPositionOne = renderer->GetCameraPosition();
//    eye = cameraPositionOne;
    
    ResetGraphics();
};

RIV3DView::RIV3DView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo,EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo, const TriangleMeshGroup& sceneDataOne, const TriangleMeshGroup& sceneDataTwo, Octree* energyDistributionOne, Octree* energyDistributionTwo, RIVColorProperty* pathColorOne, RIVColorProperty* rayColorOne, RIVColorProperty* pathColorTwo, RIVColorProperty* rayColorTwo) :
    RIVDataView(datasetOne,datasetTwo),
    rendererOne(rendererOne),
    rendererTwo(rendererTwo),
    energyDistributionOne(energyDistributionOne),
    energyDistributionTwo(energyDistributionTwo),
    pathColorOne(pathColorOne),
    rayColorOne(rayColorOne),
    pathColorTwo(pathColorTwo),
    rayColorTwo(rayColorTwo){
    
    if(instance != NULL) {
        throw std::runtime_error("Only 1 instance of RIV3DView allowed.");
    }
    instance = this;
    identifier = "3DView";
        
    drawHeatmapDepth = std::min(energyDistributionOne->Depth(),energyDistributionTwo->Depth());

    this->meshesOne = sceneDataOne;
    this->meshesTwo = sceneDataTwo;
    
    Vec3f modelCenterOne = meshesOne.GetCenter();
    
    modelScale = meshesOne.GetScale();
    modelCenter = meshesOne.GetCenter();
    
    if(datasetTwo && drawDataSetTwo) {
        float scaleTwo = meshesTwo.GetScale();
        if(scaleTwo > modelScale) {
            modelScale = scaleTwo;
        }
        Vec3fa modelCenterTwo = meshesTwo.GetCenter();
        float lengthOne = length(modelCenterOne);
        float lengthTwo = length(modelCenterTwo);
        if( lengthTwo > lengthOne ) {
            modelCenter = modelCenterTwo;
        }
    }
    cameraPositionOne = rendererOne->GetCameraPosition();
    cameraPositionTwo = rendererTwo->GetCameraPosition();
//    eye = cameraPositionOne;
    ResetGraphics();
};

void RIV3DView::CyclePathSegment(bool direction) {
    float delta = 1.F / maxBounce;
    direction ? MovePathSegment(delta) : MovePathSegment(-delta);
}

void RIV3DView::Reshape(int newWidth, int newHeight) {
//    width = newWidth;
//    height = newHeight;
//    
////    eye = cameraPositionOne;
//    
//    eye[0] = 0;
//    eye[1] = 12;
//    eye[2] = 17;
//    
//    tbInitTransform();
////    tbHelp();
//    
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glViewport(0, 0, width, height);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(55, (double)width/height, .1, 100);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
    
    width = newWidth;
    this->height = newHeight;
    
    eye.x = 0;
    eye.y = 0;
    eye.z = 1;
    
    //    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
    
    tbInitTransform();
    tbHelp();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double)width/height, 0.1, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void RIV3DView::ToggleDrawIntersectionPoints() {
    drawIntersectionPoints = !drawIntersectionPoints;
    if(drawIntersectionPoints && !pathsCreated) {
        createPaths();
    }
    else if(!drawIntersectionPoints && !drawLightPaths) {
        pathsCreated = false;
        pathsOne.clear();
        pathsTwo.clear();
    }
    printf("drawIntersectionPoints is now ");
    if(drawIntersectionPoints) printf("ON\n");
    else printf("OFF\n");
    
    Invalidate();
    glutPostRedisplay();
}
void RIV3DView::ToggleDrawHeatmap() {
    drawHeatmapTree = !drawHeatmapTree;
}

void RIV3DView::ToggleDrawDataSetOne() {
    drawDataSetOne = !drawDataSetOne;
}
void RIV3DView::ToggleDrawDataSetTwo() {
    if(datasetTwo) {
        drawDataSetTwo = !drawDataSetTwo;
    }
    else printf("No second dataset set.");
}

void RIV3DView::drawEnergyHelper(OctreeNode* node, float max,riv::ColorMap& heatmap, ushort maxDepth) {
    bool nodeMaxReached = (node->IsLeafNode() || node->GetDepth() >= maxDepth);
    float min = 0;
    float alpha = 0.8;
    if(nodeMaxReached) {
        float ratio = node->AggregateValue() / max;
        if(ratio > min) {
//            printf("Ratio = %f\n",ratio);
            riv::Color c = heatmap.ComputeColor(ratio);
//            glColor4f(c.R,c.G,c.B,alpha);
            glColor4f(c.R,c.G,c.B,alpha);
            glPushMatrix();
            glTranslatef(node->cx, node->cy, node->cz);
            glutSolidCube(node->GetSize());
            glPopMatrix();
        }
        else if(ratio > 1) {
            
        }
    }
    else {
        for(int i = 0 ; i < 8 ; ++i) {
            drawEnergyHelper(node->GetChild(i),max,heatmap,maxDepth);
        }
    }
}
void RIV3DView::drawEnergyDifferenceHelper(OctreeNode* nodeOne, OctreeNode* nodeTwo, float max) {
    
    bool maxDepthReachedOne = nodeOne->GetDepth() >= drawHeatmapDepth || nodeOne->IsLeafNode();
    bool maxDepthReachedTwo = nodeTwo->GetDepth() >= drawHeatmapDepth || nodeTwo->IsLeafNode();
    
    if(maxDepthReachedOne || maxDepthReachedTwo) {
        
        if(maxDepthReachedOne && !maxDepthReachedTwo) {
            for(int i = 0 ; i < 8 ; ++i) {
                drawEnergyDifferenceHelper(nodeOne,nodeTwo->GetChild(i),max);
            }
        }
        else if(maxDepthReachedTwo && !maxDepthReachedOne) {
            for(int i = 0 ; i < 8 ; ++i) {
                drawEnergyDifferenceHelper(nodeOne->GetChild(i),nodeTwo,max);
            }
        }
        else if(maxDepthReachedOne && maxDepthReachedTwo) { //
            

            
            OctreeNode* smallestNode = nodeTwo;
            if(nodeOne->GetDepth() > nodeTwo->GetDepth()) {
                smallestNode = nodeOne;
            }
            
            float multiplier = smallestNode->GetDepth() / nodeOne->GetDepth();
            float multiplierTwo = smallestNode->GetDepth() / nodeTwo->GetDepth();
            
//            float valueOne = nodeOne->AggregateValue() / max;
//            float valueTwo = nodeTwo->AggregateValue() / max;
            
            float valueOne = nodeOne->AggregateValue() * multiplier / max;
            float valueTwo = nodeTwo->AggregateValue() * multiplierTwo / max;
            
            float red,blue,saturation;
            
            float min = 0;
            
            if(valueOne > min || valueTwo > min) {
                if(valueTwo > valueOne) {
                    blue = ((valueTwo - valueOne) / valueTwo + 1) / 2.F;
                    red = 1-blue;
                    saturation = valueTwo / max;
                }
                else {
                    red = ((valueOne - valueTwo) / valueOne + 1) / 2.F;
                    blue = 1 - red;
                    saturation = valueOne / max;
                }
                
//                glColor4f(red,0,blue,std::pow(saturation,.33));
                glColor4f(red,0,blue,1);
                glPushMatrix();
                glTranslatef(smallestNode->cx, smallestNode->cy, smallestNode->cz);
                glutSolidCube(smallestNode->GetSize());
                glPopMatrix();
            }
        }
    }
    else {
        for(int i = 0 ; i < 8 ; ++i) {
            drawEnergyDifferenceHelper(nodeOne->GetChild(i),nodeTwo->GetChild(i), max);
        }
    }
}

//Draw energy difference by comparing two octrees
void RIV3DView::drawEnergyDifference(Octree *energyDistributionOne, Octree *energyDistributionTwo,ushort maxDepth) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float maxOne = energyDistributionOne->MaxValue(maxDepth);
    float maxTwo = energyDistributionTwo->MaxValue(maxDepth);
    float max = std::max(maxOne,maxTwo);
    if(energyDistributionOne && energyDistributionTwo) {
        drawEnergyDifferenceHelper(energyDistributionOne->GetRoot(), energyDistributionTwo->GetRoot(), max);
    }
    else throw std::runtime_error("One of the energy distributions is not set.");
}

void RIV3DView::drawEnergyDistribution(Octree* energyDistribution, ushort maxDepth) {
    drawEnergyDistribution(energyDistribution,maxDepth,energyDistribution->MaxValue());
}
void RIV3DView::drawEnergyDistribution(Octree* energyDistribution, ushort maxDepth, float maxEnergy) {
//    riv::ColorMap colors = colors::brownColorMap();
    riv::ColorMap colors = colors::jetColorMap();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
//    float maxValue = energyDistribution->MaxValue(maxDepth);
    drawEnergyHelper(energyDistribution->GetRoot(),maxEnergy,colors,maxDepth);
}
void RIV3DView::Draw() {

    
    //	reporter::startTask("3D Draw");
    
    glEnable(GL_DEPTH_TEST);
//    glClearColor(1.0, 1.0, 1.0, 0.0); //White
    glClearColor(0.0, 0.0, 0.0, 0.0); //black
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Somehow it is mirrored so lets mirror it again to match the image
    
    //    printf("eye (x,y,z) * modelScale = (%f,%f,%f)\n",-eye.x * modelData.GetScale(),-eye.y * modelData.GetScale(),-eye.z * modelData.GetScale());
    glScalef(-1,1,1);
    glTranslatef(-eye.x,-eye.y,-eye.z);
    tbVisuTransform();
    
    drawCoordSystem();
    
    glPushMatrix();
    
    glScalef(modelScale,modelScale,modelScale);
    
    glTranslatef(-modelCenter[0], -modelCenter[1], -modelCenter[2]);

    if(showMeshes) {
        if(drawDataSetOne) {
            //		float purpleColor[3] =  {.5f,.2f,1.0f};
            float redColor[] = {1,0,0};
            drawMeshModel(&meshesOne,redColor,&selectedObjectIdOne);
        }
        if(drawDataSetTwo) {
            float blueColor[] = {0,0,1};
            drawMeshModel(&meshesTwo,blueColor,&selectedObjectIdTwo);
        }
    }
    if(drawIntersectionPoints)
        drawPoints();
    
    if(drawDataSetOne && !drawDataSetTwo && drawHeatmapTree) {
        if(datasetTwo) {
            float maxOne = energyDistributionOne->MaxValue(drawHeatmapDepth);
            float maxTwo = energyDistributionTwo->MaxValue(drawHeatmapDepth);
            printf("maxOne = %f\n",maxOne);
            printf("maxTwo = %f\n",maxTwo);
            float max = std::max(maxOne,maxTwo);
            drawEnergyDistribution(energyDistributionOne,drawHeatmapDepth,max);
        }
        else drawEnergyDistribution(energyDistributionOne,drawHeatmapDepth);
    }
    if(drawDataSetTwo && !drawDataSetOne && drawHeatmapTree) {
        float max = std::max(energyDistributionOne->MaxValue(drawHeatmapDepth),energyDistributionTwo->MaxValue(drawHeatmapDepth));
        drawEnergyDistribution(energyDistributionTwo,drawHeatmapDepth,max);
    }
    else if(drawHeatmapTree && (drawDataSetTwo && drawDataSetOne)) {
        drawEnergyDifference(energyDistributionOne, energyDistributionTwo,drawHeatmapDepth);
    }
    
//    drawLightCones(lightConesOne);
    
    //Draw selection ray
    glColor3f(1,1,1);
    glBegin(GL_LINES);
//    	glVertex3f(selectNear.x, selectNear.y, selectNear.z);
//    	glVertex3f(selectFar.x, selectFar.y, selectFar.z);
    glVertex3f(pickRay.org[0], pickRay.org[1], pickRay.org[2]);
    Vec3fa dest = pickRay.org + 1.F * pickRay.dir;
    glColor3f(1, 0, 0);
    glVertex3f(dest[0],dest[1],dest[2]);
    glEnd();
    
    //Draw Phit
    if(meshSelected) {
        glColor3f(1, 1, 0);
        glPushMatrix();
        glTranslatef(Phit[0], Phit[1], Phit[2]);
        //	std::cout << "Phit = " << Phit << std::endl;
        glScalef(.01*modelScale, .01*modelScale, .01*modelScale);
        gluSphere(quadric, 2, 9, 9);
        glPopMatrix();
    }
    
    //Draw
    float cameraScale = modelScale * .3;
    glColor3f(1, .2, .2);
    //Draw camera position
    glPushMatrix();
    glTranslatef(cameraPositionOne[0],cameraPositionOne[1],cameraPositionOne[2]);
    glScalef(cameraScale,cameraScale,cameraScale);
    //    glScalef(0.01, 0.01, 0.01);x§
    gluSphere(quadric, 10, 10, 10);
    glPopMatrix();
    
    if(datasetTwo) {
        glColor3f(.2, .2, 1);
        //Draw camera position
        glPushMatrix();
        glTranslatef(cameraPositionTwo[0],cameraPositionTwo[1],cameraPositionTwo[2]);
        glScalef(cameraScale,cameraScale,cameraScale);
        //    glScalef(0.01, 0.01, 0.01);x§
        gluSphere(quadric, 10, 10, 10);
        glPopMatrix();
    }
    
//    if(drawHeatmapTree && heatmap != NULL)
//        drawHeatmap();
    
    //Draw some lines
    if(drawLightPaths)
        drawPaths(segmentStart,segmentStop);
    
    glPopMatrix();
    
    //Flush the buffer
    glFlush();
    
    //Swap back and front buffer
    glutSwapBuffers();
    
    glDisable(GL_BLEND);
    
    //	reporter::stop("3D Draw");
}

void RIV3DView::drawMeshModel(TriangleMeshGroup* meshGroup, float* color, ushort* selectedObjectId) {
    
    //	reporter::startTask("Draw mesh model");
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //    glColor3f(.5f,.2f,1.0f); //Purple
    glColor3f(color[0],color[1],color[2]);
    /** Draw the model **/
    glBegin(GL_TRIANGLES);
    
    size_t meshindex = 0;
    
    for(TriangleMeshFull* mesh : meshGroup->GetTriangleMeshes()) {
        vector_t<TriangleMeshFull::Triangle> triangles = mesh->triangles;
        vector_t<Vec3fa>& position = mesh->position;
        if(selectedObjectId && meshindex == *selectedObjectId) {
            glColor3f(0.8, 0.8, 0.6);
        }
        else {
            glColor3f(color[0], color[1], color[2]);
        }
        for(size_t i = 0 ; i < triangles.size() ; ++i) {
            Vec3fa& v0 = position[triangles[i].v0];
            Vec3fa& v1 = position[triangles[i].v1];
            Vec3fa& v2 = position[triangles[i].v2];
            glVertex3f(v0[0],v0[1],v0[2]);
            glVertex3f(v1[0],v1[1],v1[2]);
            glVertex3f(v2[0],v2[1],v2[2]);
        }
        meshindex++;
    }
    //	reporter::stop("Draw mesh model");
    glEnd();
}

void RIV3DView::drawPoints() {
    if(drawIntersectionPoints) {
        if(drawDataSetOne)
            drawPoints(*datasetOne,pathsOne);
        if(datasetTwo && drawDataSetTwo) {
            drawPoints(*datasetTwo, pathsTwo);
        }
    }
}
void RIV3DView::ToggleHideMesh() {
    showMeshes = !showMeshes;
    redisplayWindow();
}
void RIV3DView::drawPoints(RIVDataSet<float,ushort>* dataset, const std::vector<Path>& paths) {
    //	reporter::startTask("Draw points.");
    //	printf("Drawing intersections points.\n");
    
    RIVTable<float,ushort>* isectTable = dataset->GetTable("intersections");
    
    RIVFloatRecord* xRecord = isectTable->GetRecord<float>("x");
    RIVFloatRecord* yRecord = isectTable->GetRecord<float>("y");
    RIVFloatRecord* zRecord = isectTable->GetRecord<float>("z");
    
    size_t row = 0;
    TableIterator* it = isectTable->GetIterator();
    
    glPointSize(2);
    
    glBegin(GL_POINTS);
    for(const Path& path : paths) {
        for(size_t i = 0 ; i < path.Size() ; ++i) {
            
            it->GetNext(row);
            
            PathPoint* point = path.GetPoint(i);
            riv::Color pointColor = point->color;
            
            float x = xRecord->Value(point->rowIndex);
            float y = yRecord->Value(point->rowIndex);
            float z = zRecord->Value(point->rowIndex);
            
            glColor3f(pointColor.R,pointColor.G,pointColor.B);
            glVertex3f(x,y,z);
        }
    }
    glEnd();
    
    //	reporter::stop("Draw points.");
}
void RIV3DView::ResetGraphics() {
    if(drawLightPaths || drawIntersectionPoints) {
        createPaths();
    }
}
//(Re)create the paths objects for the datasets being used
void RIV3DView::createPaths() {
    pathsOne = createPaths(*datasetOne,pathColorOne,rayColorOne,lightConesOne);
    if(datasetTwo) {
        pathsTwo = createPaths(*datasetTwo,pathColorTwo,rayColorTwo,lightConesTwo);
    }
    pathsCreated = true;
}
//Create buffered data for points, not working anymore, colors seem to be red all the time.
std::vector<Path> RIV3DView::createPaths(RIVDataSet<float,ushort>* dataset, RIVColorProperty* pathColor, RIVColorProperty* rayColor, std::map<size_t,LightCone*>& lightCones) {
    
    reporter::startTask("Creating paths");
    
    RIVTable<float,ushort>* isectTable = dataset->GetTable(INTERSECTIONS_TABLE);
    
    RIVTable<float,ushort>* pathsTable = dataset->GetTable(PATHS_TABLE);
    RIVShortRecord* bounceRecord = isectTable->GetRecord<ushort>(BOUNCE_NR);
    RIVShortRecord* primitiveRecord = isectTable->GetRecord<ushort>(PRIMITIVE_ID);
    
    RIVFloatRecord* xRecord = isectTable->GetRecord<float>(POS_X);
    RIVFloatRecord* yRecord = isectTable->GetRecord<float>(POS_Y);
    RIVFloatRecord* zRecord = isectTable->GetRecord<float>(POS_Z);
    
    std::vector<Path> paths;
    
    //Get the records we want;
    //Get the iterator, this iterator is aware of what rows are filtered and not
    TableIterator* intersectionsIterator = isectTable->GetIterator();
    
    size_t row = 0;
    size_t* pathID = NULL;
    size_t oldPathID = 0;
    ushort bounceNr;
    
    std::vector<PathPoint> points;
    lightCones.clear();
    LightCone* previousCone = NULL;
    
//    dataset->Print(10);
    std::map<RIVTableInterface*,std::pair<size_t*,ushort>> referenceRowsMap;
    
    while(intersectionsIterator->GetNext(row,referenceRowsMap)) {
        
        std::pair<size_t*,ushort> refRows;
        
        for(auto it : referenceRowsMap) {
            if(it.first->name == PATHS_TABLE) {
                refRows = it.second;
            }
        }
        if(refRows.first) {
            pathID = &refRows.first[0];
        }

        
        ushort primitiveId = primitiveRecord->Value(row);
        LightCone*& existing = lightCones[primitiveId];
        float x = xRecord->Value(row);
        float y = yRecord->Value(row);
        float z = zRecord->Value(row);
        if(!existing) {
           existing = new LightCone();
        }
        
        size_t N = existing->originN;
//        float rcp = 1.F / existing->originN;
//        std::cout << existing->origin << " + " << x << "," << y << "," << z << " = ";
        existing->origin[0] = (existing->origin[0] * N + x) / (N+1);
        existing->origin[1] = (existing->origin[1] * N + y) / (N+1);
        existing->origin[2] = (existing->origin[2] * N + z) / (N+1);
        existing->originN++;
//        std::cout  << existing->origin << std::endl;
        
        //New path, clear previous stuff
        if(pathID && *pathID != oldPathID) {
            riv::Color pColor;
            pathColor->ComputeColor(pathsTable, *pathID, pColor);
            if(points.size() > 0) {
                paths.push_back(Path(points,pColor));
                points.clear();
            }
            previousCone = NULL;
            
            oldPathID = *pathID;
        }
        //If still the same path, the previous cone should point more towards this point
        if(previousCone) {
//            float rcp = 1.F / existing->targetN;
            size_t N = previousCone->targetN;
            previousCone->target[0] = (previousCone->target[0] * N + x) / (N+1);
            previousCone->target[1] = (previousCone->target[1] * N + y) / (N+1);
            previousCone->target[2] = (previousCone->target[2] * N + z) / (N+1);
            ++previousCone->targetN;
        }
        bounceNr = bounceRecord->Value(row);
        riv::Color pointColor;
        rayColor->ComputeColor(isectTable, row, pointColor); //Check if any color can be computed for the given row
        PathPoint p;
        p.rowIndex = row;
        p.bounceNr = bounceNr;
        p.color = pointColor;
        points.push_back(p);
        oldPathID = *pathID;
        previousCone = existing;
    }
    reporter::stop("Creating paths");
    
    return paths;
}

#define PI 3.141592
float angleTest = 0;
void RIV3DView::drawLightCones(const std::map<size_t,LightCone*>& lightCones) {
    GLUquadric* qobj = gluNewQuadric();
    int i = 0;
    int size = lightCones.size();
    for(auto pair : lightCones) {
        glColor3f(i/(float)size, 0, 0);
        LightCone* cone = pair.second;

//        cone->origin = 0;
//        cone->target = 1;
        Vec3fa diff = cone->target - cone->origin;
//        float max = std::max(diff.x,std::max(diff.y,diff.z));
        float diffLength = length(diff);
        Vec3fa dir = diff / diffLength;
//        glRotatef(360, dir.x, dir.y, dir.z);
        glPushMatrix();
        glTranslatef(cone->origin.x,cone->origin.y,cone->origin.z);
//        glTranslatef(modelCenter[0],modelCenter[1],modelCenter[2]);
        double toDegrees = 180.0 / PI;
        
        
//        float acosX = std::acos(dir.x / toDegrees) / 2;
//        float angleX = acosX * toDegrees;
//        float angleY = std::acos(dir.y / toDegrees)/2*toDegrees;
//        float angleZ = std::acos(dir.z / toDegrees)/2*toDegrees;
        
        //DEGREES
//        float angleX = 360 - std::atan(diff.y / diff.z) * toDegrees;
        //RAD
        float angleX = std::atan2(diff.y,diff.z);
        

//        angle *= toDegrees;
          

//        

        if(diff.y < 0 || diff.z < 0) {
            //DEGREES
//            angleX += 180;
            //RAD
//            angleX += PI;
        }
        float angleY = std::atan2(diff.x,diff.z);
        if(diff.x < 0 || diff.z < 0) {
//            angleY += 180;
//            angleY += PI;
        }
        float angleZ = std::atan2(diff.x,diff.y);
        if(diff.y < 0 || diff.x < 0) {
//            angleZ += 180;
//            angleZ += PI;
        }
        Vec3f angle(angleX,angleY,angleZ);
//        glPushMatrix();
//        glRotatef(angle.z,0,0,1);
//        glPopMatrix();
//        glPushMatrix();
        
        GLdouble xRotationMatrix[16] =
        {1,0,0,0,
         0,std::cos(angleX),-std::sin(angleX),0,
         0,std::sin(angleX), std::cos(angleX),0,
         0,0,0,1};
        
//        angleTest += 0.1;
//        angleY = angleTest;
        GLdouble yRotationMatrix[16] =
        {std::cos(angleY),0,-std::sin(angleY),0,
         0,1,0,0,
         std::sin(angleY),0,std::cos(angleY),0,
         0,0,0,1};
        
        GLdouble zRotationMatrix[16] =
        {std::cos(angleZ),-sin(angleZ),0,0,
         std::sin(angleZ),std::cos(angleZ),0,0,
         0,0,1,0,
         0,0,0,1};
        
        glPushMatrix();
//        glRotatef(45,.5,.5,0);
        glMultMatrixd(xRotationMatrix);
        glMultMatrixd(yRotationMatrix);
        glMultMatrixd(zRotationMatrix);
        
        
//        glPopMatrix();
//        glPushMatrix();
//        glRotatef(angle.y,0,1,0);
//        glPopMatrix();
//        glTranslatef(-cone->origin.x,-cone->origin.y,-cone->origin.z);
        
//        glPushMatrix();

//        glRotatef(angleZ,0,0,1);
//        glRotatef(1, angleX, angleY, angleZ);
        std::cout << " dir = " << dir << std::endl;
        std::cout << " origin = " << cone->origin << std::endl;
        std::cout << " target = " << cone->target << std::endl;
        printf("angle = %f,%f,%f\n",angle.x*toDegrees,angle.y*toDegrees,angle.z*toDegrees);
        
//        float angleY
//        float angleZ
        

//        glTranslatef(cone->origin.x,cone->origin.y,cone->origin.z);
//        glRotatef(angle, 360*dir.x,360*dir.y,360*dir.z);
//        ++angle;
        

        
        float heightCylinder = length(diff);
        gluCylinder(qobj, 10.0, 40.0, heightCylinder, 8, 16);
//        glPopMatrix();
        glPopMatrix();
//        glPopMatrix();
        glColor3f(0, i/(float)size, 0);
        glBegin(GL_LINES);
        glVertex3f(cone->origin.x, cone->origin.y, cone->origin.z);
        glVertex3f(cone->origin.x+diff.x,cone->origin.y+ diff.y,cone->origin.z+ diff.z);
        //            glVertex3f(cone->target.x, cone->target.y, cone->target.z);
        glEnd();
        ++i;
    }
    gluDeleteQuadric(qobj);
}
void RIV3DView::MovePathSegment(float ratioIncrement) {
    if(drawLightPaths && (drawDataSetOne || drawDataSetTwo)) {
        segmentStart += ratioIncrement;
        segmentStop += ratioIncrement;
        
        float undershoot = 0 - segmentStart;
        if(undershoot > 0) {
            segmentStart += undershoot;
            segmentStop += undershoot;
        }
        float overshoot = segmentStop - 1.F;
        if(overshoot > 0) {
            segmentStart -= overshoot;
            segmentStop -= overshoot;
        }
        redisplayWindow();
        isDirty = true;
    }
}

void RIV3DView::drawPaths(float startSegment, float stopSegment) {
    if(drawDataSetOne) {
        drawPaths((*datasetOne),pathsOne, startSegment, stopSegment,cameraPositionOne);
    }
    if(datasetTwo && drawDataSetTwo) {
        drawPaths((*datasetTwo),pathsTwo, startSegment, stopSegment,cameraPositionTwo);
    }
}

void RIV3DView::drawPaths(RIVDataSet<float,ushort>* dataset, const std::vector<Path>& paths, float startSegment, float stopSegment, const Vector3f& cameraPosition) {
    //See if it should consist of two partial segments
    for(float i = 1 ; i < maxBounce ; i++) {
        if(startSegment < i / maxBounce && stopSegment > i / maxBounce) {
            //                printf("(%f,%f) segment split in (%f,%f) and (%f,%f)\n",startSegment,stopSegment,startSegment,i/maxBounce,i/maxBounce,stopSegment);
            drawPaths(dataset,paths,startSegment, i / maxBounce,cameraPosition);
            drawPaths(dataset,paths,i / maxBounce, stopSegment,cameraPosition);
            return;
        }
    }
    
    //Start and end vertex index
    int startBounce = floor(startSegment * maxBounce);
    int endBounce = startBounce + 1;
    
    RIVTable<float,ushort>* intersectionsTable = dataset->GetTable("intersections");
    RIVFloatRecord* xRecord = intersectionsTable->GetRecord<float>("x");
    RIVFloatRecord* yRecord = intersectionsTable->GetRecord<float>("y");
    RIVFloatRecord* zRecord = intersectionsTable->GetRecord<float>("z");
    
    //	printf("start,end bounce = %d,%d\n",startBounce,endBounce);

    if(startBounce == 0) {
        for(const Path& path : paths) {
            PathPoint *p = path.GetPointWithBounce(1);
            const riv::Color& pathColor = path.pathColor;
            glColor3f(pathColor.R,pathColor.G,pathColor.B);
            if(p != NULL) {
                
                float deltaX = xRecord->Value(p->rowIndex) - cameraPosition[0];
                float deltaY = yRecord->Value(p->rowIndex) - cameraPosition[1];
                float deltaZ = zRecord->Value(p->rowIndex) - cameraPosition[2];
                
                float endX = cameraPosition[0] + deltaX * stopSegment * maxBounce;
                float endY = cameraPosition[1] + deltaY * stopSegment * maxBounce;
                float endZ = cameraPosition[2] + deltaZ * stopSegment * maxBounce;
                
//                glColor3f(1,1,1);

                glBegin(GL_LINES);
                    glVertex3f(cameraPosition[0] + deltaX * startSegment * maxBounce,cameraPosition[1] + deltaY * startSegment * maxBounce,cameraPosition[2] + deltaZ * startSegment * maxBounce);
                    glVertex3f(endX,endY,endZ);
                glEnd();

                //TODO: Trying to draw a pointer at the end of the path
//                glPushMatrix();
//                glScalef(modelScale, modelScale, modelScale);
//                glTranslatef(endX, endY, endZ);
//                gluSphere(quadric, 1, 4, 4);
//                glPopMatrix();
            }
        }
    }
    else {
        for(const Path& path : paths) {
            if(path.Size() >= 2) {
                PathPoint* startPoint = path.GetPointWithBounce(startBounce);
                PathPoint* endPoint = path.GetPointWithBounce(endBounce);
                const riv::Color& pathColor = path.pathColor;
                glColor3f(pathColor.R,pathColor.G,pathColor.B);
                if(startPoint != NULL && endPoint != NULL) {
                    
                    float Cstart = startSegment * maxBounce - startBounce;
                    float Cend = stopSegment * maxBounce - startBounce;
                    
                    float startX = xRecord->Value(startPoint->rowIndex);
                    float startY = yRecord->Value(startPoint->rowIndex);
                    float startZ = zRecord->Value(startPoint->rowIndex);
                    
                    float endX = xRecord->Value(endPoint->rowIndex);
                    float endY = yRecord->Value(endPoint->rowIndex);
                    float endZ = zRecord->Value(endPoint->rowIndex);
                    
                    float deltaX = endX - startX;
                    float deltaY = endY - startY;
                    float deltaZ = endZ - startZ;
                    
                    glBegin(GL_LINES);

                        glVertex3f(startX + deltaX * Cstart, startY + deltaY * Cstart, startZ + deltaZ * Cstart);
                        glVertex3f(startX + deltaX * Cend, startY + deltaY * Cend, startZ + deltaZ * Cend);
                    glEnd();
                }
            }
        }
    }

    //	reporter::stop(taskname);
}

void RIV3DView::ToggleDrawPaths() {
    //Create the paths if necessary (allows for smoother animations)
    printf("Display light paths is now ");
    drawLightPaths = !drawLightPaths;
    if(drawLightPaths) {
        printf("ON\n");
    }
    else {
        printf("OFF\n");
    }
    if(drawLightPaths && !pathsCreated) {
        createPaths();
    }
    else if(!drawLightPaths && !drawIntersectionPoints) {
        //		paths.clear();
        //		pathsCreated = false;
    }
    redisplayWindow();
    isDirty = true;
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

void RIV3DView::OnDataChanged(RIVDataSet<float,ushort>* source) {
    //Nothing
    if(source == *datasetOne) {
        pathColorOne->Reset(source);
        rayColorOne->Reset(source);
        if(bounceCountOne > 0) {
            filterPaths((*datasetOne), bounceCountOne, selectedObjectIdOne, pathFiltersOne);
        }
        createPaths((*datasetOne), pathColorOne,rayColorOne,lightConesOne);
    }
    else if(source == *datasetTwo) {
        pathColorTwo->Reset(source);
        rayColorTwo->Reset(source);
        if(bounceCountTwo > 0) {
            filterPaths((*datasetTwo), bounceCountTwo, selectedObjectIdTwo, pathFiltersTwo);
        }
        createPaths((*datasetTwo), pathColorTwo,rayColorTwo,lightConesTwo);
    }
    
    //	TODO: Paths and points are stale when this happens, but recreation is not necessary unless drawPoints or drawPaths is set to TRUE
    //	createPaths();
}
void RIV3DView::redisplayWindow() {
    int currentWindow = glutGetWindow();
    glutSetWindow(RIV3DView::windowHandle);
    glutPostRedisplay();
    //Return window to given window
    glutSetWindow(currentWindow);
    
    isDirty = true;
}
void RIV3DView::OnFiltersChanged(RIVDataSet<float,ushort>* source) {
    printf("3D View received on filter change.");
    
    if(source == *datasetOne || (datasetTwo != NULL && source == *datasetTwo)) {
        ResetGraphics();
    }
    else {
        throw std::runtime_error("Unknown dataset");
    }
    
    redisplayWindow();
}
void RIV3DView::ZoomIn(float zoom) {
    tb_zoom(zoom);
    
    redisplayWindow();
}
void RIV3DView::MoveCamera(float x, float y, float z) {
//    eye.x += x;
//    eye.y += y;
//    eye.z += z;
    
    
//    tb_zoom(1, 0);
//    tb_zoom(0, 1);
    
//    printf("new eye (x,y,z) = (%f,%f,%f)\n",eye.x,eye.y,eye.z);
//    isDirty = true;
}

Vec3fa RIV3DView::screenToWorldCoordinates(int screenX, int screenY, float zPlane) {
    
    Vec3fa worldPos;
    
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
    
    double x,y,z;
    gluUnProject((GLdouble) screenX, (GLdouble) screenY, zPlane, mvmatrix, projmatrix, viewport, &x,&y,&z);
    
    worldPos[0] = x;
    worldPos[1] = y;
    worldPos[2] = z;
    //    printf("worldPos = [%f,%f,%f]\n",worldPos.x,worldPos.y,worldPos.z);
    
    return worldPos;
}
void RIV3DView::filterPaths(RIVDataSet<float,ushort>* dataset, ushort bounceNr, ushort selectedObjectID, std::vector<riv::RowFilter*>& pathFilters) {
    dataset->StartFiltering();
    
    RIVTable<float,ushort>* pathTable = dataset->GetTable(PATHS_TABLE);
    RIVTable<float,ushort>* isectsTable = dataset->GetTable(INTERSECTIONS_TABLE);
    auto ref = pathTable->GetReferenceTo(INTERSECTIONS_TABLE);
    RIVMultiReference* pathIsectReference = dynamic_cast<RIVMultiReference*>(ref);
    RIVMultiReference* isectsToLightsReference = dynamic_cast<RIVMultiReference*>(isectsTable->GetReferenceTo(LIGHTS_TABLE));
    RIVTable<float,ushort>* lightsTable = dataset->GetTable(LIGHTS_TABLE);
    
    auto intersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
    auto primitiveIds = intersectionsTable->GetRecord<ushort>(PRIMITIVE_ID);
    auto bounceNrs = intersectionsTable->GetRecord<ushort>(BOUNCE_NR);
    auto occluderIds = lightsTable->GetRecord<ushort>(OCCLUDER_ID);
    

    std::map<size_t,bool> filteredRows;
//    printf("BEFORE PATH FILTERING : \n");
//    intersectionsTable->Print();
    
//    if(pathFilter != NULL) { //Add to the previous filter
        //            dataset->ClearRowFilter(pathFilter);
//    }
    
//    dataset->Print(5000);
    printf("Path filtering bounce# = %d selectedObjectID = %d\n",bounceNr,selectedObjectID);
    TableIterator* iterator = pathTable->GetIterator();
    size_t row;
    while(iterator->GetNext(row)) {
        const auto& mapping = pathIsectReference->GetReferenceRows(row);
        ushort nrIntersections = mapping.second;
        size_t* intersectionRows = mapping.first;
        bool filter = true;
        for(ushort i = 0 ; i < nrIntersections ; ++i) {
            size_t intersectionRow = intersectionRows[i];
            
            //Check if it has occluders and if the selectedObjectID is in them
            if(selectionMode == OBJECT) {
                if(primitiveIds->Value(intersectionRow) == selectedObjectID) {
                    filter = false;
                    break;
                }
                //Only when the first bounce is in the shadow
                else if(i == 0) {
                    const auto& lightsMapping = isectsToLightsReference->GetReferenceRows(intersectionRow);
                    ushort nrLightRows = lightsMapping.second;
                    size_t* lightRefRows = lightsMapping.first;
                    for(ushort j = 0 ; j < nrLightRows ; ++j) {
                        size_t lightRow = lightRefRows[j];
                        ushort occluderId = occluderIds->Value(lightRow);
                        if(occluderId == selectedObjectID) {
                            filter = false;
                            printf("Path %zu intersection #%zu occluder %zu has occluder id = %d\n",row,intersectionRow,lightRow,occluderId);
                            break;
                        }
                    }
                }
            }
            else if(selectionMode == PATH && primitiveIds->Value(intersectionRow) == selectedObjectID && bounceNrs->Value(intersectionRow) == bounceNr) {
                filter = false;
                break;
            }
        }
        if(filter) {
            filteredRows[row] = true;
        }
    }
    
    riv::RowFilter* pathFilter = new riv::RowFilter(PATHS_TABLE, filteredRows);
    dataset->AddFilter(pathFilter);
    pathFilters.push_back(pathFilter);
    
    dataset->StopFiltering();
}
void RIV3DView::CycleSelectionMode() {
    switch (selectionMode) {
        case PATH:
            selectionMode = OBJECT;
            printf("Selection mode is now set to 'OBJECT'\n");
            break;
        case OBJECT:
            selectionMode = PATH;
            printf("Selection mode is now set to 'PATH'\n");
            break;
    }
}
//Checks if a ray intersects with the mesh group and creates the path filters accordingly and applies them to the dataset
bool RIV3DView::pathCreation(RIVDataSet<float,ushort>* dataset, const TriangleMeshGroup& meshes, std::vector<riv::RowFilter*>& pathFilters, ushort* bounceCount, ushort* selectedObjectId) {
    ushort selectedObjectID;
    float distance;
    bool intersects = meshes.Intersect(pickRay, selectedObjectID, Phit, distance, eye);
    
    bool refilterNeeded = false;
    
    if(intersects && *bounceCount < maxBounce) {
        ++(*bounceCount);
        printf("new selected object ID = %hu\n",selectedObjectID);
        meshSelected = true;
        *selectedObjectId = selectedObjectID;
        refilterNeeded = true;
    }
    else {
        //        *selectedObjectId = -1;
        return false;
    }
    //
    printf("Path creation filter");
    filterPaths(dataset, *bounceCount, selectedObjectID, pathFilters);

    //		dataset->Print();
    
    return refilterNeeded;
}
bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
    y = height - y;
    if(state == GLUT_DOWN) {
        if(button == GLUT_LEFT_BUTTON) {
            tbMouseFunc(button, state, width-x, y);

            isDragging = true;
            
            //Determine the world space cordinates on the near and far plane for the selected pixel
            Vec3fa selectNear = screenToWorldCoordinates(x, y, 0);
            Vec3fa selectFar = screenToWorldCoordinates(x, y, 1);
            
            Vec3fa modelPosition = -modelCenter;
            float reverseScaleModel = 1 / modelScale;
            
            Vec3fa dest = reverseScaleModel * selectNear;
            dest = dest - modelPosition;
            Vec3fa origin = reverseScaleModel * selectFar;
            origin = origin - modelPosition;
            
            Vec3fa dir = dest - origin;
            pickRay = Ray(origin, dir);
            
            return true;
        }
        else if(button == GLUT_RIGHT_BUTTON) { //Clear paths created
            tbMouseFunc(button, state, width -x, y);
            if(pathFiltersOne.size()) {
                bounceCountOne = 0;
                (*datasetOne)->StartFiltering();
                for(riv::RowFilter* pathFilter : pathFiltersOne) {
                    (*datasetOne)->ClearRowFilter(pathFilter);
                }
                (*datasetOne)->StopFiltering();
                
                pathFiltersOne.clear();
                createPaths(*datasetOne,pathColorOne,rayColorOne,lightConesOne);
                selectedObjectIdOne = -1;
            }
            if(pathFiltersTwo.size() && datasetTwo) {
                bounceCountTwo = 0;
                (*datasetTwo)->StartFiltering();
                for(riv::RowFilter* pathFilter : pathFiltersTwo) {
                    (*datasetTwo)->ClearRowFilter(pathFilter);
                }
                (*datasetTwo)->StopFiltering();
                
                pathFiltersTwo.clear();
                selectedObjectIdTwo = -1;
                createPaths(*datasetTwo,pathColorTwo,rayColorTwo,lightConesTwo);
            }
            return true;
        }
        else if(button == GLUT_MIDDLE_BUTTON) {
            printf("Middle button used.\n");
        }
        return false;
    }
    else {
        tbMouseFunc(button, state, width -x, y);
        if(!didMoveCamera && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
            if(!drawDataSetOne && drawDataSetTwo) {
                pathCreation(*datasetTwo, meshesTwo,pathFiltersTwo,&bounceCountTwo,&selectedObjectIdTwo);
            }
            else if(drawDataSetOne && !drawDataSetTwo) {
                pathCreation(*datasetOne, meshesOne,pathFiltersOne, &bounceCountOne,&selectedObjectIdOne);
            }
            else if(drawDataSetOne && drawDataSetTwo) {
                pathCreation(*datasetOne, meshesOne,pathFiltersOne, &bounceCountOne,&selectedObjectIdOne);
                pathCreation(*datasetTwo, meshesTwo,pathFiltersTwo, &bounceCountTwo,&selectedObjectIdTwo);
            }
        }
        didMoveCamera = false;
        isDragging = false;
        return true;
    }
}
bool RIV3DView::HandleMouseMotion(int x, int y) {
    y = height - y;
    tbMotionFunc(width-x, y);
    redisplayWindow();
    if(isDragging) {
        didMoveCamera = true;
    }
    return true;
}
void RIV3DView::IncrementHeatmapDepth() {
    if(drawHeatmapDepth < energyDistributionOne->Depth() && (energyDistributionTwo == NULL || drawHeatmapDepth < energyDistributionTwo->Depth())) {
        drawHeatmapDepth++;
        redisplayWindow();
    }
}
void RIV3DView::DecrementHeatmapDepth() {
    if(drawHeatmapDepth > 1) {
        --drawHeatmapDepth;
        redisplayWindow();
    }
}
