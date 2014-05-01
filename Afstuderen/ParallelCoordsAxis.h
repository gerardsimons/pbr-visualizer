//
//  ParallelCoordsAxis.h
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include <string>
#include "Record.h"

class ParallelCoordsAxis {
public:
    ParallelCoordsAxis(int x,int y,int height,RIVRecord* record);
    int x,y;
    int height;
    RIVRecord* record;
};