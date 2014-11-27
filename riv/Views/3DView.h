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
protected:;

    float cameraPosition[3] = {278, 273, -500}; //The original
    Point3D eye;
	
    //Whether to draw the members of clusters, or only the cluster medoid
    bool drawIntersectionPoints = false;
	
	//Whether the generated octree should be drawn (if any is generated)
	bool drawHeatmapTree = false;
	bool drawLightPaths = false;
	
	//Mesh model data
	TriangleMeshGroup meshes;
	
	EMBREERenderer* rendererOne = NULL;
	
	//Octree generated from 3D points (generated in createPoints)
	Octree* heatmap = NULL;
	riv::ColorMap treeColorMap;
    
    //Path drawing variables
    int maxBounce = 5; //TODO: Deduce this value from the bounce record
	int selectRound = 1;
    
    const float segmentWidth = .05F; // a tenth of the total path length
    float segmentStart = 0;
    float segmentStop = segmentWidth;
	
	GLUquadric* quadric = gluNewQuadric();
	Vec3fa Phit; //Supposedly the point of intersection of the ray with the plane supporting the triangle
	Ray pickRay;
	
	bool meshSelected = false;
	riv::GroupFilter* pathFilter = NULL;
    
    //Buffered graphics point data, generated from the data, stored here for speed, TODO: Only store indices and a pointer to these records?
	bool sizesAllTheSame; //Because sizes are often set to the same, we take advantage of this to get a big performance boost
	//Indices of the points to draw
	std::vector<Path> paths;
	bool pathsCreated = false;
	
	static std::vector<RIV3DView*> instances;

	//Generate a octree from the unfiltered intersection points
	void generateOctree(size_t maxDepth, size_t maxCapacity, float minNodeSize);
	//Determines if the objectId is currently selected
	bool isSelectedObject(ushort objectId);
	//Draw the mesh model loaded from the PBRT file
	void drawMeshModel();
    void drawPaths(float startSegment, float stopSegment); //Draw the paths between two consecutive bounces
	//Draw octree representation of the generated points
	void drawHeatmap();
	//Draw the intersection points
	void drawPoints();
	//draw the leaf nodes starting from the given node
	void drawLeafNodes(OctreeNode* node);
	//Create graphics buffer from unfiltered data rows
	void createPaths();
    Vec3fa screenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
public:
    RIV3DView(RIVDataSet* dataset,EMBREERenderer* rendererOne,int,int,int,int,int,int,RIVColorProperty*,RIVSizeProperty*);
    RIV3DView(RIVDataSet* dataset,EMBREERenderer* rendererOne,RIVColorProperty*,RIVSizeProperty*);
	
	//Extract data about the scene from the embree renderer object
	void GetSceneData();
	
	static int windowHandle;
	
    void Reshape(int newWidth, int newHeight);
    void Draw();
    bool HandleMouse(int button, int state, int x, int y);
    bool HandleMouseMotion(int x, int y);
	
    void OnDataChanged();
	void OnFiltersChanged();
	
	void ResetGraphics();
	
    static void DrawInstances(); //Override
    static void ReshapeInstances(int newWidth,int newHeight);
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    
    void MovePathSegment(float ratioIncrement);
    void CyclePathSegment(bool direction = true); //Cycle the path segment to draw, direction bool indicates direction of cycling, positive meaning incrementing
    void ToggleDrawIntersectionPoints();
	void ToggleDrawHeatmap();
	void ToggleDrawPaths();
    void SetModelData(const MeshModel&);
    void MoveCamera(float,float,float);
    
    //The models to draw

    //The camera
    
    //Anything else?
};

#endif /* defined(__Afstuderen___DView__) */
