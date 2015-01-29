//
//  RIVUIElement.h
//  embree
//
//  Created by Gerard Simons on 23/01/15.
//
//

#ifndef __embree__RIVUIElement__
#define __embree__RIVUIElement__

#include <string>

class RIVUIElement {
private:
protected:
	bool needsRedraw = true;
	bool containsPoint(int pX, int pY);
	bool hidden = false;
	
	RIVUIElement(int x, int y, int width, int height);
	void drawText(char* text, int size, int x, int y, float* color, float sizeModifier);
	void drawText(const std::string& text, int x, int y, float *color, float sizeModifier);
	void drawText(const std::string& text, int x, int y, float sizeModifier);
	void drawText(const std::string& text, int x, int y);
public:
	int x;
	int y;
	int width;
	int height;
	void Hide() {
		hidden = true;
	}
	void Show() {
		hidden = false;
	}
	virtual void Draw() = 0;
	//Checks if the mouse interacts with this UI element, returns true if it does interact
	virtual bool Mouse(int button, int state, int x, int y) = 0;
};

#endif /* defined(__embree__RIVUIElement__) */
