//
//  graphics_helper.h
//  Afstuderen
//
//  Created by Gerard Simons on 19/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

//Some useful methods for debugging, drawing of cubes and faces

#ifndef Afstuderen_graphics_helper_h
#define Afstuderen_graphics_helper_h

#include <string>

//function to draw coordinate axes with a certain length (1 as a default)
void drawCoordSystem(float length=1);

void drawText(char *text, int size, int x, int y, float *color, float sizeModifier);

void drawText(std::string text, int x, int y, float *color, float sizeModifier);

void drawUnitFace();

void drawUnitCube();
void drawArm();

#endif
