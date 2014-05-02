/* loadppm.h */

#ifndef __LOADPPM_H
#define __LOADPPM_H

#include <iostream>
#include <fstream>
#include <math.h>

#include <GL/glut.h>

using namespace std;

class PPMImage {
public:
  int sizeX, sizeY;
  GLubyte *data;
  PPMImage(const char *filename);
  ~PPMImage(){
    if (data)
      delete [] data;
  }
};

#endif
