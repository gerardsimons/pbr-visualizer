//
//  3DModel.h
//  Afstuderen
//
//  Created by Gerard Simons on 19/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen___DModel__
#define __Afstuderen___DModel__

#include <vector>
#include "Geometry.h"

class MeshModel {
private:
    std::vector<float> vertices;
    float scale  = 1.F;
    Point3D center;
    bool centered = false;
public:
    bool centerComputed = false;
    MeshModel() {
        
    }
    MeshModel(const std::vector<float>& vertices_) {
        if(vertices_.size() == 0 || vertices.size() % 3 != 0) {
            throw "Malformed vertex data.";
        }
        vertices = vertices_;
//        Center();
        CenterAndScaleToUnit();
    }
    Point3D GetCenter() {
        if(!centerComputed) {
            ComputeCenter();
        }
        return center;
    }
    float GetScale() {
        return scale;
    }
    const std::vector<float>& GetVertices() {
        return vertices;
    }
    void ComputeCenter() {
        float cX = 0;
        float cY = 0;
        float cZ = 0;
        for(size_t i = 0 ; i < vertices.size() ; i += 3) {
            cX += vertices[i];
            cY += vertices[i+1];
            cZ += vertices[i+2];
        }
        cX /= vertices.size() / 3.F;
        cY /= vertices.size() / 3.F;
        cZ /= vertices.size() / 3.F;
        
        center = Point3D(cX,cY,cZ);
        
        centerComputed = true;
    }
    void Translate(float x, float y, float z) {
        for(size_t i = 0 ; i < vertices.size() ; i += 3) {
            vertices[i] += x;
            vertices[i+1] += y;
            vertices[i+2] += z;
        }
        centerComputed = false;
        centered = false;
    }
    void CenterAndScaleToUnit() {
        if(!centerComputed) {
            ComputeCenter();
        }
        float maxDistance = std::numeric_limits<float>::min();
        //Find the maximum distance
        for (size_t i = 0; i < vertices.size (); i+=3){
            float dX= (vertices[i]-center.x);
            float dY= (vertices[i+1]-center.y);
            float dZ= (vertices[i+2]-center.z);
            
            float distance = sqrt(dX*dX+dY*dY+dZ*dZ);
            if (distance > maxDistance)
                maxDistance = distance ;
        }
        //Translate to center and scale by max distance
        scale = 1.F / maxDistance;
//        scale = 1.F;
        for  (size_t i = 0; i < vertices.size (); i+=3)
        {
            vertices[i] = (vertices[i] - center.x) * scale;
            vertices[i+1] = (vertices[i+1] - center.y) * scale;
            vertices[i+2] = (vertices[i+2] - center.z) * scale;
        }

        centered = true;
    }
    void Scale(float scalar) {
        for (size_t i = 0; i < vertices.size (); i++){
            vertices[i] *= scalar;
        }
        scale = scalar;
        if(!centered) { //If it is not centered, the center might have moved
            centerComputed = false;
            centered = false;
        }
    }
};

#endif /* defined(__Afstuderen___DModel__) */
