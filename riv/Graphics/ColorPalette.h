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

		const riv::Color BLACK = {0,0,0};
		const riv::Color RED = {1.F,0.F,0.F};
		const riv::Color GREEN = {0.F,1.F,0.F};
		const riv::Color BLUE =  {0.F,0.F,1.F};
		const riv::Color YELLOW =  {1.F,1.F,0.F};
		const riv::Color CYAN = {0.F,1.F,1.F};
		const riv::Color PURPLE = {1.F,0.F,1.F};
		const riv::Color LIGHT_BLUE = {0.317F,.553F, .741F};
		const riv::Color DARK_RED = {0.5F,0,0};
		const riv::Color DARK_BLUE = {0,0,0.5F};
		
		std::vector<riv::Color> allColors();
		riv::ColorMap jetColorMap();
        riv::ColorMap redGrayBlueColorMap();
        riv::ColorMap greenGrayPurpleColorMap();
        riv::ColorMap brownColorMap();
	}

#endif
