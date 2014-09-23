//
//  MeshModelGroup.h
//  qt_widgets
//
//  Created by Gerard Simons on 22/09/14.
//
//

#ifndef qt_widgets_MeshModelGroup_h
#define qt_widgets_MeshModelGroup_h

#include "MeshModel.h"

class MeshModelGroup {
private:
	std::vector<MeshModel> models;
	void ComputeCenter();
	Vec3Df* center = NULL;
	Vec3Df scale;
public:
	Vec3Df GetCenter();
	Vec3Df GetScale();
	bool ModelIntersect(const riv::Ray<float>& r, ushort& objectIndex, Vec3Df& Phit);
	void CenterAndScaleToUnit();
	std::vector<MeshModel>* GetModels();
	MeshModelGroup(const std::vector<MeshModel>& models);
};

#endif /* defined(__qt_widgets__MeshModelGroup__) */
