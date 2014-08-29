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

namespace colors {

    const float BLACK[] = {0,0,0};
    const float RED[] = {1.F,0.F,0.F};
    const float GREEN[] = {0.F,1.F,0.F};
    const float BLUE[] =  {0.F,0.F,1.F};
    const float YELLOW[] =  {1.F,1.F,0.F};
    const float CYAN[] = {0.F,1.F,1.F};
    const float PURPLE[] = {1.F,0.F,1.F};
    
    const float DARK_RED[] = {0.5F,0,0};
    const float DARK_BLUE[] = {0,0,0.5F};
    
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
    
    std::vector<const float*> jetColorMap() {
        std::vector<const float*> jetColorMap;
        
        jetColorMap.push_back(DARK_BLUE);
        jetColorMap.push_back(BLUE);
        jetColorMap.push_back(CYAN);
        jetColorMap.push_back(YELLOW);
        jetColorMap.push_back(RED);
        jetColorMap.push_back(DARK_RED);
        
        return jetColorMap;
    }
}

#endif
