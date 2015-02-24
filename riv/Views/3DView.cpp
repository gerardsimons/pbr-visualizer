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

RIV3DView::RIV3DView(RIVDataSet<float,ushort>** dataset,EMBREERenderer* renderer, const TriangleMeshGroup& sceneDataOne, RIVColorProperty* colorPropertyOne, RIVSizeProperty* sizeProperty) : RIVDataView(dataset),rendererOne(renderer), sizeProperty(sizeProperty), colorPropertyOne(colorPropertyOne) {
    
    if(instance != NULL) {
        throw std::runtime_error("Only 1 instance of RIV3DView allowed.");
    }
    instance = this;
    identifier = "3DView";
    
    this->meshesOne = sceneDataOne;
    
//    scale = 5 * meshesOne.GetScale();
    scale = meshesOne.GetScale();
    center = meshesOne.GetCenter();
    drawDataSetTwo = false;
    
    cameraPositionOne = renderer->GetCameraPosition();
    eye = cameraPositionOne;
    
    ResetGraphics();
};

RIV3DView::RIV3DView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo,EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo, const TriangleMeshGroup& sceneDataOne, const TriangleMeshGroup& sceneDataTwo, RIVColorProperty* colorPropertyOne, RIVColorProperty* colorPropertyTwo, RIVSizeProperty* sizeProperty) :
RIVDataView(datasetOne,datasetTwo), rendererOne(rendererOne), rendererTwo(rendererTwo), sizeProperty(sizeProperty), colorPropertyOne(colorPropertyOne), colorPropertyTwo(colorPropertyTwo) {
    
    if(instance != NULL) {
        throw std::runtime_error("Only 1 instance of RIV3DView allowed.");
    }
    instance = this;
    identifier = "3DView";

    this->meshesOne = sceneDataOne;
    this->meshesTwo = sceneDataTwo;
    
    Vec3f modelCenterOne = meshesOne.GetCenter();
    
    scale = meshesOne.GetScale();
    center = meshesOne.GetCenter();
    
    if(datasetTwo && drawDataSetTwo) {
        float scaleTwo = meshesTwo.GetScale();
        if(scaleTwo > scale) {
            scale = scaleTwo;
        }
        Vec3fa modelCenterTwo = meshesTwo.GetCenter();
        float lengthOne = length(modelCenterOne);
        float lengthTwo = length(modelCenterTwo);
        if( lengthTwo > lengthOne ) {
            center = modelCenterTwo;
        }
    }
    cameraPositionOne = rendererOne->GetCameraPosition();
    cameraPositionTwo = rendererTwo->GetCameraPosition();
    eye = cameraPositionOne;
    ResetGraphics();
};


void RIV3DView::CyclePathSegment(bool direction) {
    float delta = 1.F / maxBounce;
    direction ? MovePathSegment(delta) : MovePathSegment(-delta);
}

void RIV3DView::Reshape(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    
    eye = cameraPositionOne;
    
//    eye[0] = 0;
//    eye[1] = 0.8;
//    eye[2] = 3;
    
//    tbInitTransform();
//    tbHelp();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double)width/height, .1, 100);
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
    if(drawHeatmapTree && !heatmap) {
        generateOctree(heatmapDepth, 1, .00001F);
    }
    isDirty = true;
}
size_t nodesDrawn;
//Test function to draw a simple octree
void RIV3DView::drawHeatmap() {
    if(heatmap) {
        //Draw the tree to the required depth
        reporter::startTask("Heatmap drawing.");
        OctreeNode* root = heatmap->GetRoot();
        nodesDrawn = 0;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if(root) {
            glPolygonMode(GL_FRONT, GL_FILL);
            drawLeafNodes(root,heatmap->MaxEnergyOne(),heatmap->MaxEnergyTwo());
        }
        printf("%zu / %zu nodes drawn\n",nodesDrawn,heatmap->NumberOfNodes());
        reporter::stop("Heatmap drawing.");
    }
}

