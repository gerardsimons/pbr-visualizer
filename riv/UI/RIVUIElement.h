//
//  RIVUIElement.h
//  embree
//
//  Created by Gerard Simons on 23/01/15.
//
//

#ifndef __embree__RIVUIElement__
#define __embree__RIVUIElement__

class RIVUIElement {
private:
	int x;
	int y;
	int width;
	int height;
	
	bool containsPoint(int pX, int pY);
public:
//	virtual void Draw() = 0;
	//Checks if the mouse interacts with this ui element, returns true if it does interact
//	virtual bool Mouse(int x, int y, int button, int state);
protected:
	RIVUIElement(int x, int y, int width, int height);
};

#endif /* defined(__embree__RIVUIElement__) */
