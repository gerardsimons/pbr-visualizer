//
//  TriangleMeshGroup.cpp
//  embree
//
//  Created by Gerard Simons on 24/11/14.
//
//

#include "TriangleMeshGroup.h"

void TriangleMeshGroup::init() {

	float cX = 0;
	float cY = 0;
	float cZ = 0;
	size_t nrVertices = 0;
	//Determine the center
	for(TriangleMeshFull *mesh : triangleMeshes) {
		vector_t<Vec3fa>& position = mesh->position;
		for(size_t i = 0 ; i < position.size() ; ++i) {
			Vec3fa vertex = position[i];
			cX += vertex[0];
			cY += vertex[1];
			cZ += vertex[2];
		}
		nrVertices += position.size();
	}
	cX /= nrVertices;
	cY /= nrVertices;
	cZ /= nrVertices;
	center[0] = cX;
	center[1] = cY;
	center[2] = cZ;
	float maxDistance = 0;
	//Determine the scale we need to scale it to unit
	//Find the vertex with the most distance to the center
	for(TriangleMeshFull *mesh : triangleMeshes) {
		vector_t<Vec3fa>& position = mesh->position;
		for(size_t i = 0 ; i < position.size() ; ++i) {
			Vec3fa vertex = position[i];
			float dX = vertex[0] - cX;
			float dY = vertex[1] - cY;
			float dZ = vertex[2] - cZ;
			float distance = sqrt(dX * dX + dY * dY + dZ * dZ);
			if(distance > maxDistance) {
				maxDistance = distance;
			}
		}
	}
	scale = 1 / maxDistance;
	printf("meshmodel center = [%f,%f,%f]\n",center[0],center[1],center[2]);
	printf("meshmodel scale = %f\n",scale);
}


bool TriangleMeshGroup::Intersect(const Ray& ray, ushort& resultIndex, Vec3fa& Phit, float& shortestDistance) const {
	bool intersects = false;
	shortestDistance = -1000000;
	Vec3fa bestPhit = Phit;
	for(size_t i = 0 ; i < triangleMeshes.size() ; ++i) {
		//			float d;
//		printf("i = %d\n",i);
		TriangleMeshFull* mesh = triangleMeshes[i];
		vector_t<TriangleMeshFull::Triangle> triangles = mesh->triangles;
		vector_t<Vec3fa>& position = mesh->position;
		for(size_t j = 0 ; j < triangles.size() ; ++j) {
			Vec3fa v0 = position[triangles[j].v0];
			Vec3fa v1 = position[triangles[j].v1];
			Vec3fa v2 = position[triangles[j].v2];
			
			Vec3fa edge1 = v1 - v0;
			Vec3fa edge2 = v2 - v0;
			
			
			Vec3fa N = cross(edge1, edge2);
			//		std::cout << "Normal = " << N << std::endl;
			
			float nDotRay = dot(N, ray.dir);
			//		printf("nDotRay = %f\n",nDotRay);
			if (dot(N, ray.dir) == 0) {
				//			printf("Test 1 failed\n");
				continue; // ray parallel to triangle
			}
			
			float d = dot(N, v0);
			d = -d;
			float t = -(dot(N, ray.org) + d) / nDotRay;
			//		printf("d = %f\n",d);
			//		printf("t = %f\n",t);
			
			// inside-out test
			Phit = ray.org + t * ray.dir;
			//			Phit[2] = -Phit[2];
			
			
			// inside-out test edge0
			Vec3fa v0p = Phit - v0;
			float v = dot(N, cross(edge1, v0p));
			if (v < 0) {
				//			printf("Test 2 failed\n");
				continue; // P outside triangle
			}
			
			// inside-out test edge1
			Vec3fa v1p = Phit - v1;
			Vec3fa v1v2 = v2 - v1;
			float w = dot(N, cross(v1v2, v1p));
			if (w < 0) {
				//			printf("Test 3 failed\n");
				continue;
			}
			
			// inside-out test edge2
			Vec3fa v2p = Phit - v2;
			Vec3fa v2v0 = v0 - v2;
			float u = dot(N, cross(v2v0, v2p));
			if (u < 0) {
				//			printf("Test 4 failed\n");
				continue; // P outside triangle
			}
			
//			printf("WOW a succesful intersection test with mesh %zu triangle %zu.\n",i,j);
//			printf("Phit = ");
//			std::cout << Phit << std::endl;
//			resultIndex = i;
			
			float distance = embree::length((ray.org - Phit));
			if(distance > shortestDistance ) {
				resultIndex = i;
				shortestDistance = distance;
				bestPhit = Phit;
				intersects = true;
			}
		}
	}
//	if(!intersects)
//		printf("\n******* NO INTERSECTION *******\n");
	
	Phit = bestPhit;
	return intersects;
}