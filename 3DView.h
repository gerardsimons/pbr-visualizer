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

class RIV3DView : public RIVDataView, public RIVDataSetListener {
private:
    std::vector<float> modelData;
    Point3D eye;
    bool isDirty = true;
    //Rotation parameters
    GLfloat xRotated = 0.F;
    GLfloat yRotated = 0.F;
    GLfloat zRotated = 0.F;
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
    //A constant defining the basic size of a sphere, this value is linearly increased for cluster medoids according to cluster size
    const float sphereSizeDefault = .1F;
    Point3D ScreenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
public:
    RIV3DView(int,int,int,int,int,int,RIVColorProperty*);
     void ComputeLayout();
	 void Draw();
	 bool HandleMouse(int button, int state, int x, int y);
	 bool HandleMouseMotion(int x, int y);
     void OnDataSetChanged();
    
    void ToggleDrawClusterMembers();
    void SetModelData(std::vector<float>);
    void MoveCamera(float,float,float);
    
    //The models to draw

    //The camera
    
    //Anything else?
};

#endif /* defined(__Afstuderen___DView__) */
