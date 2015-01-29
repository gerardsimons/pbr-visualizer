//
//  RIVUILabel.h
//  embree
//
//  Created by Gerard Simons on 23/01/15.
//
//

#ifndef __embree__RIVUILabel__
#define __embree__RIVUILabel__

#include <GLUT/GLUT.h>
#include <string>

#include "RIVUIElement.h"

/**
 *		A Simple UI Label that displays some text
 */
class RIVUILabel : public RIVUIElement{
private:
	std::string displayText;
public:
	RIVUILabel(const std::string& text, int x, int y, int width, int height);
	
	void Draw();
	bool Mouse(int x, int y, int width, int height);
};

#endif /* defined(__embree__RIVUILabel__) */
