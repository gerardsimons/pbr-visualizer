//
//  3DView.h
//  Afstuderen
//
//  Created by Gerard Simons on 19/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen___DView__
#define __Afstuderen___DView__

#include "DataView.h"

#include "../PBRTConfig.h"
#include "../Geometry/Path.h"
#include "../Geometry/MeshModel.h"
#include "../Octree/Octree.h"
#include "../Geometry/TriangleMeshGroup.h"

#include "devices/device_singleray/embree_renderer.h"

#include <limits>

class RIV3DView : public RIVDataView, public RIVDataSetListener {
protected:

    Vector3f cameraPositionOne;
    Vector3f cameraPositionTwo;
    Vector3f eye;
    
    bool isDirty = true;
	
	float modelScale;
	Vec3fa modelCenter;
	
    bool drawIntersectionPoints = false;
	
	//Whether the generated octree should be drawn (if any is generated)
	bool drawHeatmapTree = false;
	bool drawLightPaths = false;
    bool showMeshes = true;
//	size_t selectedMesh = -1;
	
	//Mesh model data
	TriangleMeshGroup meshesOne;
	TriangleMeshGroup meshesTwo;
	
	bool drawDataSetOne = true;
	bool drawDataSetTwo = true;
	
	bool isDragging = false;
    bool didMoveCamera = false;
	
	std::vector<Path> pathsOne;
	std::vector<Path> pathsTwo;
    
    Octree* energyDistributionOne;
    Octree* energyDistributionTwo = NULL;
	
	EMBREERenderer* rendererOne;
	EMBREERenderer* rendererTwo = NULL;
	
	RIVColorProperty* colorPropertyOne;
	RIVColorProperty* colorPropertyTwo;
	
	//Octree generated from 3D points (generated in createPoints)
	riv::ColorMap treeColorMap;
    
    //Path drawing variables
    int maxBounce = 5; //TODO: Deduce this value from the bounce record
	int selectRound = 1;
    const float segmentWidth = .025F;
    float segmentStart = 0;
    float segmentStop = segmentWidth;
	
	GLUquadric* quadric = gluNewQuadric();
	Vec3fa Phit; //Supposedly the point of intersection of the ray with the plane supporting the triangle
	Ray pickRay;
	
	bool meshSelected = false;
    std::vector<riv::RowFilter*> pathFiltersOne;
	ushort bounceCountOne = 0;
    ushort selectedObjectIdOne;
    std::vector<riv::RowFilter*> pathFiltersTwo;
	ushort bounceCountTwo = 0;
    ushort selectedObjectIdTwo;
	
    bool pathsCreated = false;

	//Generate a octree from the unfiltered intersection points
	void generateOctree(size_t maxDepth, size_t maxCapacity, float minNodeSize);
	//Determines if the objectId is currently selected
	bool isSelectedObject(ushort objectId);
	//Draw the mesh model loaded from the PBRT file
	void drawMeshModel(TriangleMeshGroup* meshGroup, float* color, ushort* selectedObjectId);
	void drawPaths(float startSegment, float stopSegment);
    void drawPaths(RIVDataSet<float,ushort>* dataset, const std::vector<Path>& paths, float startSegment, float stopSegment,const Vector3f& cameraPosition); //Draw the paths between two consecutive bounces
	//Draw octree representation of the generated points
	void drawHeatmap();
	void drawPoints();
	//Draw the intersection points
	void drawPoints(RIVDataSet<float,ushort>* dataset, const std::vector<Path>& paths);
	//draw the leaf nodes starting from the given node
	void drawLeafNodes(OctreeNode* node,float maxEnergyOne, float maxEnergyTwo);
	void createPaths();
	
    void filterPaths(RIVDataSet<float,ushort>* dataset, ushort bounceNr, ushort selectedObjectID, std::vector<riv::RowFilter*>& pathFilters);
	bool pathCreation(RIVDataSet<float,ushort>* dataset, const TriangleMeshGroup& meshes,std::vector<riv::RowFilter*>& pathFilters, ushort* bounceCount, ushort* selectedObjectId);
	//Create graphics buffer from unfiltered data rows
	std::vector<Path> createPaths(RIVDataSet<float,ushort>*, RIVColorProperty* colorProperty);
    static RIV3DView* instance;
    Vec3fa screenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
    void redisplayWindow();
public:
	//Single renderer constructor
    RIV3DView(RIVDataSet<float,ushort>** dataset,EMBREERenderer* renderer,const TriangleMeshGroup& sceneDataOne, Octree* energyDistribution, RIVColorProperty* colorProperty);
	//Dual renderer constructor
	RIV3DView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo,EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo, const TriangleMeshGroup& sceneDataOne, const TriangleMeshGroup& sceneDataTwo, Octree* energyDistributionOne, Octree* energyDistributionTwo, RIVColorProperty* colorPropertyOne,RIVColorProperty* colorPropertyTwo);
	
	//Extract data about the scene from the embree renderer object
	void GetSceneData(EMBREERenderer* renderer, TriangleMeshGroup* target);
	
	static int windowHandle;
    void DrawEnergyDistribution(Octree* energyDistribution,Vec3fa& color);
    
    void ToggleHideMesh();
    void Reshape(int newWidth, int newHeight);
    void Draw();
    bool HandleMouse(int button, int state, int x, int y);
    bool HandleMouseMotion(int x, int y);
    
    void SetHeatmapDepth(int depth);
    void IncrementHeatmapDepth();
    void DecrementHeatmapDepth();
	
    void OnDataChanged(RIVDataSet<float,ushort>* source);
	void OnFiltersChanged(RIVDataSet<float,ushort>* source);
	
	void ResetGraphics();
	
    static void DrawInstance(); //Override
    static void ReshapeInstance(int,int);
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    
    void MovePathSegment(float ratioIncrement);
    void CyclePathSegment(bool direction = true); //Cycle the path segment to draw, direction bool indicates direction of cycling, positive meaning incrementing
    void ToggleDrawIntersectionPoints();
	void ToggleDrawHeatmap();
	void ToggleDrawPaths();
	void ToggleDrawDataSetOne();
	void ToggleDrawDataSetTwo();
    void SetModelData(const MeshModel&);
    void MoveCamera(float,float,float);
	
};

#endif /* defined(__Afstuderen___DView__) */
