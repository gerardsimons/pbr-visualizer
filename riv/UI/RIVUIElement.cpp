//
//  RIVUIElement.cpp
//  embree
//
//  Created by Gerard Simons on 23/01/15.
//
//

#include "RIVUIElement.h"

#include <stdexcept>
#if __APPLE__
    #include <GLUT/GLUT.h>
#elif __linux
    #include <GL/glut.h>
#endif
#include <stdexcept>

RIVUIElement::RIVUIElement(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {
	
}
bool RIVUIElement::containsPoint(int pX, int pY) {
	return pX >= x && pX <= (x + width) && pY >= y && pY <= (y + height);
}
void RIVUIElement::drawText(char* text, int size, int x, int y, float* color, float sizeModifier) {
	//Estimate center, seems to be the magic number for font pixel size
	float xCenter = sizeModifier * size / 2.F;
	
	glLineWidth(1);
	glColor3f(*color,*(color+1),*(color+2));
	glPushMatrix();
	glTranslatef(x - xCenter,y, 0);
	glPushMatrix();
	glScalef(sizeModifier,sizeModifier,1);
	for(int i = 0 ; i < size ; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	}
	glPopMatrix();
	glPopMatrix();
}
void RIVUIElement::drawText(const std::string& text, int x, int y, float *color, float sizeModifier) {
	drawText((char*)text.c_str(),(int)text.size(),x,y,color,sizeModifier);
}
void RIVUIElement::drawText(const std::string& text, int x, int y, float sizeModifier) {
	float black[] = {0,0,0};
	drawText((char*)text.c_str(),(int)text.size(),x,y,black,sizeModifier);
}
void RIVUIElement::drawText(const std::string& text, int x, int y) {
	drawText(text, x, y, 0.08F);
}
