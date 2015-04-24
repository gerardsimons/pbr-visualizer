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
    Vec3fa translation;
	Vec3fa center;
	std::vector<TriangleMeshFull*> triangleMeshes;
public:
    Vec2f xBounds;
    Vec2f yBounds;
    Vec2f zBounds;
	TriangleMeshGroup(const std::vector<embree::TriangleMeshFull*>& triangleMeshes) : triangleMeshes(triangleMeshes) {
		init();
	}
    TriangleMeshGroup(embree::TriangleMeshFull* triangleMesh) {
        triangleMeshes.push_back(triangleMesh);
        init();
    }
	TriangleMeshGroup() { };
	void init();
	operator bool() const { return triangleMeshes.size() > 0; };
    void Translate(const Vec3fa& translation);
    void Translate(float x, float y, float z);
	bool Intersect(const Ray& r, ushort& resultIndex, Vec3fa& Phit, float& shortestDistance) const ;
    bool Intersect(const Ray& r, ushort& resultIndex, Vec3fa& Phit, float& shortestDistance, const Vec3fa& distanceCompare) const ;
    size_t NumberOfMeshes() const { return triangleMeshes.size(); };
	//Getters
	float GetScale() const { return scale; };
	Vec3fa GetCenter() const { return center; };
	std::vector<TriangleMeshFull*> GetTriangleMeshes() const { return triangleMeshes; };
};

#endif /* defined(__embree__TriangleMeshGroup__) */
