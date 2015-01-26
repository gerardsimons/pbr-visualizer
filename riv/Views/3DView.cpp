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

RIV3DView::RIV3DView(RIVDataSet<float,ushort>** dataset,EMBREERenderer* renderer, 	RIVColorRGBProperty<float>* colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(dataset),
rendererOne(renderer), sizeProperty(sizeProperty), colorProperty(colorProperty) {
	
    if(instance != NULL) {
		throw std::runtime_error("Only 1 instance of RIV3DView allowed.");
    }
    instance = this;
    identifier = "3DView";
	
	GetSceneData(rendererOne, &meshesOne);
	
	scale = meshesOne.GetScale();
	center = meshesTwo.GetCenter();
	
	ResetGraphics();
};

RIV3DView::RIV3DView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo,EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo, RIVColorRGBProperty<float>* colorProperty, RIVSizeProperty* sizeProperty) :
RIVDataView(datasetOne,datasetTwo), rendererOne(rendererOne), rendererTwo(rendererTwo), sizeProperty(sizeProperty), colorProperty(colorProperty) {
	
	if(instance != NULL) {
		throw std::runtime_error("Only 1 instance of RIV3DView allowed.");
	}
	instance = this;
	identifier = "3DView";
	
	GetSceneData(rendererOne, &meshesOne);
	GetSceneData(rendererTwo, &meshesTwo);
	
	Vec3f modelCenterOne = meshesOne.GetCenter();
	
	scale = meshesOne.GetScale();
	center = meshesTwo.GetCenter();
	
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
	
	ResetGraphics();
};

void RIV3DView::GetSceneData(EMBREERenderer* renderer, TriangleMeshGroup* target) {
	//Get the shapes and see what are trianglemeshes that we can draw
	std::vector<Shape*>* shapes = renderer->GetShapes();
	std::vector<TriangleMeshFull*> embreeMeshes;
	for(size_t i = 0 ; i < shapes->size() ; ++i) {
		TriangleMeshFull* t = dynamic_cast<TriangleMeshFull*>(shapes->at(i));
		if(t) {
			embreeMeshes.push_back(t);
		}
	}
	*target = TriangleMeshGroup(embreeMeshes);
}
void RIV3DView::CyclePathSegment(bool direction) {
    float delta = 1.F / maxBounce;
    direction ? MovePathSegment(delta) : MovePathSegment(-delta);
}

void RIV3DView::Reshape(int newWidth, int newHeight) {
    width = newWidth;
    this->height = newHeight;
    
    eye.x = 0;
    eye.y = 0;
    eye.z = 2.5;
    
//    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
    
    tbInitTransform();
    tbHelp();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double)width/height, 1, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RIV3DView::ToggleDrawIntersectionPoints() {
    drawIntersectionPoints = !drawIntersectionPoints;
	if(drawIntersectionPoints && !pathsCreated) {
		createPaths();
	}
	else if(!drawIntersectionPoints && !drawLightPaths) {
//		pathsCreated = false;
//		paths.clear();
	}
	printf("drawIntersectionPoints is now ");
	if(drawIntersectionPoints) printf("ON\n"); else printf("OFF\n");
	
	Invalidate();
	glutPostRedisplay();
}
void RIV3DView::ToggleDrawHeatmap() {
	drawHeatmapTree = !drawHeatmapTree;
	if(drawHeatmapTree && !heatmap) {
		generateOctree(7, 1, .00001F);
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
		if(root) {
			glPolygonMode(GL_FRONT, GL_FILL);
			drawLeafNodes(root);
		}
		printf("%zu / %zu nodes drawn\n",nodesDrawn,heatmap->NumberOfNodes());
		reporter::stop("Heatmap drawing.");
	}
}