void RIV3DView::drawLeafNodes(OctreeNode* node,float maxEnergyOne, float maxEnergyTwo) {
    
    bool membershipColoring = true;
    
    if(node->IsLeafNode() && node->ContainsAnyPoints()) { //Draw it
        
        Point3D nodeCenter = node->Center();
        //		size_t depth = node->GetDepth();
        //        size_t pointsInNode = node->NumberOfPointsContained();
        //		float density = node->Density();
        
        //Determine color according to number of children
        //		size_t maxCap = heatmap->MaxCapacity();
        //		float ratio = (pointsInNode) / (float)(maxCap);
        
        float energyOne = node->ComputeEnergyOne();
        float energyTwo = node->ComputeEnergyTwo();
        
        //        printf("Max energy one = %f\n",maxEnergyOne);
        //        printf("Max energy two = %f\n",maxEnergyTwo);
        //        printf("energy one = %f\n",energyOne);
        //        printf("energy two = %f\n",energyTwo);
        
        //        float maxEnergy = std::max(energyOne,energyTwo);
        
        riv::Color cubeColor;
        //        cubeColor.A = 0;
        float r,b;
        float a = .5F;
        if(!membershipColoring) {
            if(drawDataSetOne && !drawDataSetTwo) {
                float ratio = energyOne / maxEnergyOne;
//                printf("ratio = %f\n",ratio);
                cubeColor = treeColorMap.ComputeColor(ratio);
            }
            else if(!drawDataSetOne && drawDataSetTwo) {
                float ratio = energyTwo / maxEnergyTwo;
//                printf("ratio = %f\n",ratio);
                cubeColor = treeColorMap.ComputeColor(ratio);
            }
        }
        else {
            //red, blue and alpha
            //Should be blue-ish
            if(energyTwo > energyOne) {
                
                //                printf("Energy two is higher : \n");
                //                b = energyTwo / maxEnergy;
                //                r = energyOne / maxEnergy;
                
                b = ((energyTwo - energyOne) / energyTwo + 1) / 2.F;
                
                //                b = (energyTwo - energyOne) / energyTwo;
                //                b = 1;
                r = 1 - b;
                a = energyTwo / maxEnergyTwo;
                
                //                printf("r,b,a = %f,%f,%f\n",r,b,a);
                
                //                return;
                //                b = 1;
                //                r = 0;
            }
            else if(energyOne > energyTwo) {
                //                b = energyTwo / maxEnergy;
                //                r = energyOne / maxEnergy;
                
                //                printf("Energy one is higher : \n");
                
                //                r = (energyOne - energyTwo) / energyOne;
                r = ((energyOne - energyTwo) / energyOne + 1) / 2.F;
                //                r = 1;
                b = 1 - r;
                a = energyOne / maxEnergyOne;
                
                //                printf("r,b,a = %f,%f,%f\n",r,b,a);
                
                //                b = 0;
                //                r = 1;
            }
            else {
                return;
            }
            
            //            float ratio = (energyTwo / maxEnergyTwo) - (energyOne / maxEnergyOne);
            //            printf("ratio = %f\n",ratio);
            //            cubeColor.R = r;
            //            cubeColor.G = 0;
            //            cubeColor.B = b;
            //            cubeColor.A = a;
            
        }
        //        a = std::pow(a, .5);
        glColor4f(r,0,b,a);
        //        if(ratio < 0) {
        //            ratio = -ratio;
        //        }
        glPushMatrix();
        glTranslatef(nodeCenter.x, nodeCenter.y, nodeCenter.z);
        //		glutWireCube(node->GetSize());
        glutSolidCube(node->GetSize());
        glPopMatrix();
        
        ++nodesDrawn;
    }
    else { //Recursively call the function on the children
        for(int i = 0 ; i < node->NumberOfChildren() ; ++i) {
            drawLeafNodes(node->GetChild(i),maxEnergyOne,maxEnergyTwo);
        }
    }
}

size_t drawCounter_ = 1;

