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
#include <stdlib.h>

#include "TriangleMesh.h"
#include "Geometry.h"
#include "Vec3D.h"
#include "Ray.h"

typedef unsigned short ushort;


class MeshModel {
private:
	std::vector<riv::TriangleMesh> meshes;
	
    float scale  = 1.F;
    Vec3Df position;
	ushort objectID;
	
//    bool centered = false;
public:
    bool centerComputed = false;
    MeshModel() {
        position = Vec3Df(0,0,0);
    }
    MeshModel(const std::vector<riv::TriangleMesh>& meshes,ushort objectID) {
		this->meshes = meshes;
		this->objectID = objectID;
//        Center();
//        CenterAndScaleToUnit();
    }
	bool MeshContaining(const Vec3Df& p, riv::TriangleMesh*& result, size_t& triangleIndex) {
		for(size_t i = 0 ; i < meshes.size() ; ++i) {
			riv::TriangleMesh* mesh = &meshes[i];
			if(mesh->Contains(p,triangleIndex)) {
				result = mesh;
//				printf("Mesh #%zu contains p\n",i);
				return true;
			}
		}
		std::cout << "WARNING: No mesh found to contain " << p << std::endl;
		return false;
	}
	bool TriangleIntersect(const riv::Ray<float>& r, size_t& resultIndex, Vec3Df& Phit, float& shortestDistance) {
		bool intersects = false;
		shortestDistance = -10000;
		Vec3Df bestPhit = Phit;
		for(size_t i = 0 ; i < meshes.size() ; ++i) {
//			float d;
			if(meshes[i].Intersect(r, Phit)) {
//				printf("\n******* INTERSECTION WITH %zu *******\n",i);
				float distance = (Phit - r.orig).getSquaredLength();
//				printf("distance = %f\n",distance);
				if(distance > shortestDistance) {
					resultIndex = i;
					shortestDistance = distance;
					bestPhit = Phit;
					intersects = true;
//					printf("is new BEST\n");
				}
//				else {
//					printf(" is NOT good enough.\n");
//				}
			}
		}
		if(!intersects)
//			printf("\n******* NO INTERSECTION *******\n");
		Phit = bestPhit;
		return intersects;
	}
	ushort GetObjectID() {
		return objectID;
	}
    Vec3Df GetPosition() {
		return position;
    }
    float GetScale() {
        return scale;
    }
    const std::vector<riv::TriangleMesh>& GetMeshes() {
        return meshes;
    }
    void ComputeCenter() {
        float cX = 0;
        float cY = 0;
        float cZ = 0;
		
		size_t numberOfVertices = 0;
		
		for(riv::TriangleMesh& mesh : meshes) {
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
        
        position = Vec3Df(cX,cY,cZ);
        
        centerComputed = true;
    }
    void Translate(float x, float y, float z) {
		position += Vec3Df(x,y,z);
    }
    void Scale(float scalar) {
        scale *= scalar;
    }
};


#endif /* defined(__Afstuderen___DModel__) */
