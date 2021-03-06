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

#include "../Gizmo.h"
#include "../PBRTConfig.h"
#include "../Geometry/Path.h"
#include "../Geometry/MeshModel.h"
#include "../Octree/Octree.h"
#include "../Geometry/TriangleMeshGroup.h"

#include "devices/device_singleray/embree_renderer.h"

#include <limits>
#include <set>

class RIV3DView : public RIVDataView, public RIVDataSetListener {
private:
    
    static RIV3DView* instance;
    
    //When path selection mode is used, each clicked object determines the object the next bounce should interact with,
    //when regular object mode is used, the order of interaction is not important

    enum DrawPathsMode {
        CAMERA,
        LIGHTS,
        NONE
    };
    
    DrawPathsMode pathsMode = CAMERA;
    
    class LightCone {
    public:
        size_t originN = 0; //Number of points in start of cone
        Vec3fa origin;
        Vec3fa target;
        size_t targetN = 0; //Number of points cone pointing to
        float r,g,b; //Color
        float size; //
        float length;
    };
    std::map<size_t,LightCone*> lightConesOne;
    std::map<size_t,LightCone*> lightConesTwo;
    
    riv::Color backgroundColor = colors::BLACK;
    
    Vector3f cameraPositionOne;
    Vector3f cameraPositionTwo;
    Vector3f eye;
    
    int lastX = 0;
    TriangleMeshGroup* activeShape = NULL;
    Vec3fa shapeTranslation;
    float translationSpeed = 0.1F;
    
    bool showGizmos = true;
    Gizmo* activeGizmo = NULL;
    Gizmo* gizmoOne = NULL;
    Gizmo* gizmoTwo = NULL;
    std::vector<riv::RowFilter*> gizmoFiltersOne;
    std::vector<riv::RowFilter*> gizmoFiltersTwo;
    
    bool isDirty = true;
    
    float modelScale;
    Vec3fa modelCenter;
    
    bool drawIntersectionPoints = false;
    
    //Whether the generated octree should be drawn (if any is generated)
    bool drawHeatmapTree = false;
    
    enum MeshDisplayMode {
        NO_MESH,
        FIXED_COLOR,
        MEMBERSHIP_COLOR
    };
    
    MeshDisplayMode meshDisplay = MEMBERSHIP_COLOR;
    
    //	size_t selectedMesh = -1;
    
    //Mesh model data
    TriangleMeshGroup meshesOne;
    TriangleMeshGroup meshesTwo;
    
    bool drawDataSetOne = true;
    bool drawDataSetTwo = true;
    
    bool isDragging = false;
    bool didMoveCamera = false;
    
    std::vector<Path> cameraPathsOne;
    std::vector<Path> cameraPathsTwo;
    
    std::vector<Path> lightPathsOne;
    std::vector<Path> lightPathsTwo;
    
    std::vector<Ref<Light>> lightsOne;
    std::vector<Ref<Light>> lightsTwo;
    
    ushort drawHeatmapDepth;
    Octree* energyDistributionOne;
    Octree* energyDistributionTwo = NULL;
    
    EMBREERenderer* rendererOne;
    EMBREERenderer* rendererTwo = NULL;
    
    RIVColorProperty* pathColorOne;
    RIVColorProperty* rayColorOne;
    RIVColorProperty* pathColorTwo;
    RIVColorProperty* rayColorTwo;
    
    //Path drawing variables
    int depthOne;
    int depthTwo;
    int maxDepth;
    int selectRound = 1;
    const float segmentWidth = .075F;
    float segmentStart = 0;
    float segmentStop = segmentWidth;
    
    GLUquadric* quadric = gluNewQuadric();
    Vec3fa Phit; //Supposedly the point of intersection of the ray with the plane supporting the triangle
    Ray pickRay;
    
    ushort selectedLightIdOne = 0;
    ushort selectedLightIdTwo = 0;
    
    double rotateX = 0;
    
    bool meshSelected = false;
    std::vector<riv::RowFilter*> pathFiltersOne;
    ushort bounceCountOne = 0;
//    std::set<ushort> selectedObjectIdsOne;
    ushort selectedObjectIdOne = -1;
    std::vector<riv::RowFilter*> pathFiltersTwo;
//    std::set<ushort> selectedObjectIdsTwo;
    ushort bounceCountTwo = 0;
    ushort selectedObjectIdTwo = -1;
    
    bool pathsCreated = false;
    
    //Determines if the objectId is currently selected
    bool isSelectedObject(std::set<ushort>& objectIds, ushort objectId);
    //Draw the mesh model loaded from the PBRT file
//    void drawMeshModel(TriangleMeshGroup* meshGroup, float* color, std::set<ushort> selectedObjectId);
    void drawMeshModel(TriangleMeshGroup* meshGroup,const riv::Color& color,ushort* selectedObjectID = NULL, const float lineThickness = 1);
    void drawPaths(float startSegment, float stopSegment);
    void drawPaths(RIVDataSet<float,ushort>* dataset, const std::vector<Path>& paths, float startSegment, float stopSegment,const Vector3f& cameraPosition); //Draw the paths between two consecutive bounces
    void drawPoints();
    //Draw the intersection points
    void drawPoints(RIVDataSet<float,ushort>* dataset, const std::vector<Path>& paths);
    void drawLightCones(const std::map<size_t,LightCone*>& lightCones);
    void drawCamera(const Vec3fa& cameraPosition);
    void drawEnergyHelper(OctreeNode* node, float max,riv::ColorMap& heatmap,ushort maxDepth);
    void drawEnergyDifferenceHelper(OctreeNode* nodeOne, OctreeNode* nodeTwo, float max, riv::ColorMap& colors);
    void drawEnergyDistribution(Octree* energyDistribution,ushort maxDepth);
    void drawEnergyDistribution(Octree* energyDistribution,ushort maxDepth, float maxEnergy);
    void drawEnergyDifference(Octree* energyDistributionOne, Octree* energyDistributionTwo,ushort maxDepth);
    void drawTriangleMeshFull(TriangleMeshFull* mesh, const riv::Color& color, const float lineThickness = 1);
    void drawGizmo(Gizmo* gizmo);
    void drawGizmos();
    void drawLights(const std::vector<Ref<Light>>& lights, const riv::Color& membershipColor);
    
