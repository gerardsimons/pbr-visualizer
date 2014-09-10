//
//  ColorPalette.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 08/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ColorPalette.h"

namespace colors {
	ColorMap jetColorMap() {
		ColorMap jetColorMap;
		
		jetColorMap.AddColor(DARK_BLUE);
		jetColorMap.AddColor(BLUE);
		jetColorMap.AddColor(CYAN);
		jetColorMap.AddColor(YELLOW);
		jetColorMap.AddColor(RED);
		jetColorMap.AddColor(DARK_RED);
		
		return jetColorMap;
	}
	
	std::vector<float const*> allColors() {
        std::vector<float const*> allColors;
        
        allColors.push_back(BLACK);
        allColors.push_back(RED);
        allColors.push_back(GREEN);
        allColors.push_back(BLUE);
        allColors.push_back(YELLOW);
        allColors.push_back(CYAN);
        allColors.push_back(PURPLE);
        
        return allColors;
    }
}
