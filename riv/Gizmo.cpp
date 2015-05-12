//
//  Gizmo.cpp
//  embree
//
//  Created by Gerard Simons on 23/04/15.
//
//

#include "Gizmo.h"

Gizmo::Gizmo(const TriangleMeshGroup& shape, ushort gizmoId, const riv::Color& color) : shape(shape), gizmoId(gizmoId), color(color) {
    //Nothing to do
}

Gizmo::operator bool() const {
    return shape.NumberOfMeshes();
}

Gizmo::Gizmo(const Gizmo& copy) : color(copy.color), gizmoId(copy.gizmoId+1), shape(copy.shape) {
    
}

