/* loadppm.h */

#ifndef __LOADPPM_H
#define __LOADPPM_H

#include <iostream>
#include <fstream>
#include <cmath>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



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
