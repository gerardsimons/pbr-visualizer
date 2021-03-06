//
//  Gizmo.h
//  embree
//
//  Created by Gerard Simons on 23/04/15.
//
//

#ifndef __embree__Gizmo__
#define __embree__Gizmo__

#include "Geometry/TriangleMeshGroup.h"
#include "Data/Filter.h"
#include "Graphics/ColorMap.h"

#include <vector>

class Gizmo {
public:
    TriangleMeshGroup shape;
    riv::Color color;
    std::vector<riv::RowFilter*> activeFilters;
    
    std::vector<Vec3fa> hitpoints;
    std::vector<riv::Color> colors;
    
    ushort gizmoId = 0;
    
    operator bool() const;
    
    Gizmo() { };
    Gizmo(const TriangleMeshGroup& shape, ushort gizmoId, const riv::Color& color);
    Gizmo(const Gizmo& copy);
};

#endif /* defined(__embree__Gizmo__) */
