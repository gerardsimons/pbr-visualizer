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
    bool drawClusterMembers = false;
    //The mouse cursor in world coordinates at last click
    Point3D cursorNear;
    Point3D cursorFar;
    //View bounding parameters
    float zNear = 1.F;
    float zFar = 20.F;
    //Possibly the selection box created by clicking and dragging (NOT IMPLEMENTED)
    Box3D selectionBox;
    
    Point3D screenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
public:
    RIV3DView(int,int,int,int,int,int,RIVColorProperty*,RIVSizeProperty*);
    RIV3DView(RIVColorProperty*,RIVSizeProperty*);
    void ComputeLayout(float startX, float startY, float width, float height, float paddingX, float paddingY);
	 void Draw();
	 bool HandleMouse(int button, int state, int x, int y);
	 bool HandleMouseMotion(int x, int y);
     void OnDataSetChanged();
    
    void ToggleDrawClusterMembers();
    void SetModelData(const MeshModel&);
    void MoveCamera(float,float,float);
    void Reshape(float newWidth, float newHeight);
    
    //The models to draw

    //The camera
    
    //Anything else?
};

#endif /* defined(__Afstuderen___DView__) */
