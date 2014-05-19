//
//  ParallelCoordsAxisGroup.h
//  Afstuderen
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__ParallelCoordsAxisGroup__
#define __Afstuderen__ParallelCoordsAxisGroup__

#include "ParallelCoordsAxis.h"
#include "Table.h"
#include <vector>

class ParallelCoordsAxisGroup {
public:
    std::vector<ParallelCoordsAxis> axes;
    RIVTable* table;
};

#endif /* defined(__Afstuderen__ParallelCoordsAxisGroup__) */