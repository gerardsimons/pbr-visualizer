//
//  RIVUIDropdown.cpp
//  embree
//
//  Created by Gerard Simons on 27/01/15.
//
//

#include "RIVUIDropdown.h"
#include <stdexcept>
#if __APPLE__
    #include <GLUT/GLUT.h>
#elif __linux
    #include <GL/glut.h>
#endif

RIVUIDropdown::RIVUIDropdown(int x, int y, int width, int height, const std::vector<std::string>& stringElements) : RIVUIElement(x,y,width,height) {
	if(stringElements.size()) {
		selectedElementIndex = 0;
	}
	else {
		selectedElementIndex = -1;
	}
	createDropdownElements(stringElements);
}
RIVUIDropdown::RIVUIDropdown(int x, int y, int width, int heights) : RIVUIElement(x,y,width,height) {
	selectedElementIndex = -1;
}
void RIVUIDropdown::createDropdownElements(const std::vector<std::string>& texts) {
	elements.clear();
	int elementY = y - height;
	for(int i = 0 ; i < texts.size() ; ++i) {
		elements.push_back(RIVDropdownElement(elementY,texts[i]));
		elementY -= height;
	}
}
void RIVUIDropdown::SetValues(const std::vector<std::string>& values) {
	createDropdownElements(values);
}
void RIVUIDropdown::SetSelectedValue(const std::string& value) {
	for(int i = 0 ; i < elements.size() ; ++i) {
		if(elements[i].text == value) {
			selectedElementIndex = i;
			notifyListeners();
			return;
		}
	}
	throw std::runtime_error("No such value exists in this dropdown");
}
void RIVUIDropdown::SetSelectedValue(int i) {
	if(i < elements.size()) {
		selectedElementIndex = i;
		notifyListeners();
	}
	else {
		throw std::runtime_error("Not that many elements exist.");
	}
}
void RIVUIDropdown::Draw() {
	needsRedraw = true;
	if(needsRedraw && !hidden) {
		if(selectedElementIndex >= 0) {
			printf("RIVUIDropdown %s redraw\n",elements[selectedElementIndex].text.c_str());
		}
		
		const int lineWidth = 2;
		
		glClearColor(1, 1, 1, 0);
		glColor3f(1, 1,1);
		
		//Draw a rectangle as background
		//	glRectf(x, y, width, height);
		
		glColor3f(0, 0, 0);
		glLineWidth(lineWidth);
		//Draw the lines around the rectangle
		
		glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + width - lineWidth, y);
		glVertex2f(x + width - lineWidth, y + height - lineWidth);
		glVertex2f(x, y + height - lineWidth);
		glEnd();
		
		if(selectedElementIndex >= 0) {
			//		if(collapsed) {
			const std::string& selectedText = elements[selectedElementIndex].text;
			drawText(selectedText, x + 5,y + 5, 0.08);
			//		}
			if(!collapsed) {
				//Draw all the dropdown elements
				for(const RIVDropdownElement& element : elements) {
					glColor3f(1,1,1);
					glRectf(x, element.y, x+width, element.y + height);
					glLineWidth(lineWidth);
					glColor3f(0,0, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(x, element.y);
					glVertex2f(x + width - lineWidth, element.y);
					glVertex2f(x + width - lineWidth, element.y + height - lineWidth);
					glVertex2f(x, element.y + height - lineWidth);
					glEnd();
					drawText(element.text, x + 5,element.y + 5, 0.08);
				}
			}
		}
		needsRedraw = false;
	}
	
	
	//Draw the active text
	
 
}
void RIVUIDropdown::AddListener(RIVUIDropdownListener *listener) {
	listeners.push_back(listener);
}
void RIVUIDropdown::notifyListeners() {
	for(auto listener : listeners) {
		listener->OnValueChanged(this, selectedElementIndex, elements[selectedElementIndex].text);
	}
}
bool RIVUIDropdown::Mouse(int x, int y, int button, int state) {
	if(containsPoint(x, y)) {
		if(state == GLUT_DOWN) {
			if(collapsed) {
				
				if(collapsed) {
					printf("Dropdown is now uncollapsed.\n");
				}
			}
			else {
				
				printf("Dropdown is now collapsed.\n");
			}
			collapsed = !collapsed;
			needsRedraw = true;
			return true;
		}
	}
	else if(!collapsed) { //If it not collapsed it is allowed to extend beyond its view bounds
		int i = 0 ;
		for(const RIVDropdownElement& element : elements) {
			if(y > element.y && y < element.y + height) {
				
				printf("newly selected index = %d\n",i);
				needsRedraw = true;
				selectedElementIndex = i;
				collapsed = true;
				
				notifyListeners();
				
				return true;
			}
			++i;
		}
	}
	return false;
}
int RIVUIDropdown::GetSelectedIndex() {
	return selectedElementIndex;
}
std::string RIVUIDropdown::GetSelectedValue() {
	return elements[selectedElementIndex].text;
}
void RIVUIDropdown::AddValue(const std::string &value) {
	
}