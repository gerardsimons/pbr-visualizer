/*
 * BMPImage.h
 *
 *  Created on: Jun 6, 2013
 *      Author: gerard
 */

#ifndef BMPIMAGE_H_
#define BMPIMAGE_H_

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class BMPImage {
public:
	BMPImage(const char* imagePath,bool hasAlpha);
	int sizeX,sizeY;
	GLubyte *data;
	bool loadTexture(const char* imagePath, int alpha);
	virtual ~BMPImage();
};

#endif /* BMPIMAGE_H_ */
