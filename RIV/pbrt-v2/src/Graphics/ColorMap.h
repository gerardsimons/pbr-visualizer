//
//  ColorMap.h
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 08/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_DO_NOT_DELETE__ColorMap__
#define __afstuderen_DO_NOT_DELETE__ColorMap__

//#include <stdio.h>
#include <vector>



//A linearly interpolated color map
class ColorMap {
private:
	std::vector<const float*> colorTable;
public:
	ColorMap() {
		
	}
	ColorMap(std::vector<const float*>& colors) {
		colorTable = colors;
	}
	
	//Methods
	void SetRange(float minRange,float maxRange);
	const float* Color(float ratio);
	void AddColor(float const* newColor);
};

#endif /* defined(__afstuderen_DO_NOT_DELETE__ColorMap__) */
