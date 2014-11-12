//
//  Ray.h
//  afstuderen
//
//  Created by Gerard Simons on 11/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen__Ray__
#define __afstuderen__Ray__

#include <limits>
#include "../Geometry/Geometry.h"
#include "Vec3D.h"

namespace riv {
	template<typename T>
	class Ray
	{
	public:
		Ray() : tmin(0), tmax(std::numeric_limits<T>::max()) {}
		Ray(const Vec3Df &o, const Vec3Df &d) : orig(o), dir(d), tmin(0), tmax(std::numeric_limits<T>::max()) {}
		Vec3Df orig, dir;
		mutable T tmin, tmax;
	};
}


#endif /* defined(__afstuderen__Ray__) */
