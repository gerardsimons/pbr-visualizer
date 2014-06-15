//
//  GraphicsProperty.h
//  Afstuderen
//
//  Created by Gerard Simons on 15/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef GRAPHICS_PROPERTY
#define GRAPHICS_PROPERTY

#include "Interpolator.h"
#include "Table.h"
#include "helper.h"

class RIVTable;

enum INTERPOLATION_SCHEME {
    DISCRETE,
    CONTINUOUS
};

class RIVProperty {
protected:
public:
    virtual bool Value(RIVTable*,const size_t&,float& value) = 0;
};

class RIVInterpolatedProperty {
protected:
    //The table whose index defines the value of the property
    RIVTable* propertyReference;
    
    const INTERPOLATION_SCHEME defaultInterpolationMode = CONTINUOUS;
    
    //The type of interpolation used by the default interpolator, default is continuous
    INTERPOLATION_SCHEME interpolationMode;
    
    //The default interpolator to be used, should be defined
    Interpolator<size_t>* defaultInterpolator;
    
    //Any optional special non-default interpolators to be used
    std::vector<Interpolator<size_t>*> specialInterpolators;
    
    //Maps an index to a special interpolator, if none is found, the default color interpolator is used
    std::map<size_t,Interpolator<size_t>*> interpolatorMapping;
    
    //helper function, shared by constructors to avoid duplicate code
    void init(RIVTable *table, const INTERPOLATION_SCHEME& scheme, const std::vector<size_t>& interpolationValues);

//    float const* colorForMultipleResolvedRows(const std::vector<size_t>& rows);
public:
    ~RIVInterpolatedProperty() {
        deletePointerVector(specialInterpolators);
        interpolatorMapping.clear();
    }
    //Uses a continuous interpolator
    RIVInterpolatedProperty(RIVTable *colorReference_);
    RIVInterpolatedProperty(RIVTable *colorReference_,std::vector<size_t>& interpolationValues, const INTERPOLATION_SCHEME& scheme);
    RIVInterpolatedProperty(RIVTable *colorReference_,std::vector<size_t>& interpolationValues);
    void AddInterpolationScheme(std::vector<size_t>& indices, Interpolator<size_t>* newInterpolator);
    bool Value(RIVTable* sourceTable, const size_t& row, float& computedValue);
    bool Value(RIVTable* sourceTable, const std::vector<size_t>& rows,float& value);
};

#endif /* defined(__Afstuderen__GraphicsProperty__) */