void RIV3DView::ToggleDrawDataSetOne() {
    drawDataSetOne = !drawDataSetOne;
}

void RIV3DView::ToggleDrawDataSetTwo() {
    if(datasetTwo) {
        drawDataSetTwo = !drawDataSetTwo;
    }
    else printf("No second dataset set.");
}

//void RIV3DView::Draw(RIVDataSet<float,ushort>* dataset) {
//
//}
float angle = 0;

void RIV3DView::Draw() {

    
    //	reporter::startTask("3D Draw");
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 0.0); //White
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Somehow it is mirrored so lets mirror it again to match the image
    
    //    printf("eye (x,y,z) * modelScale = (%f,%f,%f)\n",-eye.x * modelData.GetScale(),-eye.y * modelData.GetScale(),-eye.z * modelData.GetScale());
    
    glPushMatrix();
    tbVisuTransform();
//    glTranslatef(-center[0], -center[1], -center[2]);
    glScalef(-1, 1, 1);
    glTranslatef(-eye.x,-eye.y,-eye.z);
//    glRotatef(angle++, 0, 1, 0);
//    glScalef(-scale,scale,scale);
    drawCoordSystem();
    
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
        glScalef(.01*scale, .01*scale, .01*scale);
        gluSphere(quadric, 2, 9, 9);
        glPopMatrix();
    }
    
    //Draw
    float cameraScale = scale * .3;
    glColor3f(1, .2, .2);
    //Draw camera position
    glPushMatrix();
    glTranslatef(cameraPositionOne[0],cameraPositionOne[1],cameraPositionOne[2]);
    glScalef(cameraScale,cameraScale,cameraScale);
    //    glScalef(0.01, 0.01, 0.01);x§
    gluSphere(quadric, 10, 10, 10);
    glPopMatrix();
    
    glColor3f(.2, .2, 1);
    //Draw camera position
    glPushMatrix();
    glTranslatef(cameraPositionTwo[0],cameraPositionTwo[1],cameraPositionTwo[2]);
    glScalef(cameraScale,cameraScale,cameraScale);
    //    glScalef(0.01, 0.01, 0.01);x§
    gluSphere(quadric, 10, 10, 10);
    glPopMatrix();
    
    if(drawHeatmapTree && heatmap != NULL)
        drawHeatmap();
    
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
    
    //Only use 1 size
    float size = sizeProperty->ComputeSize(isectTable, 0);
    glPointSize(size);
    
    size_t row = 0;
    TableIterator* it = isectTable->GetIterator();
    
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
void RIV3DView::SetHeatmapDepth(int depth) {
    heatmapDepth = depth;
    
    if(drawHeatmapTree) {
        delete heatmap;
        
        generateOctree(heatmapDepth, 1, 0);
        
        Invalidate();
    }
}
void RIV3DView::IncrementHeatmapDepth() {
    SetHeatmapDepth(heatmapDepth++);
}
void RIV3DView::DecrementHeatmapDepth() {
    if(heatmapDepth > 0) {
        SetHeatmapDepth(heatmapDepth--);
    }
}
//Move this function somewhere else
void RIV3DView::generateOctree(size_t maxDepth, size_t maxCapacity, float minNodeSize) {
    
    std::string taskName = "Generating octree";
    reporter::startTask(taskName);
    
    if(heatmap) {
        //		delete heatmap;
    }
    
    size_t row;
    
    RIVTable<float,ushort>* isectTableOne = (*datasetOne)->GetTable(INTERSECTIONS_TABLE);
    
    TableIterator* iterator = isectTableOne->GetIterator();
    
    //Generate the index subset
    std::vector<size_t> indices;
    
    while(iterator->GetNext(row)) {
        indices.push_back(row);
    }
    
    RIVFloatRecord* xsOne = isectTableOne->GetRecord<float>(POS_X);
    RIVFloatRecord* ysOne = isectTableOne->GetRecord<float>(POS_Y);
    RIVFloatRecord* zsOne = isectTableOne->GetRecord<float>(POS_Z);
    
    RIVFloatRecord* rsOne = isectTableOne->GetRecord<float>(INTERSECTION_R);
    RIVFloatRecord* gsOne = isectTableOne->GetRecord<float>(INTERSECTION_G);
    RIVFloatRecord* bsOne = isectTableOne->GetRecord<float>(INTERSECTION_B);
    
    OctreeConfig config = OctreeConfig(maxDepth, maxCapacity, minNodeSize);
    if(datasetTwo) {
        RIVTable<float,ushort>* isectTableTwo = (*datasetTwo)->GetTable(INTERSECTIONS_TABLE);
        TableIterator* iteratorTwo = isectTableTwo->GetIterator();
        
        //Generate the index subset
        std::vector<size_t> indicesTwo;
        
        while(iteratorTwo->GetNext(row)) {
            indicesTwo.push_back(row);
        }
        
        RIVFloatRecord* xsTwo = isectTableTwo->GetRecord<float>(POS_X);
        RIVFloatRecord* ysTwo = isectTableTwo->GetRecord<float>(POS_Y);
        RIVFloatRecord* zsTwo = isectTableTwo->GetRecord<float>(POS_Z);
        
        RIVFloatRecord* rsTwo = isectTableTwo->GetRecord<float>(INTERSECTION_R);
        RIVFloatRecord* gsTwo = isectTableTwo->GetRecord<float>(INTERSECTION_G);
        RIVFloatRecord* bsTwo = isectTableTwo->GetRecord<float>(INTERSECTION_B);
        
        heatmap = new Octree(xsOne, ysOne, zsOne, rsOne, gsOne, bsOne, xsTwo, ysTwo, zsTwo, rsTwo, gsTwo, bsTwo, indices, indicesTwo, config);
    }
    else {
        heatmap = new Octree(xsOne, ysOne, zsOne, rsOne, gsOne, bsOne, indices, config);
    }
    
    printf("Tree generated with \n");
    printf("\tDepth %zu\n",heatmap->Depth());
    printf("\tNodes = %zu\n",heatmap->NumberOfNodes());
    
    //Generate the color map used by the tree
    treeColorMap = colors::jetColorMap();
    
    reporter::stop(taskName);
}

