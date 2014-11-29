//
//  dataconnector.h
//  embree
//
//  Created by Gerard Simons on 10/11/14.
//
//	Simple static data connector that connects EMBREE generated data (rays, intersections) to non-EMBREE components
//

#ifndef embree_dataconnector_h
#define embree_dataconnector_h

#include <vector>

typedef unsigned short ushort;

//Struct wrapper for all intersection related data
typedef struct IntersectData {
	float position[3]; //world position
	float color[3];

	ushort primitiveId;
	ushort shapeId; //TODO: Is this still being used
	ushort lightId;
	ushort interactionType;
	
	IntersectData(float* position, float* color, ushort primitiveId, ushort shapeId, ushort lightId, ushort interactionType) :
		primitiveId(primitiveId), shapeId(shapeId), lightId(lightId), interactionType(interactionType) {
		
		memcpy(this->position, position, 3);
		memcpy(this->color, color, 3);
	}
	
	IntersectData(float x, float y, float z, float r, float g, float b, ushort primitiveId, ushort shapeId, ushort lightId, ushort interactionType) :
	primitiveId(primitiveId), shapeId(shapeId), lightId(lightId), interactionType(interactionType) {
		
		position[0] = x;
		position[1] = y;
		position[2] = z;
		
		color[0] = r;
		color[1] = g;
		color[2] = b;
	}
} IntersectData;

class PathData {
public:
	
	~PathData() {
		
	}
	//The camera sample data
	float imageX; // Continuously sampled image positions
	float imageY;
	float lensU; // Lens parameters
	float lensV;
	float timestamp;
	
	float throughput[3];
	float radiance[3];
	
	//Variable sized (=< MAX_BOUNCE) data about its intersections
	std::vector<IntersectData> intersectionData;
};

class DataConnector {
private:
	bool pathSet = false;
	PathData currentPath;
	
	typedef void (*callback_function)(PathData* newPath); // type for conciseness
	
	callback_function callback;
public:
	//Constructor
	static size_t IdCounter;
	size_t id;
	
	DataConnector(callback_function callback);
	DataConnector* Copy() {
		return new DataConnector(callback);
	}
//	void ProcessPath();
	void FinishPath(unsigned short depth, float r, float g, float b, float throughput_r, float throughput_g, float throughput_b);
	void StartPath(float x, float y, float lensU, float lensV, float time);
//	void set_callback((void) (*newCallBack)(PathData*));
	void AddIntersectionData(float x, float y, float z, float r, float g, float b, int primitive_id, ushort type);
//	void IJustAddedThisStrangeFunction();
};

#endif
