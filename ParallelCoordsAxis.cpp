//
//  ParallelCoordsAxis.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ParallelCoordsAxis.h"

ParallelCoordsAxis::ParallelCoordsAxis(int x,int y, int height, RIVRecord* record) {
    this->x = x;
    this->y = y;
    this->height = height;
    this->record = record;
}