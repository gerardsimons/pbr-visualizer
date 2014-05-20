//
//  Geometry.h
//  Afstuderen
//
//  Created by Gerard Simons on 12/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef Afstuderen_Geometry_h
#define Afstuderen_Geometry_h

typedef struct Point {
	int x;
	int y;
} Point;

//Rectangular area used for selections and highlights
typedef struct Area {
	Point start;
	Point end;
} Area;

typedef struct Point3D {
    float x;
    float y;
    float z;
} Point3D;

#endif
