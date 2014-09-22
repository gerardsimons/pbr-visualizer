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

//void RIV3DView::initialize() {
//	
//}

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
    
    cursorNear.x = 0.F;
    cursorNear.y = 0.F;
    cursorNear.z = 0.F;
    
    cursorFar.x = 0.F;
    cursorFar.y = 0.F;
    cursorFar.z = 1.F;
    
    tbInitTransform();
    tbHelp();
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double)width/height, 1, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void RIV3DView::ToggleDrawClusterMembers() {
    drawClusterMembers = !drawClusterMembers;
    isDirty = true;
}
void RIV3DView::ToggleDrawHeatmap() {
	drawHeatmapTree = !drawHeatmapTree;
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
		size_t depth = node->GetDepth();
		size_t pointsInNode = node->NumberOfPointsContained();
		float density = node->Density();

		//Determine color according to number of children
		size_t maxCap = heatmap->MaxCapacity();
		float ratio = (pointsInNode - 1) / (float)(maxCap - 5);
		
//		float maxDensity = heatmap->MaxDensity();
//		float ratio = density / maxDensity;
//		ratio = 1.F - ratio;
//		float ratio = (pointsInNode) / ((float)heatmap->GetConfiguration()->MaxNodeCapacity());
		const float* color = treeColorMap.Color(ratio);
//		glColor3fv(color);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(color[0], color[1], color[2],.5F);
		
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

void RIV3DView::Draw() {
//    printf("3DView Draw!\n");
	
//	reporter::startTask("3D Draw");
	Vec3Df modelPosition = modelData.GetPosition();
	
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
	
	glPushMatrix();
	glScalef(modelData.GetScale(), modelData.GetScale(), modelData.GetScale());
	glTranslatef(-modelPosition[0], -modelPosition[1], -modelPosition[2]);
    
	drawMeshModel();
	
	if(drawClusterMembers)
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
	
    /* Draw the intersection positions */
    GLUquadric* quadric = gluNewQuadric();
	
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

void RIV3DView::drawMeshModel() {
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    glColor3f(.5f,.2f,1.0f); //Purple
    /** Draw the model **/
    glBegin(GL_TRIANGLES);
    const std::vector<TriangleMesh>& meshes = modelData.GetMeshes();
	size_t meshNumber = 1;
	for(size_t j = 0 ; j < meshes.size() ; ++j) {
//		printf("Drawing Mesh %zu\n",j);
//		++meshNumber;
		TriangleMesh mesh = meshes[j];
		if(meshSelected && j == selectedMeshIndex) {
//			printf("This is the selected mesh\n");
			const float *color = colors::LIGHT_BLUE;
			glColor3f(color[0], color[1], color[2]);
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor3f(.5f,.2f,1.0f); //Purple
		}
		for(size_t i = 0 ; i < mesh.indices.size() ; i += 3) {
			Vec3Df v0 = mesh.GetVertex(mesh.indices[i]);
			Vec3Df v1 = mesh.GetVertex(mesh.indices[i+1]);
			Vec3Df v2 = mesh.GetVertex(mesh.indices[i+2]);
//			printf("glVertex3f(%f,%f,%f)\n",mesh.vertices[v0], mesh.vertices[v1], mesh.vertices[v2]);
			glVertex3f(v0[0],v0[1],v0[2]);
			glVertex3f(v1[0],v1[1],v1[2]);
			glVertex3f(v2[0],v2[1],v2[2]);
		}
//		return;
	}
    glEnd();
}

void RIV3DView::drawPoints() {
//	reporter::startTask("Draw points.");
	
	RIVFloatRecord* xRecord = isectTable->GetRecord<RIVFloatRecord>("intersection X");
	RIVFloatRecord* yRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Y");
	RIVFloatRecord* zRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Z");
	
	//Only use 1 size
	float size = sizeProperty->ComputeSize(isectTable, 0);
	
	if(sizesAllTheSame) {
		glPointSize(size);
//		printf("Sizes are all the same!\n");
	}
	
	if(isectTable != NULL) {

		glBegin(GL_POINTS);
		for(size_t i = 0 ; i < pointsToDraw.size() ; ++i) {
			size_t index = pointsToDraw[i];
//			const float* color = colorProperty->Color(isectTable, index);

//			if(color[0] != pointsR[i] || color[1] != pointsG[i] || color[2] != pointsB[i]) {
//				printf("row = %zu\n",index);
//				printf("Fresh color = ");
//				printArray(color, 3);
//				printf("cached col		or = [%f,%f,%f]",pointsR[i],pointsG[i],pointsB[i]);
//			}
			//			float const* color = colorProperty->Color(isectTable, row); //Check if any color can be computed for the given row
			
			if(!sizesAllTheSame) {
				glPointSize(pointsSize[i]);
			}
			
			float x = xRecord->Value(index);
			float y = yRecord->Value(index);
			float z = zRecord->Value(index);
			
			glPushMatrix();
			glColor3f(pointsR[i],pointsG[i],pointsB[i]);
//			glColor3fv(color); //Very fresh colors
			glVertex3f(x,y,z);
			
		}
		glEnd();
	}
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
	
	RIVTable* isectTable = dataset->GetTable("intersections");
	TableIterator *iterator = isectTable->GetIterator();
	
	//Generate the index subset
	std::vector<size_t> indices;
	
	while(iterator->GetNext(row)) {
		indices.push_back(row);
	}
	
	RIVFloatRecord* xRecord = isectTable->GetRecord<RIVFloatRecord>("intersection X");
	std::vector<float>* xValues = xRecord->GetValuesPointer();
	RIVFloatRecord* yRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Y");
	std::vector<float>* yValues = yRecord->GetValuesPointer();
	RIVFloatRecord* zRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Z");
	std::vector<float>* zValues = zRecord->GetValuesPointer();
	
	OctreeConfig config = OctreeConfig(maxDepth, maxCapacity, minNodeSize);
	heatmap = new Octree(xValues, yValues, zValues, indices, config);
	
	printf("Tree generated with \n");
	printf("\tDepth %zu\n",heatmap->Depth());
	printf("\tNodes = %zu\n",heatmap->NumberOfNodes());
//	printf("\tMemory size = %.3f KB\n",sizeof(heatmap) / 1000.F); //Not sure if that works like this, probably not
	
	//Generate the color map used by the tree
	treeColorMap = colors::jetColorMap();
	
	reporter::stop(taskName);
}

void RIV3DView::InitializeGraphics() {
	createPoints();
	generateOctree(7, 1, .00001F);
}

//Create buffered data for points, not working anymore, colors seem to be red all the time.
void RIV3DView::createPoints() {
    
    //Clear buffer data
    pointsR.clear();
    pointsG.clear();
    pointsB.clear();
	pointsToDraw.clear();
    pointsSize.clear();
	
	std::vector<float> uniqueSizes;
    
    //Get the records we want;
    //Get the iterator, this iterator is aware of what rows are filtered and not
    TableIterator *iterator = isectTable->GetIterator();
    
    size_t row = 0;
	
	sizesAllTheSame = true;
//	RIVUnsignedShortRecord *bounceRecord = isectTable->GetRecord<RIVUnsignedShortRecord>("bounce#");
    
    while(iterator->GetNext(row)) {
        float const* color = colorProperty->Color(isectTable, row); //Check if any color can be computed for the given row
		
//		printf("row = %zu\n",row);
//		printf("bounce# = %d\n",bounceRecord->Value(row));
//		printf("Computed color for point : ");
//        printArray(color, 3);
//		printf("\n");
		
		pointsToDraw.push_back(row);
		
        if(color != NULL) {
			float size = sizeProperty->ComputeSize(isectTable, row);
			if(uniqueSizes.empty()) {
				uniqueSizes.push_back(size);
			}
			
			if(sizesAllTheSame && !vectorContains(uniqueSizes, size)) {
				sizesAllTheSame = false;
			}
			else {
				pointsSize.push_back(sizeProperty->ComputeSize(isectTable, row));
			}
            pointsR.push_back(color[0]);
            pointsG.push_back(color[1]);
            pointsB.push_back(color[2]);
        }
    }
	if(sizesAllTheSame) {
		pointsSize.clear(); //Not necessary
	}
    
    printf("%zu points created.\n",pointsToDraw.size());
	
}

void RIV3DView::MovePathSegment(float ratioIncrement) {
    segmentStart += ratioIncrement;
    segmentStop += ratioIncrement;
    
    float undershoot = 0 - segmentStart;
    if(undershoot > 0) {
        segmentStart += undershoot;
        segmentStop += undershoot;
        
//        else {
//            segmentStart = 1.F - segmentWidth;
//            segmentStop = 1;
//            CyclePathSegment(false);
//        }
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
	//The table we are drawing from
	RIVTable *table = dataset->GetTable("intersections");
	//Get the records we need from the table
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
	
	if(startSegment < 1.F / maxBounce) { //Connect with camera
		glBegin(GL_LINES);
		while(iterator->GetNext(row)) {
			ushort bounceNr = bounceRecord->Value(row);
			if(bounceNr == 1) {
				//Arbitrary camera color
				float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row
				if(color) {
					glColor3f(1, 1, 1); //White
					float deltaX = xRecord->Value(row) - cameraPosition[0];
					float deltaY = yRecord->Value(row) - cameraPosition[1];
					float deltaZ = zRecord->Value(row) - cameraPosition[2];
					glVertex3f(cameraPosition[0] + deltaX * startSegment * maxBounce,cameraPosition[1] + deltaY * startSegment * maxBounce,cameraPosition[2] + deltaZ * startSegment * maxBounce);
					glColor3fv(color);
					glVertex3f(cameraPosition[0] + deltaX * stopSegment * maxBounce,cameraPosition[1] + deltaY * stopSegment * maxBounce,cameraPosition[2] + deltaZ * stopSegment * maxBounce);
				}
			}
		}
		glEnd();
		return;
	}
	
	glBegin(GL_LINES);
	while(iterator->GetNext(row)) {
		int bounce = floor(startSegment * maxBounce);
		float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row
		if(color != NULL) {
			ushort bounceNr = bounceRecord->Value(row);
			if(!pathStartFound && bounceNr == bounce ) {
				//                printf("Row %zu is valid start point for path\n",row);;
				pathStartFound = true;
			}
			else if(pathStartFound && row == lastRow + 1 && bounceNr == bounce + 1) { //Valid path found, draw
				//                printf("Row %zu is valid end point for path\n",row);
				//                printf("Last color = ");
				//                printArray(lastColor, 3);
				
//                    printf("lastBounceNr = %d\n",lastBounceNr);
//                    printf("bounceNr = %d\n",bounceNr]);
				
				float lastX = xRecord->Value(lastRow);
				float lastY = yRecord->Value(lastRow);
				float lastZ = zRecord->Value(lastRow);
				
				float deltaX = xRecord->Value(row) - lastX;
				float deltaY = yRecord->Value(row) - lastY;
				float deltaZ = zRecord->Value(row) - lastZ;
				
				float C_one = linearInterpolate(startSegment * maxBounce, lastBounceNr, bounceNr);
				float C_two = linearInterpolate(stopSegment * maxBounce, lastBounceNr, bounceNr);

				//Sanity check
//                    if(C_one < 0 || C_one > 1 || C_two < 0 || C_two > 1) {
//                        
//                    }
				glColor3fv(lastColor);
				glVertex3f(lastX + deltaX * C_one, lastY + deltaY * C_one, lastZ + deltaZ * C_one);
				glColor3fv(color);
				glVertex3f(lastX + deltaX * C_two, lastY + deltaY * C_two, lastZ + deltaZ * C_two);
				

//                    glVertex3f(lastX + deltaX * segmentStart,lastY + deltaY * segmentStart,lastZ + deltaZ * segmentStart);
//                    glColor3fv(color);
//                    glVertex3f(lastX + deltaX * segmentStop,lastY + deltaY * segmentStop,lastZ + deltaZ * segmentStop);
				
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
void RIV3DView::ToggleDrawPaths() {
	drawLightPaths = !drawLightPaths;
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
//    printf("isDragging = %d\n",isDragging);
	selectNear = screenToWorldCoordinates(x, y, 0);
	selectFar = screenToWorldCoordinates(x, y, 1);
	
	std::cout << "selectNear = " << selectNear << std::endl;
	std::cout << "selectFar = " << selectFar << std::endl;
	
//	Vec3Df origin = Vec3Df(modelData.GetScale() * selectNear.x,modelData.GetScale() * selectNear.y,selectNear.z);
//	Vec3Df dest = modelData.GetScale() * Vec3Df(selectFar.x,selectFar.y,selectFar.z);
//	Vec3Df origin = Vec3Df(selectNear.x,selectNear.y,selectNear.z);
//	Vec3Df dest = Vec3Df(selectFar.x,selectFar.y,selectFar.z);
	
	Vec3Df modelPosition = -modelData.GetPosition();
//	modelPosition = Vec3Df(0,0,0);
	float modelScale = 1/modelData.GetScale();
	Vec3Df dest = modelScale * Vec3Df(selectNear.x,selectNear.y,selectNear.z);
	dest -= modelPosition;
	Vec3Df origin = modelScale * Vec3Df(selectFar.x,selectFar.y,selectFar.z);
	origin -= modelPosition;
	
//	std::cout << "Ray origin = " << origin << std::endl;
//	std::cout << "Ray destination = " << dest << std::endl;
//	origin = Vec3Df(.5F,.5F,2.F);
//	dest = Vec3Df(.5F,.5F,-2.F);
	
	Vec3Df dir = dest - origin;
//	dir.normalize();
	
	pickRay = Ray<float>(origin,dir);
	if(modelData.TriangleIntersect(pickRay, selectedMeshIndex, Phit)) {
		printf("Result = %zu\n",selectedMeshIndex);
		meshSelected = true;
		
	}
	else {
		meshSelected = false;
		printf("Intersect failed.\n");
	}
	
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

