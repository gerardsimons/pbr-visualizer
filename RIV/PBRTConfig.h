//
//  PBRTConfig.h
//  qt_widgets
//
//  Created by Gerard Simons on 22/09/14.
//
//

#ifndef __qt_widgets__PBRTConfig__
#define __qt_widgets__PBRTConfig__

#include <vector>
#include "Geometry/MeshModel.h"
#include "Geometry/MeshModelGroup.h"

class PBRTConfig {
private:
	MeshModelGroup objects;
public:
	PBRTConfig(MeshModelGroup objects_) : objects(objects_){

	}
	MeshModelGroup* GetMeshModelGroup() {
		return &objects;
	}
};


#endif /* defined(__qt_widgets__PBRTConfig__) */
