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

#endif
