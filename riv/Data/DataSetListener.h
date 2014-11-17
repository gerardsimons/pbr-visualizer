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
	//Did some filter get added, removed or altered?
    virtual void OnFiltersChanged() = 0;
	//Did new data get added or old data removed?
	virtual void OnDataChanged() = 0;
};

#endif
