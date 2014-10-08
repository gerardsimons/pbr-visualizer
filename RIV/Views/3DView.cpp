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


#if __APPLE__
    #include <GLUT/GLUT.h>
#else
	#include <GL/glut.h>
#endif

//Init instance to draw
RIV3DView* RIV3DView::instance = NULL;
int RIV3DView::windowHandle = -1;

RIV3DView::RIV3DView(DataController* dataController, PBRTConfig* config, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty)
	: RIVDataView(dataController,x,y,width,height,paddingX,paddingY,colorProperty,sizeProperty) {
	this->pbrtConfig = config;
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    instance = this;
    identifier = "3DView";
	
	createPaths();
};

RIV3DView::RIV3DView(DataController* dataController,PBRTConfig* config, RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(dataController,colorProperty,sizeProperty) {
	pbrtConfig = config;
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    instance = this;
    identifier = "3DView";
	
	createPaths();
};

void RIV3DView::CyclePathSegment(bool direction) {
    float delta = 1.F / maxBounce;
    direction ? MovePathSegment(delta) : MovePathSegment(-delta);
}

void RIV3DView::Reshape(int newWidth, int newHeight) {
    width = newWidth;
    this->height = newHeight;
    
    eye.x = 0;
    eye.y = 0;
    eye.z = 2;
    
//    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
    
    tbInitTransform();
    tbHelp();
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double)width/height, 1, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void RIV3DView::ToggleDrawIntersectionPoints() {
    drawIntersectionPoints = !drawIntersectionPoints;
	printf("drawIntersectionPoints is now ");
	if(drawIntersectionPoints) printf("ON\n"); else printf("OFF\n");
    isDirty = true;
}
void RIV3DView::ToggleDrawHeatmap() {
	drawHeatmapTree = !drawHeatmapTree;
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
		Color cubeColor = treeColorMap.ComputeColor(ratio);
//		glColor3fv(color);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(cubeColor.R,cubeColor.G,cubeColor.B,.5F);
		
//		if(almost_equal(ratio,0.05,.1) || almost_equal(ratio,0,.001)) {
		if(pointsInNode > 14) {
//			printf("Points in node = %zu\n",pointsInNode);
////			printf("Node density = %f\n",density);
////			printf("Max density = %f\n",maxDensity);
//			printf("MAX_NODE_CAPACITY = %zu\n",maxCap);
//			printf("Ratio = %f\n",ratio);
//			printf("Color = ");
//			printArray(color, 3);
//			printf("\n");
		}
		
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

void RIV3DView::Draw() {
//    printf("3DView Draw #%zu\n",++drawCounter_);
	
//	reporter::startTask("3D Draw");
//	MeshModelGroup = pbrtConfig->GetModels();
	MeshModelGroup* objects = pbrtConfig->GetMeshModelGroup();
	Vec3Df modelPosition = objects->GetCenter();
	
    glEnable(GL_DEPTH_TEST);
//    glClearColor (0.0, 0.0, 0.0, 0.0);
	glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
    
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
	Vec3Df scale = objects->GetScale();
	glScalef(scale[0],scale[1],scale[2]);
	glTranslatef(-modelPosition[0], -modelPosition[1], -modelPosition[2]);
    
	drawMeshModel();
	
	if(drawIntersectionPoints)
		drawPoints();
	
	//Draw selection ray
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	//	glVertex3f(selectNear.x, selectNear.y, selectNear.z);
	//	glVertex3f(selectFar.x, selectFar.y, selectFar.z);
	glVertex3f(pickRay.orig[0], pickRay.orig[1], pickRay.orig[2]);
	Vec3Df dest = pickRay.orig + 1.F * pickRay.dir;
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
	
//	reporter::stop("3D Draw");
}

bool RIV3DView::isSelectedObject(ushort objectId) {
	for(ushort oid : selectedObjectIDs) {
		if(oid == objectId)
			return true;
	}
	return false;
}

void RIV3DView::drawMeshModel() {
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    glColor3f(.5f,.2f,1.0f); //Purple
    /** Draw the model **/
    glBegin(GL_TRIANGLES);
	std::vector<MeshModel>* objects = pbrtConfig->GetMeshModelGroup()->GetModels();
//	size_t meshNumber = 1;
	for(size_t i = 0 ; i < objects->size() ; ++i) {
		MeshModel model = objects->at(i);
		if(meshSelected && isSelectedObject(model.GetObjectID())) {
			//			printf("This is the selected mesh\n");
			Color lightBluecolor = colors::LIGHT_BLUE;
			glColor3f(lightBluecolor.R,lightBluecolor.G,lightBluecolor.B);
			//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor3f(.5f,.2f,1.0f); //Purple
		}
		const std::vector<riv::TriangleMesh>& meshes = model.GetMeshes();
		for(size_t j = 0 ; j < meshes.size() ; ++j) {
//		printf("Drawing Mesh %zu\n",j);
			riv::TriangleMesh mesh = meshes[j];

			for(size_t i = 0 ; i < mesh.indices.size() ; i += 3) {
				Vec3Df v0 = mesh.GetVertex(mesh.indices[i]);
				Vec3Df v1 = mesh.GetVertex(mesh.indices[i+1]);
				Vec3Df v2 = mesh.GetVertex(mesh.indices[i+2]);
//				printf("glVertex3f(%f,%f,%f)\n",mesh.vertices[v0], mesh.vertices[v1], mesh.vertices[v2]);
				glVertex3f(v0[0],v0[1],v0[2]);
				glVertex3f(v1[0],v1[1],v1[2]);
				glVertex3f(v2[0],v2[1],v2[2]);
			}
	//		return;
		}
	}
    glEnd();
}

void RIV3DView::drawPoints() {
//	reporter::startTask("Draw points.");
//	printf("Drawing intersections points.\n");
	
//	RIVTable* isectTable = dataset->GetTable("intersections");
	std::vector<std::string> coordinateColumns = {"INTERSECTION_X","INTERSECTION_X","INTERSECTION_Z"};
//	sqlite3_stmt* stmt = isectsTable->SelectStmt(coordinateColumns);
	sqlite3_stmt* stmt = dataController->GetIsectsView()->SelectStmt(coordinateColumns);
	
	//Only use 1 size
//	float size = sizeProperty->ComputeSize(isectTable, 0);
//	printf("Point size = %f\n",size);
	
	if(sizesAllTheSame) {
		glPointSize(1);
	}
	
	glBegin(GL_POINTS);
	for(Path& path : paths) {
		for(size_t i = 0 ; i < path.Size() ; ++i) {
			
			size_t index = path.GetPoint(i);
			Color pointColor = path.GetColor(i);
			
			sqlite3_step(stmt);
			float x = sqlite3_column_double(stmt, 1);
			float y = sqlite3_column_double(stmt, 2);
			float z = sqlite3_column_double(stmt, 3);
			
			glPushMatrix();
			glColor3f(pointColor.R,pointColor.G,pointColor.B);
			glVertex3f(x,y,z);
		}
	}
	glEnd();
	
//	reporter::stop("Draw points.");
}

//Move this function somewhere else
void RIV3DView::generateOctree(size_t maxDepth, size_t maxCapacity, float minNodeSize) {
	
//	std::string taskName = "Generating octree";
//	reporter::startTask(taskName);
//	
//	if(heatmap) {
//		delete heatmap;
//	}
//	
//	size_t row;
//	
//	RIVTable* isectTable = dataset->GetTable("intersections");
//	TableIterator *iterator = isectTable->GetIterator();
//	
//	//Generate the index subset
//	std::vector<size_t> indices;
//	
//	while(iterator->GetNext(row)) {
//		indices.push_back(row);
//	}
//	
//	RIVFloatRecord* xRecord = isectTable->GetRecord<RIVFloatRecord>("intersection X");
//	std::vector<float>* xValues = xRecord->GetValuesPointer();
//	RIVFloatRecord* yRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Y");
//	std::vector<float>* yValues = yRecord->GetValuesPointer();
//	RIVFloatRecord* zRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Z");
//	std::vector<float>* zValues = zRecord->GetValuesPointer();
//	
//	OctreeConfig config = OctreeConfig(maxDepth, maxCapacity, minNodeSize);
//	heatmap = new Octree(xValues, yValues, zValues, indices, config);
//	
//	printf("Tree generated with \n");
//	printf("\tDepth %zu\n",heatmap->Depth());
//	printf("\tNodes = %zu\n",heatmap->NumberOfNodes());
////	printf("\tMemory size = %.3f KB\n",sizeof(heatmap) / 1000.F); //Not sure if that works like this, probably not
//	
//	//Generate the color map used by the tree
//	treeColorMap = colors::jetColorMap();
//	
//	reporter::stop(taskName);
}

void RIV3DView::ResetGraphics() {
	createPaths();
//	generateOctree(7, 1, .00001F);
}

//Create buffered data for points, not working anymore, colors seem to be red all the time.
void RIV3DView::createPaths() {
	
//	reporter::startTask("Creating paths");
//	
////	RIVTable* isectTable = dataset->GetTable("intersections");
//	
//	sqlite3_stmt* isects = dataController->GetIntersectionsStmt();
//	paths.clear();
//    
//    //Get the records we want;
//    //Get the iterator, this iterator is aware of what rows are filtered and not
////    TableIterator* iterator = isectTable->GetIterator();
//	
//    size_t row = 0;
//	size_t *pathID = 0;
//	size_t oldPathID = 0;
//	
//	sizesAllTheSame = true;
//	std::vector<size_t> vertices;
//	std::vector<Color> colors;
//	
//    while(sqlite3_step(isects) == SQLITE_ROW) {
//		if(*pathID != oldPathID && colors.size() > 0) {
//			paths.push_back(Path(&vertices[0],&colors[0],colors.size()));
//			vertices.clear();
//			colors.clear();
//			oldPathID = *pathID;
//		}
//		Color pointColor;
////        bool hasColor = colorProperty->ComputeColor(isectTable, row, pointColor); //Check if any color can be computed for the given row
//		
//		vertices.push_back(row);
//		colors.push_back(pointColor);
//    }
//
//	reporter::stop("Creating paths");
//	reportVectorStatistics("paths", paths);
//	
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
	//    printf("drawPaths(%f,%f)\n",startSegment,stopSegment);
	//See if it should consist of two partial segments
	for(float i = 1 ; i < maxBounce ; i++) {
		if(startSegment < i / maxBounce && stopSegment > i / maxBounce) {
			//                printf("(%f,%f) segment split in (%f,%f) and (%f,%f)\n",startSegment,stopSegment,startSegment,i/maxBounce,i/maxBounce,stopSegment);
			drawPaths(startSegment, i / maxBounce);
			drawPaths(i / maxBounce, stopSegment);
			return;
		}
	}
	char taskname[100];
//	sprintf(taskname,"drawPaths %f - %f\n",startSegment,stopSegment);
	reporter::startTask(taskname);
	
	//Start and end vertex index
	int startBounce = floor(startSegment * maxBounce);
	int endBounce = startBounce + 1;
	

	

	
//	printf("start,end bounce = %d,%d\n",startBounce,endBounce);
	
	glBegin(GL_LINES);
	if(startBounce == 0) {
		std::vector<std::string> coordinateColumns = {"BOUNCE_NR","INTERSECTION_X","INTERSECTION_X","INTERSECTION_Z"};
		sqlite3_stmt* stmt = dataController->GetIsectsView()->SelectStmt(coordinateColumns);
		//Get the points with bounce = 1
//		for(const Path& path : paths) {
//			size_t point = path.GetPoint(0);
//			Color c = path.GetColor(0);
		
		while(sqlite3_step(stmt)) {
				int bounceNr = sqlite3_column_int(stmt, 1);
				if(bounceNr == 1) {
				float x = sqlite3_column_double(stmt, 2);
				float y = sqlite3_column_double(stmt, 3);
				float z = sqlite3_column_double(stmt, 4);

				float deltaX = x - cameraPosition[0];
				float deltaY = y - cameraPosition[1];
				float deltaZ = z - cameraPosition[2];
				
				glVertex3f(cameraPosition[0] + deltaX * startSegment * maxBounce,cameraPosition[1] + deltaY * startSegment * maxBounce,cameraPosition[2] + deltaZ * startSegment * maxBounce);
	//			glColor3f(c.R,c.G,c.B);
				glVertex3f(cameraPosition[0] + deltaX * stopSegment * maxBounce,cameraPosition[1] + deltaY * stopSegment * maxBounce,cameraPosition[2] + deltaZ * stopSegment * maxBounce);
			}
		}
	}
	else {
		//Get the points with bounce = 1 and bounce = 2 of the same path
//		for(const Path& path : paths) {
//			if(path.Size() >= endBounce) {
//				size_t startPoint = path.GetPoint(startBounce - 1);
//				Color startColor = path.GetColor(startBounce - 1);
//				
//				size_t endPoint = path.GetPoint(endBounce - 1);
//				Color endColor = path.GetColor(endBounce - 1);
//				
//				float Cstart = startSegment * maxBounce - startBounce;
//				float Cend = stopSegment * maxBounce - startBounce;
//
//				sqlite3_step(stmt);
//				float x = sqlite3_column_double(stmt, 1);
//				float y = sqlite3_column_double(stmt, 2);
//				float z = sqlite3_column_double(stmt, 3);
//
//				float endX = xRecord->Value(endPoint);
//				float endY = yRecord->Value(endPoint);
//				float endZ = zRecord->Value(endPoint);
//
//				float deltaX = endX - startX;
//				float deltaY = endY - startY;
//				float deltaZ = endZ - startZ;
//
//				glColor3f(startColor.R,startColor.G,startColor.B);
//				glVertex3f(startX + deltaX * Cstart, startY + deltaY * Cstart, startZ + deltaZ * Cstart);
//				glColor3f(endColor.R,endColor.G,endColor.B);
//				glVertex3f(startX + deltaX * Cend, startY + deltaY * Cend, startZ + deltaZ * Cend);
//			}
//		}
		std::vector<std::string> coordinateColumns = {"PID","BOUNCE_NR","INTERSECTION_X","INTERSECTION_X","INTERSECTION_Z"};
		int pid = -1;
		int previousPid = -1;
		sqlite3_stmt* stmt = dataController->GetIsectsView()->SelectStmt();
		float startX,startY,startZ;
		while(sqlite3_step(stmt)) {
			int bounceNr = sqlite3_column_int(stmt, 1);
			int pid = sqlite3_column_int(stmt, 2);
			if(bounceNr == endBounce && pid == previousPid) {

				
				float endX = sqlite3_column_double(stmt, 1);
				float endY = sqlite3_column_double(stmt, 2);
				float endZ = sqlite3_column_double(stmt, 3);
				
				float Cstart = startSegment * maxBounce - startBounce;
				float Cend = stopSegment * maxBounce - startBounce;

				float deltaX = endX - startX;
				float deltaY = endY - startY;
				float deltaZ = endZ - startZ;

//				glColor3f(startColor.R,startColor.G,startColor.B);
				glVertex3f(startX + deltaX * Cstart, startY + deltaY * Cstart, startZ + deltaZ * Cstart);
//				glColor3f(endColor.R,endColor.G,endColor.B);
				glVertex3f(startX + deltaX * Cend, startY + deltaY * Cend, startZ + deltaZ * Cend);
			}
			else if(bounceNr == startBounce) {
				
				float startX = sqlite3_column_double(stmt, 1);
				float startY = sqlite3_column_double(stmt, 2);
				float startZ = sqlite3_column_double(stmt, 3);

			}
			previousPid = pid;
		}
	}
	glEnd();
	
	//	printf("%zu path lines drawn\n",linesDrawn);
	reporter::stop(taskname);
}

void RIV3DView::ToggleDrawPaths() {
	drawLightPaths = !drawLightPaths;
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

void RIV3DView::OnDataSetChanged() {
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

Vec3Df RIV3DView::screenToWorldCoordinates(int screenX, int screenY, float zPlane) {
    
    Vec3Df worldPos;
    
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

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {

    y = height - y;



	if(state == GLUT_DOWN) {
		
		Vec3Df selectNear = screenToWorldCoordinates(x, y, 0);
		Vec3Df selectFar = screenToWorldCoordinates(x, y, 1);
		
		Vec3Df modelPosition = -pbrtConfig->GetMeshModelGroup()->GetCenter();
		Vec3Df modelScale = pbrtConfig->GetMeshModelGroup()->GetScale();
		
		modelScale[0] = 1 / modelScale[0];
		modelScale[1] = 1 / modelScale[1];
		modelScale[2] = 1 / modelScale[2];
		
		Vec3Df dest = modelScale * selectNear;
		dest -= modelPosition;
		Vec3Df origin = modelScale * selectFar;
		origin -= modelPosition;
		
		Vec3Df dir = dest - origin;
		
		ushort selectedObjectID;
		
		pickRay = riv::Ray<float>(origin,dir);
		bool intersects = pbrtConfig->GetMeshModelGroup()->ModelIntersect(pickRay, selectedObjectID, Phit);
		
		printf("selected Object IDs = ");
		printVector(selectedObjectIDs);
		printf("new Object ID = %hu\n",selectedObjectID);
		
		bool refilterNeeded = false;

		if(intersects && selectRound < maxBounce) {
			printf("new selected object ID = %hu\n",selectedObjectID);
			meshSelected = true;
			selectedObjectIDs.push_back(selectedObjectID);
			refilterNeeded = true;
		}
//		else if(!newObjectID) {
//			refilterNeeded = true;
//			selectedObjectIDs.erase(selectedObjectIDs.begin() + objIdIndex);
//		}
		
		if(refilterNeeded) { //Create path filter
			printf("Path creation filter");
			if(pathCreationFilterHandle) {
//				dataset->ClearFilter(pathCreationFilterHandle);
			}
			std::vector<riv::Filter*> allFilters;
			for(size_t i = 0 ; i < selectedObjectIDs.size() ; ++i) {
				riv::Filter* objectFilter = new riv::DiscreteFilter("object ID",selectedObjectIDs[i]);
				riv::Filter* bounceFilter = new riv::DiscreteFilter("bounce#",i+1);
				std::vector<riv::Filter*> fs;
				fs.push_back(objectFilter);
				fs.push_back(bounceFilter);
				allFilters.push_back(new riv::ConjunctiveFilter(fs));
			}
//			riv::GroupFilter* pathCreationFilter = new riv::GroupFilter(allFilters,dataset->GetTable("path"));
//			pathCreationFilter->Print();
//			printf("\n");
//			dataset->StartFiltering();
//			dataset->AddFilter("path", pathCreationFilter);
//			dataset->StopFiltering();
		}
		
		isDragging = true;
		tbMouseFunc(button, state, width-x, y);
		return true;
	}
    isDragging = false;
    return false;
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

