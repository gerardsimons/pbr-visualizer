//
//  TriangleMesh.cpp
//  afstuderen
//
//  Created by Gerard Simons on 12/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "TriangleMesh.h"
#include "../helper.h"

	Vec3Df TriangleMesh::GetVertex(size_t vertexIndex) const {
		size_t index = 3 * vertexIndex;
		if(index + 2 < vertices.size()) {
			return Vec3Df(vertices[index],vertices[index + 1],vertices[index + 2]);
		}
		else throw "Vertex out of bounds";
	}

	bool TriangleMesh::Intersect(const Ray<float>& r, Vec3Df& Phit) {
	//	size_t i = 0;
		for(size_t index = 0 ; index < indices.size() ; index += 3) {
	//		printf("Testing triangle %zu\n",i);
	//		++i;
			//			if(triangleIndex == )
			//			printf("TriangleIndex = %zu\n",triangleIndex);
	//		printf("Ray origin = ");
	//		std::cout << r.orig << " dir = " << r.dir << std::endl;
			
			Vec3Df v0 = GetVertex(indices[index]);
			Vec3Df v1 = GetVertex(indices[index+1]);
			Vec3Df v2 = GetVertex(indices[index+2]);
			
			Vec3Df edge1 = v1 - v0;
			Vec3Df edge2 = v2 - v0;
			
			Vec3Df N = Vec3Df::crossProduct(edge1, edge2);
	//		std::cout << "Normal = " << N << std::endl;
			float nDotRay = Vec3Df::dotProduct(N, r.dir);
	//		printf("nDotRay = %f\n",nDotRay);
			if (Vec3Df::dotProduct(N, r.dir) == 0) {
	//			printf("Test 1 failed\n");
				continue; // ray parallel to triangle
			}
			
			float d = Vec3Df::dotProduct(N, v0);
			d = -d;
			float t = -(Vec3Df::dotProduct(N, r.orig) + d) / nDotRay;
	//		printf("d = %f\n",d);
	//		printf("t = %f\n",t);
			
			// inside-out test
			Phit = r.orig + t * r.dir;
			//			Phit[2] = -Phit[2];
			
	//		std::cout << "Phit = " << Phit << std::endl;
			size_t result=0;
			if(Contains(Phit,result)) {
				return true;
			}
			else {
				continue;
			}
			
			// inside-out test edge0
			Vec3Df v0p = Phit - v0;
			float v = Vec3Df::dotProduct(N, Vec3Df::crossProduct(edge1, v0p));
			if (v < 0) {
	//			printf("Test 2 failed\n");
				continue; // P outside triangle
			}
			
			// inside-out test edge1
			Vec3Df v1p = Phit - v1;
			Vec3Df v1v2 = v2 - v1;
			float w = Vec3Df::dotProduct(N, Vec3Df::crossProduct(v1v2, v1p));
			if (w < 0) {
	//			printf("Test 3 failed\n");
				continue;
			}
			
			// inside-out test edge2
			Vec3Df v2p = Phit - v2;
			Vec3Df v2v0 = v0 - v2;
			float u = Vec3Df::dotProduct(N, Vec3Df::crossProduct(v2v0, v2p));
			if (u < 0) {
	//			printf("Test 4 failed\n");
				continue; // P outside triangle
			}
			
			//			isectData.t = t;
	//		printf("WOW a succesful intersection test with triangle %zu.\n",i);
	//		resultIndex = triangleIndex;
			return true;
		}
		return false;
	}

	bool TriangleMesh::Contains(const Vec3Df &point, size_t& triangleIndex) {
		for(size_t index = 0 ; index < indices.size() ; index += 3) {
			Vec3Df v0 = GetVertex(indices[index]);
			Vec3Df v1 = GetVertex(indices[index+1]);
			Vec3Df v2 = GetVertex(indices[index+2]);
			
			Vec3Df edge1 = v1 - v0;
			Vec3Df edge2 = v2 - v0;
			
			Vec3Df N = Vec3Df::crossProduct(edge1, edge2);
			
			// inside-out test edge0
			Vec3Df v0p = point - v0;
			float v = Vec3Df::dotProduct(N, Vec3Df::crossProduct(edge1, v0p));
			if (v < 0) {
				//			printf("Test 2 failed\n");
				continue; // P outside triangle
			}
			
			// inside-out test edge1
			Vec3Df v1p = point - v1;
			Vec3Df v1v2 = v2 - v1;
			float w = Vec3Df::dotProduct(N, Vec3Df::crossProduct(v1v2, v1p));
			if (w < 0) {
				//			printf("Test 3 failed\n");
				continue;
			}
			
			// inside-out test edge2
			Vec3Df v2p = point - v2;
			Vec3Df v2v0 = v0 - v2;
			float u = Vec3Df::dotProduct(N, Vec3Df::crossProduct(v2v0, v2p));
			if (u < 0) {
				//			printf("Test 4 failed\n");
				continue; // P outside triangle
			}
			
			triangleIndex = index / 3; //Should be dividable by 3 always
			return true;
		}
		return false;
	}

	void TriangleMesh::AssignShapeId(size_t triangleIndex, ushort shapeID) {
		printf("Assigning shape ID %d to triangle %zu\n",shapeID, triangleIndex);
		if(shapeIDAssigned) {
	//		printf("WARNING: Overwriting existing shapeID %d\n",this->shapeID);
		}
		shapeIDAssigned = true;
		shapeIDs[triangleIndex] = shapeID;
		printMap(shapeIDs);
	}

