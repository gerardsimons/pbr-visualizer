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
#include "TriangleMesh.h"
#include "Geometry.h"
#include "Vec3D.h"
#include "Ray.h"

class MeshModel {
private:
	std::vector<TriangleMesh> meshes;
    float scale  = 1.F;
    Point3D center;
    bool centered = false;
public:
    bool centerComputed = false;
    MeshModel() {
        
    }
    MeshModel(const std::vector<TriangleMesh>& meshes) {
		this->meshes = meshes;
//        Center();
        CenterAndScaleToUnit();
    }
//	Vec3Df GetVertex(size_t vertexIndex) {
//		size_t index = 3 * vertexIndex;
//		if(index + 2 < vertices.size()) {
//			return Vec3Df(vertices[index],vertices[index + 1],vertices[index + 2]);
//		}
//		else throw "Vertex out of bounds";
//	}
	bool TriangleIntersect(const Ray<float>& r, size_t& resultIndex, Vec3Df& Phit) {
		for(size_t i = 0 ; i < meshes.size() ; ++i) {
			float d;
			if(meshes[i].Intersect(r, Phit, d)) {
				resultIndex = i;
				printf("\n******* INTERSECTION WITH %zu *******\n",i);
				return true;
			}
				
		}
		printf("\n******* NO INTERSECTION *******\n");
		return false;
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
    const std::vector<TriangleMesh>& GetMeshes() {
        return meshes;
    }
    void ComputeCenter() {
        float cX = 0;
        float cY = 0;
        float cZ = 0;
		
		size_t numberOfVertices = 0;
		
		for(TriangleMesh& mesh : meshes) {
			for(size_t i = 0 ; i < mesh.vertices.size() ; i += 3) {
				cX += mesh.vertices[i];
				cY += mesh.vertices[i+1];
				cZ += mesh.vertices[i+2];
				numberOfVertices += 3;
			}
		}
        cX /= numberOfVertices / 3.F;
        cY /= numberOfVertices / 3.F;
        cZ /= numberOfVertices / 3.F;
        
        center = Point3D(cX,cY,cZ);
        
        centerComputed = true;
    }
    void Translate(float x, float y, float z) {
//        for(size_t i = 0 ; i < vertices.size() ; i += 3) {
//            vertices[i] += x;
//            vertices[i+1] += y;
//            vertices[i+2] += z;
//        }
		throw "Not yet implemented.";
        centerComputed = false;
        centered = false;
    }
    void CenterAndScaleToUnit() {
        if(!centerComputed) {
            ComputeCenter();
        }
        float maxDistance = std::numeric_limits<float>::min();
        //Find the maximum distance
		for(const TriangleMesh& mesh : meshes) {
			for (size_t i = 0; i < mesh.vertices.size (); i+=3){
				float dX= (mesh.vertices[i]-center.x);
				float dY= (mesh.vertices[i+1]-center.y);
				float dZ= (mesh.vertices[i+2]-center.z);
				
				float distance = sqrt(dX*dX+dY*dY+dZ*dZ);
				if (distance > maxDistance)
					maxDistance = distance ;
			}
		}
        //Translate to center and scale by max distance
        scale = 1.F / maxDistance;
		printf("Scaling model by %f\n",scale);
//        scale = 1.F;
		for(TriangleMesh& mesh : meshes) {
        for  (size_t i = 0; i < mesh.vertices.size (); i+=3)
        {
//            mesh.vertices[i] = (mesh.vertices[i] - center.x) * scale;
//            mesh.vertices[i+1] = (mesh.vertices[i+1] - center.y) * scale;
//            mesh.vertices[i+2] = (mesh.vertices[i+2] - center.z) * scale;
            
            mesh.vertices[i] = (mesh.vertices[i]) * scale;
            mesh.vertices[i+1] = (mesh.vertices[i+1]) * scale;
            mesh.vertices[i+2] = (mesh.vertices[i+2]) * scale;
        }
		}

        centered = true;
    }
    void Scale(float scalar) {
		throw "Not (yet) implemented.";
//        for (size_t i = 0; i < vertices.size (); i++){
//            vertices[i] *= scalar;
//        }
        scale = scalar;
        if(!centered) { //If it is not centered, the center might have moved
            centerComputed = false;
            centered = false;
        }
    }
};

#endif /* defined(__Afstuderen___DModel__) */
