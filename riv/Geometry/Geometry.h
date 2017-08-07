//
//  Geometry.h
//  Afstuderen
//
//  Created by Gerard Simons on 12/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef Afstuderen_Geometry_h
#define Afstuderen_Geometry_h

#if __APPLE__
    #include <GLUT/GLUT.h>
#elif __linux
#include <GL/glut.h>
#endif
#include <cmath>
#include <ostream>
#include <iostream>



inline double euclideanDistance(double x1, double x2, double y1, double y2, double z1, double z2) {
//    double distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 -y1) + (z2 - z1) * (z2 - z1));
//    printf("(%f-%f)*(%f-%f)+(%f-%f)*(%f-%f)+(%f-%f)*(%f-%f)=%f\n",x1,x2,x1,x2,y1,y2,y1,y2,z1,z2,z1,z2,distance);
//    return distance;
//    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 -y1) + (z2 - z1) * (z2 - z1));
    
    throw std::runtime_error("This does not work anymore.");
}

class RIVPoint {
public:
	int x;
	int y;
    RIVPoint() {
        x = 0;
        y = 0;
    }
    RIVPoint(int x, int y) : x(x), y(y){
        
    }
};

//Rectangular area used for selections and highlights
class RIVRectangle {
public:
	RIVPoint start;
	RIVPoint end;
    
    RIVRectangle(int startX, int startY, int endX, int endY) {
        start.x = startX;
        start.y = startY;
        end.x = endX;
        end.y = endY;
    }
    RIVRectangle() {
        
    }
};

class Point3D {
public:
    float x;
    float y;
    float z;
    Point3D(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }
    Point3D() { x = 0; y = 0 ; z = 0;};
    
    float EuclideanDistanceTo(Point3D& P) {
        return sqrt(pow(P.x - x,2) + pow(P.y - y,2) + pow(P.z - z,2));
    }
    float ManhattanDistanceTo(Point3D& P) {
        return abs(float(P.x - x)) + abs(float(P.y - y)) + abs(float(P.z - z));
    }
    bool operator==(Point3D& p) {
        return p.x == x && p.y == y && p.z == z;
    }
    void Print() {
        printf("(%f,%f,%f)\n",x,y,z);
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Point3D& p) {
        os << "(" << p.x << "," << p.y << ',' << p.z << ")";
        return os;
    }
};

//A 3D Box
class Box3D {
private:
    static const int SIZE = 8;
    int facePoints[24];
    float color[4][3];
public:
    Point3D points[SIZE];
    bool initialized;
    Box3D() {
        initialized = false;
    };
    Box3D(float x, float y, float z, float width, float height, float depth) {
        initialized = true;
        //Front face
//        printf("x,y,z = %f,%f,%f  widht,height,depth = %f,%f,%f\n",x,y,z,width,height,depth);
        points[0] = Point3D(x,y,z);
        points[1] = Point3D(x + width, y, z);
        points[2] = Point3D(x + width, y + height,z);
        points[3] = Point3D(x, y + height, z);
        
        //Back face
        points[4] = Point3D(x,y,z + depth);
        points[5] = Point3D(x + width, y, z + depth);
        points[6] = Point3D(x + width, y + height,z + depth);
        points[7] = Point3D(x, y + height, z + depth);
        
        facePoints[0] = 0;
        facePoints[1] = 1;
        facePoints[2] = 2;
        facePoints[3] = 3;
        
        facePoints[4] = 4;
        facePoints[5] = 5;
        facePoints[6] = 6;
        facePoints[7] = 7;
        
        //Right face
        facePoints[8] = 1;
        facePoints[9] = 2;
        facePoints[10] = 5;
        facePoints[11] = 6;
        
        //Left face
        facePoints[12] = 0;
        facePoints[13] = 3;
        facePoints[14] = 4;
        facePoints[15] = 7;
        
        //Top Face
        facePoints[16] = 2;
        facePoints[17] = 3;
        facePoints[18] = 6;
        facePoints[19] = 7;
        
        //Bottom face
        facePoints[20] = 0;
        facePoints[21] = 1;
        facePoints[22] = 4;
        facePoints[23] = 5;
        
        color[0][0] = 1.F;
        color[0][1] = 0.F;
        color[0][2] = 0.F;
        
        color[1][0] = 0.F;
        color[1][1] = 1.F;
        color[1][2] = 0.F;
        
        color[2][0] = 0.F;
        color[2][1] = 0.F;
        color[2][2] = 1.F;
        
        color[3][0] = 0.F;
        color[3][1] = 1.F;
        color[3][2] = 1.F;
        
        color[4][0] = 1.F;
        color[4][1] = 1.F;
        color[4][2] = 0.F;
        
        color[5][0] = 1.F;
        color[5][1] = 0.F;
        color[5][2] = 1.F;
    }
    bool ContainsPoint(const Point3D& point) {
        float const &x = point.x;
        float const &y = point.y;
        float const &z = point.z;
        //right x value
        if(x < points[0].x || x > points[1].x) {
            return false;
        }
        if(y < points[0].y || x > points[2].y) {
            return false;
        }
        if(z < points[0].z || z > points[4].z) {
            return false;
        }
        return true;
        //right y value
        
        //right z value
    }
    void Draw() {
        
        glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        for(int i = 0 ; i < 4 ; i++) {
                glVertex3f(        points[i].x,         points[i].y,         points[i].z);
        }
        glColor3f(0, 0, 1);
        for(int i = 4 ; i < 8 ; i++) {
            glVertex3f(        points[i].x,         points[i].y,         points[i].z);
        }
        int leftFace[4] = {4,3,0,7};
            glColor3f(0, 1, 0); //Green
        for(int i = 0 ; i < 4 ; i++) {
            glVertex3f(        points[leftFace[i]].x,         points[leftFace[i]].y,         points[leftFace[i]].z);
        }
        
//        facePoints[0] = 0;
//        facePoints[1] = 1;
//        facePoints[2] = 2;
//        facePoints[3] = 3;
//        
//        facePoints[4] = 4;
//        facePoints[5] = 5;
//        facePoints[6] = 6;
//        facePoints[7] = 7;
//        //Front face
//        for(int i = 0 ; i < 24 ; i++) {
//            int index = facePoints[i];
//            Point3D p = points[index];
//            const int faceIndex = i / 4;
//            printf("Point %d : ",i);
//            p.Print();
//            
////            glColor4f(color[faceIndex][0],color[faceIndex][1],color[faceIndex][2], 1.F);
//            glVertex3f(p.x,p.y,(p).z);
//            if((i + 1) % 4 == 0) {
//                printf("\n");
//                float r = rand() / (float)RAND_MAX;
//                float g = rand() / (float)RAND_MAX;
//                float b = rand() / (float)RAND_MAX;
//                
//                glColor4f(r,g,b, 1.F);
//            }
//            
//        }
    
        
        glEnd();
        
    }
};

#endif
