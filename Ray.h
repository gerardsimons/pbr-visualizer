//
//  Ray.h
//  afstuderen
//
//  Created by Gerard Simons on 11/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen__Ray__
#define __afstuderen__Ray__

#include "Geometry.h"
#include "Vec3D.h"

//class Ray {
//public:
//	Vec3Df origin;
//	Vec3Df dir;
//	Ray(const Point3D& origin, const Point3D& end) {
//		dir = Vec3Df(end.x - origin.x,end.y - origin.y,end.z - origin.z);
//		this->origin = Vec3Df(origin.x,origin.y,origin.z);
//	}
//private:
//};

template<typename T>
class Ray
{
public:
	Ray() : tmin(0), tmax(std::numeric_limits<T>::max()) {}
	Ray(const Vec3Df &o, const Vec3Df &d) : orig(o), dir(d), tmin(0), tmax(std::numeric_limits<T>::max()) {}
	Vec3Df orig, dir;
	mutable T tmin, tmax;
};


#endif /* defined(__afstuderen__Ray__) */
