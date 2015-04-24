//
//  Gizmo.cpp
//  embree
//
//  Created by Gerard Simons on 23/04/15.
//
//

#include "Gizmo.h"

Gizmo::Gizmo(const TriangleMeshGroup& shape) : shape(shape) {
    //Nothing to do
}

Gizmo::operator bool() const {
    return shape.NumberOfMeshes();
}

