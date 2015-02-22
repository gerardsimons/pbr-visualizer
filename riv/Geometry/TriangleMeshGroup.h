//
//  TriangleMeshGroup.h
//  embree
//
//  Created by Gerard Simons on 24/11/14.
//
//

#ifndef __embree__TriangleMeshGroup__
#define __embree__TriangleMeshGroup__

#include <vector>
#include "devices/device_singleray/shapes/trianglemesh_full.h"
#include "Ray.h"

using namespace embree;

class TriangleMeshGroup {
private:
    float scale;

	Vec3fa center;
	std::vector<TriangleMeshFull*> triangleMeshes;
public:
    Vec2f xBounds;
    Vec2f yBounds;
    Vec2f zBounds;
	TriangleMeshGroup(const std::vector<embree::TriangleMeshFull*>& triangleMeshes) : triangleMeshes(triangleMeshes) {
		init();
	}
	TriangleMeshGroup() { };
	void init();
	operator bool() const { return triangleMeshes.size() > 0; };
	
	bool Intersect(const Ray& r, ushort& resultIndex, Vec3fa& Phit, float& shortestDistance) const ;
    size_t NumberOfMeshes() { return triangleMeshes.size(); };
	//Getters
	float GetScale() const { return scale; };
	Vec3fa GetCenter() const { return center; };
	std::vector<TriangleMeshFull*> GetTriangleMeshes() const { return triangleMeshes; };
};

#endif /* defined(__embree__TriangleMeshGroup__) */
