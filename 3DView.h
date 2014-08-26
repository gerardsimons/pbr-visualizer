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
#include "MeshModel.h"

class RIV3DView : public RIVDataView, public RIVDataSetListener {
private:
    MeshModel modelData;
    Point3D eye;
    bool isDirty = true;
    //Whether to draw the members of clusters, or only the cluster medoid
    bool drawClusterMembers = true;
    //The mouse cursor in world coordinates at last click
    Point3D cursorNear;
    Point3D cursorFar;
    
    //Buffered graphics point data, generated from the data, stored here for speed
    std::vector<float> pointsX;
    std::vector<float> pointsY;
    std::vector<float> pointsZ;
    std::vector<float> pointsR;
    std::vector<float> pointsG;
    std::vector<float> pointsB;
    std::vector<float> pointsSize;

    //Possibly the selection box created by clicking and dragging (NOT IMPLEMENTED)
    Box3D selectionBox;
    
    //Create graphics buffer from unfiltered data rows
    void createPoints();
    void drawPaths(int startBounce); //Draw the paths between two consecutive bounces
    
    static RIV3DView* instance;
    Point3D screenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
public:
    RIV3DView();
    RIV3DView(int,int,int,int,int,int,RIVColorProperty*,RIVSizeProperty*);
    RIV3DView(RIVColorProperty*,RIVSizeProperty*);
    void Reshape(int newWidth, int newHeight);
	 void Draw();
	 bool HandleMouse(int button, int state, int x, int y);
	 bool HandleMouseMotion(int x, int y);
     void OnDataSetChanged();
    
    void SetData(RIVDataSet *newDataSet) { RIVDataView::SetData(newDataSet); createPoints(); };
    static void DrawInstance(); //Override
    static void ReshapeInstance(int,int);
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    
    void ToggleDrawClusterMembers();
    void SetModelData(const MeshModel&);
    void MoveCamera(float,float,float);
    
    //The models to draw

    //The camera
    
    //Anything else?
};

#endif /* defined(__Afstuderen___DView__) */
