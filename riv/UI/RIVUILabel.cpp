//
//  RIVUILabel.cpp
//  embree
//
//  Created by Gerard Simons on 23/01/15.
//
//

#include "RIVUILabel.h"

RIVUILabel::RIVUILabel(const std::string& text, int x, int y, int width, int height) : RIVUIElement(x,y,width,height), displayText(text) {
	
}

void RIVUILabel::Draw() {
	drawText(displayText, x, y, 0.10);
}

bool RIVUILabel::Mouse(int x, int y, int button, int state) {
	if(containsPoint(x, y)) {
		return true; //Catches mouse but does nothing...
	}
	return false;
};

