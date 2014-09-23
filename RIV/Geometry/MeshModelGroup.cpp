//
//  MeshModelGroup.cpp
//  qt_widgets
//
//  Created by Gerard Simons on 22/09/14.
//
//

#include "MeshModelGroup.h"

MeshModelGroup::MeshModelGroup(const std::vector<MeshModel>& modelData) {
	center = NULL;
	this->models = modelData;
	CenterAndScaleToUnit();
}

Vec3Df MeshModelGroup::GetCenter() {
	if(center == NULL) {
		ComputeCenter();
	}
	return *center;
}
Vec3Df MeshModelGroup::GetScale() {
	return scale;
}

void MeshModelGroup::ComputeCenter() {
	if(center) {
		delete center;
	}
	center = new Vec3Df(0,0,0);
	for(MeshModel& model : models) {
		model.ComputeCenter();
		*center += model.GetPosition();
	}
	*center /= models.size();
}
//Find the model that has the intersection closes to the ray origin
bool MeshModelGroup::ModelIntersect(const riv::Ray<float>& r, ushort& objectIDResult, Vec3Df& Phit) {
	float bestDistance = -std::numeric_limits<float>::max();
	float distance;
	objectIDResult = 0;
	bool intersects = false;
	for(MeshModel& model : models) {
		size_t triangleIndex;
		if(model.TriangleIntersect(r, triangleIndex, Phit, distance)) {
			if(distance > bestDistance) {
				bestDistance = distance;
				objectIDResult = model.GetObjectID();
				intersects = true;
			}
		}
	}
	return intersects;
}
std::vector<MeshModel>* MeshModelGroup::GetModels() {
	return &models;
}
void MeshModelGroup::CenterAndScaleToUnit() {
	if(!center) {
		ComputeCenter();
	}
	float maxDistance = 0;
	//Find the maximum distance
	for(MeshModel& model : models) {
		for(const riv::TriangleMesh& mesh : model.GetMeshes()) {
			Vec3Df position = model.GetPosition();
			for (size_t i = 0; i < mesh.vertices.size (); i+=3){
				float dX= (mesh.vertices[i]-position[0]);
				float dY= (mesh.vertices[i+1]-position[1]);
				float dZ= (mesh.vertices[i+2]-position[2]);
				float distance = sqrt(dX*dX+dY*dY+dZ*dZ);
//				printf("distance=%f\n",distance);
				if (distance > maxDistance)
					maxDistance = distance ;
			}
		}
	}
	//Translate to center and scale by max distance
	float s = 1.F / maxDistance;
	
	scale = Vec3Df(s,s,s);
	
	printf("Model scale = %f\n",s);
}