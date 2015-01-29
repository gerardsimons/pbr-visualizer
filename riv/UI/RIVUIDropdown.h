//
//  RIVUIDropdown.h
//  embree
//
//  Created by Gerard Simons on 27/01/15.
//
//

#ifndef __embree__RIVUIDropdown__
#define __embree__RIVUIDropdown__

#include "RIVUIElement.h"
#include <string>
#include <vector>

class RIVUIDropdownListener;

class RIVUIDropdown : public RIVUIElement {
private:
	
//	typedef bool (*callback)(RIVUIDropdown* source, int selectedIndex, const std::string& selectedText); // type for conciseness
	
	std::vector<RIVUIDropdownListener*> listeners;
	
	class RIVDropdownElement {
	public:
		RIVDropdownElement(int y,const std::string& text) : y(y), text(text) {
			
		}
		int y;
//		int x;
//		int width;
//		int height;
		std::string text;
	};
	
	std::vector<RIVDropdownElement> elements;
	void createDropdownElements(const std::vector<std::string>& texts);
	void notifyListeners();
	
	int selectedElementIndex = 0;
	bool collapsed = true;
public:
	RIVUIDropdown(int x, int y, int width, int height, const std::vector<std::string>& elements);
	void AddListener(RIVUIDropdownListener* listener);
	void SetValues(const std::vector<std::string>& values);
	void SetSelectedValue(const std::string& value);
	int GetSelectedIndex();
	std::string GetSelectedValue();
	void Draw();
	bool Mouse(int button, int state, int x, int y);
};


class RIVUIDropdownListener {
public:
	virtual void OnValueChanged(RIVUIDropdown* source, int selectedIndex, const std::string& selectedText) = 0;
};

#endif /* defined(__embree__RIVUIDropdown__) */
