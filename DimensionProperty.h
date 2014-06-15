//
//  DimensionProperty.h
//  Afstuderen
//
//  Created by Gerard Simons on 14/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__DimensionProperty__
#define __Afstuderen__DimensionProperty__

#include "Table.h"

//Determines the size or dimension of a given row for a given table. It may be based on a
class RIVDimensionsProperty {
    
    RIVTable* referenceTable;
    
    virtual float Dimension(const size_t& row, RIVTable* sourceTable) = 0;
};

//Maps certain rows to fixed discrete and pre-determined sizes
class RIVDiscreteDimensionsProperty {
    //Any special non-default interpolators to be used (i.e. for clusters)
    std::vector<Interpolator<size_t>*> specialInterpolators;
    
    //Maps an index to a special interpolator, if none is found, the default color interpolator is used
    std::map<size_t,Interpolator<size_t>*> interpolatorMapping;
    
    
//    void AddMapping(const size_t& row, )
    float Dimension(const size_t& row, RIVTable* sourceTable);
};

#endif /* defined(__Afstuderen__DimensionProperty__) */
