//
//  TriangleMesh.h
//  afstuderen
//
//  Created by Gerard Simons on 12/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen__TriangleMesh__
#define __afstuderen__TriangleMesh__

#include <vector>
#include <map>

#include "Vec3D.h"
#include "Ray.h"

class TriangleMesh {
private:
	std::map<size_t,ushort> shapeIDs;
	bool shapeIDAssigned = false;
public:
	std::vector<float> vertices;
	std::vector<size_t> indices;
	TriangleMesh(const std::vector<float>& vertices, const std::vector<size_t>& indices) {
		this->vertices = vertices;
		this->indices = indices;
	}
	Vec3Df GetVertex(size_t vertexIndex) const;
	bool Intersect(const Ray<float>& r, Vec3Df& Phit);
	bool Contains(const Vec3Df& point, size_t& triangleIndex);
	
	void AssignShapeId(size_t triangleIndex, ushort shapeID);
};

#endif /* defined(__afstuderen__TriangleMesh__) */
