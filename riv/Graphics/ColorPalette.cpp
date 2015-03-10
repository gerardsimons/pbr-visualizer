//
//  ColorPalette.cpp
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 08/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ColorPalette.h"

	namespace colors {
		riv::ColorMap jetColorMap() {
			riv::ColorMap jetColorMap;
			
			jetColorMap.AddColor(DARK_BLUE);
			jetColorMap.AddColor(BLUE);
			jetColorMap.AddColor(CYAN);
			jetColorMap.AddColor(YELLOW);
			jetColorMap.AddColor(RED);
			jetColorMap.AddColor(DARK_RED);
			
			return jetColorMap;
		}
        
        riv::ColorMap greenGrayPurpleColorMap() {
            riv::ColorMap greenGrayPurpleColorMap;
            
//            greenGrayPurpleColorMap.AddColor(riv::Color(0.498F,0.749F,0.765F));
//            greenGrayPurpleColorMap.AddColor(riv::Color(0.969F,0.969F,0.969F));
//            greenGrayPurpleColorMap.AddColor(riv::Color(.686F,.552F,.765F)); //PURPLE
            
            greenGrayPurpleColorMap.AddColor(riv::Color(0.F,1.F,0.F));
            greenGrayPurpleColorMap.AddColor(riv::Color(0.969F,0.969F,0.969F));
            greenGrayPurpleColorMap.AddColor(riv::Color(1.F,0.F,1.F)); //PURPLE
            
            return greenGrayPurpleColorMap;
        }
        
        riv::ColorMap brownColorMap() {
            
            riv::ColorMap browns;
            
            browns.AddColor(riv::Color(1,0.968,0.737));
                        browns.AddColor(riv::Color(0.999,0.768,0.31));
                        browns.AddColor(riv::Color(0.851,0.373,0.0054));
            return browns;
        }
        
		std::vector<riv::Color> allColors() {
			std::vector<riv::Color> allColors;
			
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

