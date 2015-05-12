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

template<typename...Ts>
class RIVDataSet;

class RIVDataSetListener {
public:
	//Did some filter get added, removed or altered?
    virtual void OnFiltersChanged(RIVDataSet<float,ushort>* source) = 0;
	//Did new data get added or old data removed?
	virtual void OnDataChanged(RIVDataSet<float,ushort>* source) = 0;
    //The structure of the data changed, tables or records added or removed
    virtual void OnDataStructureChanged(RIVDataSet<float,ushort>* source) {
        
    }
};

#endif