void RIV3DView::ResetGraphics() {
    if(heatmap) {
        //		delete heatmap;
        //		generateOctree(7, 1, .00001F);
    }
    if(drawLightPaths || drawIntersectionPoints) {
        createPaths();
    }
}

//(Re)create the paths objects for the datasets being used
void RIV3DView::createPaths() {
    pathsOne = createPaths(*datasetOne,colorPropertyOne);
    if(datasetTwo) {
        pathsTwo = createPaths(*datasetTwo,colorPropertyTwo);
    }
    pathsCreated = true;
}

//Create buffered data for points, not working anymore, colors seem to be red all the time.
std::vector<Path> RIV3DView::createPaths(RIVDataSet<float,ushort>* dataset, RIVColorProperty* colorProperty) {
    
    reporter::startTask("Creating paths");
    
    RIVTable<float,ushort>* isectTable = dataset->GetTable("intersections");
    RIVShortRecord* bounceRecord = isectTable->GetRecord<ushort>("bounce_nr");
    
    //	colorProperty->SetColorRecords(isectTable->GetRecord<float>(INTERSECTION_R),isectTable->GetRecord<float>(INTERSECTION_G),isectTable->GetRecord<float>(INTERSECTION_B));
    
    std::vector<Path> paths;
    
    //Get the records we want;
    //Get the iterator, this iterator is aware of what rows are filtered and not
    TableIterator* iterator = isectTable->GetIterator();
    
    size_t row = 0;
    size_t *pathID = 0;
    size_t oldPathID = 0;
    ushort bounceNr;
    
    std::vector<PathPoint> points;
    
    while(iterator->GetNext(row,pathID)) {
        if(*pathID != oldPathID && points.size() > 0) {
            paths.push_back(Path(points));
            points.clear();
            oldPathID = *pathID;
        }
        bounceNr = bounceRecord->Value(row);
        riv::Color pointColor;
        colorProperty->ComputeColor(isectTable, row, pointColor); //Check if any color can be computed for the given row
        PathPoint p;
        p.rowIndex = row;
        p.bounceNr = bounceNr;
        p.color = pointColor;
        points.push_back(p);
        oldPathID = *pathID;
    }
    reporter::stop("Creating paths");
    //	reportVectorStatistics("paths", paths);
    
    return paths;
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
    //	char taskname[100];
    //	sprintf(taskname,"drawPaths %f - %f\n",startSegment,stopSegment);
    //	reporter::startTask(taskname);
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
    
    glBegin(GL_LINES);
    if(startBounce == 0) {
        for(const Path& path : paths) {
            PathPoint *p = path.GetPointWithBounce(1);
            if(p != NULL) {
                
                float deltaX = xRecord->Value(p->rowIndex) - cameraPosition[0];
                float deltaY = yRecord->Value(p->rowIndex) - cameraPosition[1];
                float deltaZ = zRecord->Value(p->rowIndex) - cameraPosition[2];
                glColor3f(1,1,1);
                glVertex3f(cameraPosition[0] + deltaX * startSegment * maxBounce,cameraPosition[1] + deltaY * startSegment * maxBounce,cameraPosition[2] + deltaZ * startSegment * maxBounce);
                riv::Color& c = p->color;
                glColor3f(c.R,c.G,c.B);
                glVertex3f(cameraPosition[0] + deltaX * stopSegment * maxBounce,cameraPosition[1] + deltaY * stopSegment * maxBounce,cameraPosition[2] + deltaZ * stopSegment * maxBounce);
            }
            
            //			size_t point = path.GetPoint(0);
            //			Color c = path.GetColor(0);
            //
            //			glVertex3f(cameraPosition[0] + deltaX * startSegment * maxBounce,cameraPosition[1] + deltaY * startSegment * maxBounce,cameraPosition[2] + deltaZ * startSegment * maxBounce);
            //			glColor3f(c.R,c.G,c.B);
            //			glVertex3f(cameraPosition[0] + deltaX * stopSegment * maxBounce,cameraPosition[1] + deltaY * stopSegment * maxBounce,cameraPosition[2] + deltaZ * stopSegment * maxBounce);
        }
    }
    else {
        for(const Path& path : paths) {
            if(path.Size() >= 2) {
                PathPoint* startPoint = path.GetPointWithBounce(startBounce);
                PathPoint* endPoint = path.GetPointWithBounce(endBounce);
                
                if(startPoint != NULL && endPoint != NULL) {
                    riv::Color startColor = startPoint->color;
                    riv::Color endColor = endPoint->color;
                    
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
                    
                    glColor3f(startColor.R,startColor.G,startColor.B);
                    glVertex3f(startX + deltaX * Cstart, startY + deltaY * Cstart, startZ + deltaZ * Cstart);
                    glColor3f(endColor.R,endColor.G,endColor.B);
                    glVertex3f(startX + deltaX * Cend, startY + deltaY * Cend, startZ + deltaZ * Cend);
                }
            }
        }
    }
    glEnd();
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
        colorPropertyOne->Reset(source);
        if(bounceCountOne > 0) {
            filterPaths((*datasetOne), bounceCountOne, selectedObjectIdOne, pathFiltersOne);
        }
        createPaths((*datasetOne), colorPropertyOne);
    }
    else if(source == *datasetTwo) {
        colorPropertyTwo->Reset(source);
        if(bounceCountTwo > 0) {
            filterPaths((*datasetTwo), bounceCountTwo, selectedObjectIdTwo, pathFiltersTwo);
        }
        createPaths((*datasetTwo), colorPropertyTwo);
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
    
    ResetGraphics();
    
    redisplayWindow();
}

