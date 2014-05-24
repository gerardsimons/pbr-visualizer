//
//  helper.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 16/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "helper.h"

float* linearInterpolateColor(float ratio, const float colorOne[3],const float colorTwo[3]) {
    float color[3];
    for(int i = 0 ; i < 3 ; i++) {
        color[i] = ratio * colorOne[i] + (1-ratio) * colorTwo[i];
    }
    return color;
}