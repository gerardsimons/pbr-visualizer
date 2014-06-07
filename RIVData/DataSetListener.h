//
//  Header.h
//  Afstuderen
//
//  Created by Gerard Simons on 02/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef Afstuderen_Header_h
#define Afstuderen_Header_h

#include "Filter.h"

class RIVDataSetListener {
public:
    virtual void OnDataSetChanged() = 0;
};

#endif