void RIV3DView::MoveCamera(float x, float y, float z) {
    eye.x += x;
    eye.y += y;
    eye.z += z;
    
    printf("new eye (x,y,z) = (%f,%f,%f)\n",eye.x,eye.y,eye.z);
    isDirty = true;
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
    
    auto pathTable = dataset->GetTable(PATHS_TABLE);
    auto reference = dynamic_cast<RIVMultiReference*>(pathTable->reference);
    
    auto intersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
    auto primitiveIds = intersectionsTable->GetRecord<ushort>(PRIMITIVE_ID);
    auto bounceNrs = intersectionsTable->GetRecord<ushort>(BOUNCE_NR);
    
    printf("Path filtering bounce# = %d selectedObjectID = %d\n",bounceNr,selectedObjectID);
    std::map<size_t,bool> filteredRows;
//    printf("BEFORE PATH FILTERING : \n");
//    intersectionsTable->Print();
    
//    if(pathFilter != NULL) { //Add to the previous filter
        //            dataset->ClearRowFilter(pathFilter);
//    }
    
    TableIterator* iterator = pathTable->GetIterator();
    size_t row;
    while(iterator->GetNext(row)) {
        const auto& mapping = reference->GetReferenceRows(row);
        ushort nrRows = mapping.second;
        size_t* refRows = mapping.first;
        bool filter = true;
        for(ushort i = 0 ; i < nrRows ; ++i) {
            size_t refRow = refRows[i];
            if(primitiveIds->Value(refRow) == selectedObjectID && bounceNrs->Value(refRow) == bounceNr) {
                
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
    
    printf("\n");
    dataset->StopFiltering();
    
//    printf("AFTER PATH FILTERING : \n");
//    intersectionsTable->Print();
    
    
}
//Checks if a ray intersects with the mesh group and creates the path filters accordingly and applies them to the dataset
bool RIV3DView::pathCreation(RIVDataSet<float,ushort>* dataset, const TriangleMeshGroup& meshes, std::vector<riv::RowFilter*>& pathFilters, ushort* bounceCount, ushort* selectedObjectId) {
    ushort selectedObjectID;
    float distance;
    bool intersects = meshes.Intersect(pickRay, selectedObjectID, Phit, distance);
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
//            return true;
            
            //Determine the world space cordinates on the near and far plane for the selected pixel
            Vec3fa selectNear = screenToWorldCoordinates(x, y, 0);
            Vec3fa selectFar = screenToWorldCoordinates(x, y, 1);
            
            Vec3fa modelPosition = -center;
            float reverseScaleModel = 1 / scale;
            
            Vec3fa dest = reverseScaleModel * selectNear;
            dest = dest - modelPosition;
            Vec3fa origin = reverseScaleModel * selectFar;
            origin = origin - modelPosition;
            
            Vec3fa dir = dest - origin;
            pickRay = Ray(origin, dir);
            
          
            return true;
        }
        else if(button == GLUT_RIGHT_BUTTON) { //Clear paths created
            if(pathFiltersOne.size()) {
                bounceCountOne = 0;
                (*datasetOne)->StartFiltering();
                for(riv::RowFilter* pathFilter : pathFiltersOne) {
                    (*datasetOne)->ClearRowFilter(pathFilter);
                }
                (*datasetOne)->StopFiltering();
                
                pathFiltersOne.clear();
                createPaths(*datasetOne,colorPropertyOne);
            }
            if(pathFiltersTwo.size() && datasetTwo) {
                bounceCountTwo = 0;
                (*datasetTwo)->StartFiltering();
                for(riv::RowFilter* pathFilter : pathFiltersOne) {
                    (*datasetTwo)->ClearRowFilter(pathFilter);
                }
                (*datasetTwo)->StopFiltering();
                
                pathFiltersTwo.clear();
                
                createPaths(*datasetTwo,colorPropertyTwo);
            }
            return true;
        }
        return false;
    }
    else {
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
    didMoveCamera = true;
    if(isDragging) {
        tbMotionFunc(width-x, y);
        redisplayWindow();
        return true;
    }
    return false;
}

