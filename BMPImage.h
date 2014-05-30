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
private:
    bool loadTexture(const char* imagePath, int alpha);
public:
	BMPImage(const char* imagePath,bool hasAlpha);
    bool hasAlpha;
	int sizeX,sizeY;
	GLubyte *data;
    short* RGB(int x, int y) const;
	virtual ~BMPImage();
};

#endif /* BMPIMAGE_H_ */
