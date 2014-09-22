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

		const float BLACK[] = {0,0,0};
		const float RED[] = {1.F,0.F,0.F};
		const float GREEN[] = {0.F,1.F,0.F};
		const float BLUE[] =  {0.F,0.F,1.F};
		const float YELLOW[] =  {1.F,1.F,0.F};
		const float CYAN[] = {0.F,1.F,1.F};
		const float PURPLE[] = {1.F,0.F,1.F};
		const float LIGHT_BLUE[] = {0.317F,.553F, .741F,.5F};
		const float DARK_RED[] = {0.5F,0,0};
		const float DARK_BLUE[] = {0,0,0.5F};
		
		std::vector<float const*> allColors();
		ColorMap jetColorMap();
	}

#endif
