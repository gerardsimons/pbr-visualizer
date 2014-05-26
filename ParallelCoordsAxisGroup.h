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
    
    ParallelCoordsAxisGroup* connectedGroup = 0;
    ParallelCoordsAxis* connectorAxis;
    
    ParallelCoordsAxis* LastAxis() {
        if(axes.size() > 0) {
            return &axes[axes.size() - 1];
        }
        else {
            printf("Axis group is empty.");
            return 0;
        }
    }
    
    ParallelCoordsAxis* FirstAxis() {
        if(axes.size() > 0) {
            return &axes[0];
        }
        else {
            printf("Axis group is empty.");
            return 0;
        }
    }
};

#endif /* defined(__Afstuderen__ParallelCoordsAxisGroup__) */