    void filterPaths(RIVDataSet<float,ushort>* dataset, ushort bounceNr, std::set<ushort>& selectedObjectID, std::vector<riv::RowFilter*>& pathFilters);
    bool pathCreation(RIVDataSet<float,ushort>* dataset, const TriangleMeshGroup& meshes,std::vector<riv::RowFilter*>& pathFilters, ushort* bounceCount, ushort* selectedObjectId);
//    bool pathCreation(RIVDataSet<float,ushort>* dataset, const TriangleMeshGroup& meshes,std::vector<riv::RowFilter*>& pathFilters, ushort* bounceCount, std::set<ushort>& selectedObjectId);
    
    void createLightPaths();
    std::vector<Path> createLightPaths(ushort lightID, RIVDataSet<float,ushort>* dataset,RIVColorProperty* pathColor, RIVColorProperty* pointColor);
    
    //Create graphics buffer from unfiltered data rows
    void createCameraPaths();
    std::vector<Path> createCameraPaths(RIVDataSet<float,ushort>*, RIVColorProperty* pathColor, RIVColorProperty* rayColor);
    
    Vec3fa screenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
    void redisplayWindow();
    Gizmo createGizmo(ushort newGizmoId);
    Gizmo createGizmo(ushort newGizmoId,const Vec3fa& position,const Vec3fa& scale);
public:
    //Single renderer constructor
    RIV3DView(RIVDataSet<float,ushort>** dataset,EMBREERenderer* renderer,const TriangleMeshGroup& sceneDataOne, Octree* energyDistribution, RIVColorProperty* pathColor, RIVColorProperty* rayColor);
    //Dual renderer constructor
    RIV3DView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo,EMBREERenderer* rendererOne, EMBREERenderer* rendererTwo, const TriangleMeshGroup& sceneDataOne, const TriangleMeshGroup& sceneDataTwo, Octree* energyDistributionOne, Octree* energyDistributionTwo, RIVColorProperty* pathColorOne, RIVColorProperty* rayColorOne, RIVColorProperty* pathColorTwo, RIVColorProperty* rayColorTwo);
    
    enum SelectionMode {
        PATH,
        INTERACTION,
        INTERACTION_AND_SHADOW,
        SHADOW,
        OBJECT
    };
    SelectionMode selectionMode = PATH;
    
    static int windowHandle;
    
    void filterForGizmo();
    void filterForGizmo(Gizmo* gizmo, RIVDataSet<float,ushort>* dataset);
    void filterForGizmos();
//    void filterForGizmos(RIVDataSet<float,ushort>* dataset,std::vector<riv::RowFilter*>& gizmoFilters);
    
    void CycleMeshDiplayMode();
    void Reshape(int newWidth, int newHeight);
    void Draw();
    bool HandleMouseMotionPassive(int x, int y);
    bool HandleMouse(int button, int state, int x, int y);
    bool HandleMouseMotion(int x, int y);
    
    void SetHeatmapDepth(int depth);
    void IncrementHeatmapDepth();
    void DecrementHeatmapDepth();
    
    void OnDataChanged(RIVDataSet<float,ushort>* source);
    void OnFiltersChanged(RIVDataSet<float,ushort>* source);
    
    void ResetGraphics();
    void AddGizmo();
    
    //Shortcut methods to automate path or other selection mechanisms, be sure to put the right path mode beforehand
    void FilterPathsOne(ushort bounceNr, ushort objectId);
    void FilterPathsTwo(ushort bounceNr, ushort objectId);
    void FilterPathsOne(ushort bounceNr, std::set<ushort>& objectIds);
    void FilterPathsTwo(ushort bounceNr, std::set<ushort>& objectIds);
    
    static void DrawInstance(); //Override
    static void ReshapeInstance(int,int);
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    static void PassiveMotion(int x, int y);
    
    void RotateView(double angle);
    void ToggleGizmoTranslationMode(int x, int y, int z);
    void ToggleActiveGizmo();
    void ToggleBackgroundColor();
    void IncrementBounceNrPath(int delta);
    void CycleSelectedLights();
    void CycleSelectionMode();
    void SetSelectionMode(SelectionMode newMode);
    void MovePathSegment(float ratioIncrement);
    void CyclePathSegment(bool direction = true); //Cycle the path segment to draw, direction bool indicates direction of cycling, positive meaning incrementing
    void ToggleDrawIntersectionPoints();
    void ToggleDrawHeatmap();
    void ToggleDrawPaths();
    void ToggleDrawDataSetOne();
    void ToggleDrawDataSetTwo();
    void SetModelData(const MeshModel&);
    void ZoomIn(float zoom);
    void MoveCamera(float,float,float);
};

#endif /* defined(__Afstuderen___DView__) */