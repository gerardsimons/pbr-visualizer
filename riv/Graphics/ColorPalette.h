//
//  ColorPallete.h
//  Afstuderen
//
//  Created by Gerard Simons on 10/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef Afstuderen_ColorPallete_h
#define Afstuderen_ColorPallete_h

#include <vector>
#include "ColorMap.h"

	namespace colors {

		const Color BLACK = {0,0,0};
		const Color RED = {1.F,0.F,0.F};
		const Color GREEN = {0.F,1.F,0.F};
		const Color BLUE =  {0.F,0.F,1.F};
		const Color YELLOW =  {1.F,1.F,0.F};
		const Color CYAN = {0.F,1.F,1.F};
		const Color PURPLE = {1.F,0.F,1.F};
		const Color LIGHT_BLUE = {0.317F,.553F, .741F};
		const Color DARK_RED = {0.5F,0,0};
		const Color DARK_BLUE = {0,0,0.5F};
		
		std::vector<Color> allColors();
		ColorMap jetColorMap();
	}

#endif