void RIV3DView::drawLeafNodes(OctreeNode* node) {
	if(node->IsLeafNode() && node->ContainsAnyPoints()) { //Draw it
		
		Point3D nodeCenter = node->Center();
//		size_t depth = node->GetDepth();
		size_t pointsInNode = node->NumberOfPointsContained();
//		float density = node->Density();

		//Determine color according to number of children
		size_t maxCap = heatmap->MaxCapacity();
		float ratio = (pointsInNode - 1) / (float)(maxCap - 5);
		
//		float maxDensity = heatmap->MaxDensity();
//		float ratio = density / maxDensity;
//		ratio = 1.F - ratio;
//		float ratio = (pointsInNode) / ((float)heatmap->GetConfiguration()->MaxNodeCapacity());
		riv::Color cubeColor = treeColorMap.ComputeColor(ratio);
//		glColor3fv(color);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(cubeColor.R,cubeColor.G,cubeColor.B,.5F);
		
//		if(almost_equal(ratio,0.05,.1) || almost_equal(ratio,0,.001)) {
//		if(pointsInNode > 14) {
//			printf("Points in node = %zu\n",pointsInNode);
////			printf("Node density = %f\n",density);
////			printf("Max density = %f\n",maxDensity);
//			printf("MAX_NODE_CAPACITY = %zu\n",maxCap);
//			printf("Ratio = %f\n",ratio);
//			printf("Color = ");
//			printArray(color, 3);
//			printf("\n");
//		}
		
		glPushMatrix();
		glTranslatef(nodeCenter.x, nodeCenter.y, nodeCenter.z);
//		glutWireCube(node->GetSize());
		glutSolidCube(node->GetSize());
		glPopMatrix();
		
		++nodesDrawn;
		
//		printf("child center = ");
//		std::cout << childCenter << "\n";
//		printf("child size = %f\n",child->GetSize());
	}
	else { //Recursively call the function on the children
		for(int i = 0 ; i < node->NumberOfChildren() ; ++i) {
			drawLeafNodes(node->GetChild(i));
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

void RIV3DView::Draw() {
//    printf("3DView Draw #%zu\n",++drawCounter_);
	
//	reporter::startTask("3D Draw");
	
    glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	//Somehow it is mirrored so lets mirror it again to match the image
	glScalef(-1,1,1);
	glTranslatef(-eye.x,-eye.y,-eye.z);
//    glScalef(modelData.GetScale(), modelData.GetScale(), modelData.GetScale());
    
//    printf("eye (x,y,z) * modelScale = (%f,%f,%f)\n",-eye.x * modelData.GetScale(),-eye.y * modelData.GetScale(),-eye.z * modelData.GetScale());
    
    tbVisuTransform();
    
    drawCoordSystem();
	
	glPushMatrix();
	
	glScalef(scale,scale,scale);

	glTranslatef(-center[0], -center[1], -center[2]);
	
	if(drawDataSetOne) {
//		float purpleColor[3] =  {.5f,.2f,1.0f};
		float redColor[] = {1,0,0};
		drawMeshModel(&meshesOne,redColor);
	}
	if(drawDataSetTwo) {
		float blueColor[] = {0,0,1};
		drawMeshModel(&meshesTwo,blueColor);
	}
	if(drawIntersectionPoints)
		drawPoints();
	
	//Draw selection ray
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	//	glVertex3f(selectNear.x, selectNear.y, selectNear.z);
	//	glVertex3f(selectFar.x, selectFar.y, selectFar.z);
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
		glScalef(4, 4, 4);
		gluSphere(quadric, 2, 9, 9);
		glPopMatrix();
	}
    
    //Draw
    glColor3f(1, 1, 1);
    
//    Translate -278.000000 -273.000000 500.000000
    //Draw camera position
    glPushMatrix();
    glTranslatef(278, 273, -500);
//    glScalef(0.01, 0.01, 0.01);
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

//bool RIV3DView::isSelectedObject(ushort objectId) {
//	if(pathFilter) {
//		std::vector<riv::Filter*> filters = pathFilter->GetFilters();
//		for(riv::Filter* f : filters) { //One conjunctive filter at a time
//			riv::ConjunctiveFilter* conjunctiveF = dynamic_cast<riv::ConjunctiveFilter*>(f);
//			if(conjunctiveF) {
//				for(riv::Filter* f2 : conjunctiveF->GetFilters()) {
//					riv::DiscreteFilter* singleF = dynamic_cast<riv::DiscreteFilter*>(f2);
//					if(singleF && singleF->GetAttribute() == "object ID" && almost_equal(singleF->GetValue(), (float)objectId, 0.001)) {
//						return true;
//					}
//				}
//			}
//		}
//	}

//	return false;
//}

void RIV3DView::drawMeshModel(TriangleMeshGroup* meshGroup, float* color) {
	
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
		if(meshindex == selectedMesh) {
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

//Move this function somewhere else
void RIV3DView::generateOctree(size_t maxDepth, size_t maxCapacity, float minNodeSize) {
	
	std::string taskName = "Generating octree";
	reporter::startTask(taskName);
	
	if(heatmap) {
		delete heatmap;
	}
	
	size_t row;
	
	RIVTable<float,ushort>* isectTable = (*datasetOne)->GetTable("intersections");
	TableIterator* iterator = isectTable->GetIterator();
	
	//Generate the index subset
	std::vector<size_t> indices;
	
	while(iterator->GetNext(row)) {
		indices.push_back(row);
	}
	
	RIVFloatRecord* xRecord = isectTable->GetRecord<float>("x");
	std::vector<float>* xValues = xRecord->GetValues();
	RIVFloatRecord* yRecord = isectTable->GetRecord<float>("y");
	std::vector<float>* yValues = yRecord->GetValues();
	RIVFloatRecord* zRecord = isectTable->GetRecord<float>("z");
	std::vector<float>* zValues = zRecord->GetValues();
	
	OctreeConfig config = OctreeConfig(maxDepth, maxCapacity, minNodeSize);
	heatmap = new Octree(xValues, yValues, zValues, indices, config);
	
	printf("Tree generated with \n");
	printf("\tDepth %zu\n",heatmap->Depth());
	printf("\tNodes = %zu\n",heatmap->NumberOfNodes());
	
	//Generate the color map used by the tree
	treeColorMap = colors::jetColorMap();
	
	reporter::stop(taskName);
}

void RIV3DView::ResetGraphics() {
	if(heatmap) {
		delete heatmap;
//		generateOctree(7, 1, .00001F);
	}
	if(drawLightPaths || drawIntersectionPoints) {
		createPaths();
	}
}

//(Re)create the paths objects for the datasets being used
void RIV3DView::createPaths() {
	pathsOne = createPaths(*datasetOne);
	if(datasetTwo) {
		pathsTwo = createPaths(*datasetTwo);
	}
	pathsCreated = true;
}

//Create buffered data for points, not working anymore, colors seem to be red all the time.
std::vector<Path> RIV3DView::createPaths(RIVDataSet<float,ushort>* dataset) {
	
	reporter::startTask("Creating paths");
	
	RIVTable<float,ushort>* isectTable = dataset->GetTable("intersections");
	RIVShortRecord* bounceRecord = isectTable->GetRecord<ushort>("bounce_nr");
	
	colorProperty->SetColorRecords(isectTable->GetRecord<float>(INTERSECTION_R),isectTable->GetRecord<float>(INTERSECTION_G),isectTable->GetRecord<float>(INTERSECTION_B));
	
	std::vector<Path> paths;
	
    //Get the records we want;
    //Get the iterator, this iterator is aware of what rows are filtered and not
    TableIterator* iterator = isectTable->GetIterator();
    
    size_t row = 0;
	size_t *pathID = 0;
	size_t oldPathID = 0;
	ushort bounceNr;
	
	sizesAllTheSame = true;
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
}

void RIV3DView::drawPaths(float startSegment, float stopSegment) {
	if(drawDataSetOne) {
		drawPaths((*datasetOne),pathsOne, startSegment, stopSegment);
	}
	if(datasetTwo && drawDataSetTwo) {
		drawPaths((*datasetTwo),pathsTwo, startSegment, stopSegment);
	}
}

void RIV3DView::drawPaths(RIVDataSet<float,ushort>* dataset, const std::vector<Path>& paths, float startSegment, float stopSegment) {
//	char taskname[100];
//	sprintf(taskname,"drawPaths %f - %f\n",startSegment,stopSegment);
//	reporter::startTask(taskname);
	//See if it should consist of two partial segments
	for(float i = 1 ; i < maxBounce ; i++) {
		if(startSegment < i / maxBounce && stopSegment > i / maxBounce) {
			//                printf("(%f,%f) segment split in (%f,%f) and (%f,%f)\n",startSegment,stopSegment,startSegment,i/maxBounce,i/maxBounce,stopSegment);
			drawPaths(dataset,paths,startSegment, i / maxBounce);
			drawPaths(dataset,paths,i / maxBounce, stopSegment);
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
	drawLightPaths = !drawLightPaths;
	if(drawLightPaths && !pathsCreated) {
		createPaths();
	}
	else if(!drawLightPaths && !drawIntersectionPoints) {
//		paths.clear();
//		pathsCreated = false;
	}
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
}

void RIV3DView::OnFiltersChanged(RIVDataSet<float,ushort>* source) {
    printf("3D View received on filter change.");
	
	ResetGraphics();
	
	int currentWindow = glutGetWindow();
	glutSetWindow(RIV3DView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
	
    isDirty = true;
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
    
//    double distanceEyeToPlane = fabs(eye.z - zNear);
    
    //    printf("Distance near plane to camera = %f\n",distanceEyeToPlane);
//    if(distanceEyeToPlane == 0) {
//        distanceEyeToPlane = 1;
//    }
    //    worldPos.x = x * distanceEyeToPlane;
    //    worldPos.y = y * distanceEyeToPlane
    worldPos[0] = x;
    worldPos[1] = y;
    worldPos[2] = z;
    //    printf("worldPos = [%f,%f,%f]\n",worldPos.x,worldPos.y,worldPos.z);
    
    return worldPos;
}
//Checks if a ray intersects with the mesh group and creates the path filters accordingly and applies them to the dataset
bool RIV3DView::pathCreation(RIVDataSet<float,ushort>* dataset, const TriangleMeshGroup& meshes, riv::RowFilter*& pathFilter, ushort* bounceCount) {
	ushort selectedObjectID;
	float distance;
	
	bool intersects = meshes.Intersect(pickRay, selectedObjectID, Phit, distance);
	bool refilterNeeded = false;
	
	if(intersects && selectRound < maxBounce) {
		printf("new selected object ID = %hu\n",selectedObjectID);
		meshSelected = true;
		selectedMesh = selectedObjectID;
		refilterNeeded = true;
	}
	else {
		selectedMesh = -1;
		selectedObjectID = -1;
		refilterNeeded = false;
	}
	//
	if(refilterNeeded) { //Create path filter
		printf("Path creation filter");
		dataset->StartFiltering();
		
		auto pathTable = dataset->GetTable(PATHS_TABLE);
		auto reference = dynamic_cast<RIVMultiReference*>(pathTable->reference);
		
		auto intersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
		auto primitiveIds = intersectionsTable->GetRecord<ushort>(PRIMITIVE_ID);
		auto bounceNrs = intersectionsTable->GetRecord<ushort>(BOUNCE_NR);
		
		std::map<size_t,bool> filteredRows;
		
//		intersectionsTable->Print();
		
		if(pathFilter != NULL) { //Add to the previous filter
			dataset->ClearRowFilter(pathFilter);
		}
		
		TableIterator* iterator = pathTable->GetIterator();
		size_t row;
		while(iterator->GetNext(row)) {
			const auto& mapping = reference->GetReferenceRows(row);
			ushort nrRows = mapping.second;
			size_t* refRows = mapping.first;
			bool filter = true;
			for(ushort i = 0 ; i < nrRows ; ++i) {
				size_t refRow = refRows[i];
				if(primitiveIds->Value(refRow) == selectedObjectID && bounceNrs->Value(refRow) == *bounceCount) {
					filter = false;
					break;
				}
			}
			
			if(filter) {
				filteredRows[row] = true;
			}
		}
		(*bounceCount)++;
	
		pathFilter = new riv::RowFilter(PATHS_TABLE, filteredRows);
		dataset->AddFilter(pathFilter);
		
		printf("\n");
		dataset->StopFiltering();
	}
	return refilterNeeded;
}

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
    y = height - y;
	if(state == GLUT_DOWN) {
		
		if(button == GLUT_LEFT_BUTTON) {
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
			
			if(!drawDataSetOne && drawDataSetTwo) {
				pathCreation(*datasetTwo, meshesTwo,pathFilterTwo,&bounceCountTwo);
			}
			else if(drawDataSetOne && !drawDataSetTwo) {
				pathCreation(*datasetOne, meshesOne,pathFilterOne, &bounceCountOne);
			}
			else if(drawDataSetOne && drawDataSetTwo) {
				pathCreation(*datasetOne, meshesOne,pathFilterOne, &bounceCountOne);
				pathCreation(*datasetTwo, meshesTwo,pathFilterTwo,&bounceCountTwo);
			}
			//TODO: Else just use the one that intersects closest to the view point
			
			isDragging = true;
			tbMouseFunc(button, state, width-x, y);
			return true;
		}
		else if(button == GLUT_RIGHT_BUTTON) { //Clear paths created
			if(pathFilterOne) {
				bounceCountOne = 1;
				(*datasetOne)->StartFiltering();
				(*datasetOne)->ClearRowFilter(pathFilterOne);
				(*datasetOne)->StopFiltering();
				
				createPaths(*datasetOne);
			}
			if(pathFilterTwo) {
				bounceCountTwo = 1;
				(*datasetTwo)->StartFiltering();
				(*datasetTwo)->ClearRowFilter(pathFilterTwo);
				(*datasetTwo)->StopFiltering();
				
				createPaths(*datasetTwo);
			}
		}
	}
	else {
		isDragging = false;
		return true;
	}
}

bool RIV3DView::HandleMouseMotion(int x, int y) {
//    printf("RIV3DView HandleMouseMotion\n");
//    ToViewSpaceCoordinates(&x, &y);
//	return true;
    y = height - y;
    if(isDragging) {
        tbMotionFunc(width-x, y);
		glutPostRedisplay();
        return true;
    }
    return false;
}

