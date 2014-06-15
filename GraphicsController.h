//
//  GraphicsProperty.h
//  Afstuderen
//
//  Created by Gerard Simons on 13/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__GraphicsProperty__
#define __Afstuderen__GraphicsProperty__

#include "ColorProperty.h"

typedef struct RIVGraphics {
    float* color;
    float size;
};

//Determines graphical attributes for a given table's row
class RIVGraphicsController {
private:
    RIVColorProperty* colorProperty;
//    RIVDimensionProperty* dimProperty;
public:
    RIVGraphics GetGraphics(const size_t& row, RIVTable* table);
};

#endif /* defined(__Afstuderen__GraphicsProperty__) */
