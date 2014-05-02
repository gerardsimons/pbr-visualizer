/*
 * BMPImage.h
 *
 *  Created on: Jun 6, 2013
 *      Author: gerard
 */

#ifndef BMPIMAGE_H_
#define BMPIMAGE_H_


#include <iostream>
#include <fstream>
#include <math.h>

#include <GLUT/glut.h>

class BMPImage {
public:
	BMPImage(const char* imagePath,bool hasAlpha);
	int width,height;
	GLubyte *data;
	bool loadTexture(const char* imagePath, int alpha);
	virtual ~BMPImage();
};

#endif /* BMPIMAGE_H_ */
