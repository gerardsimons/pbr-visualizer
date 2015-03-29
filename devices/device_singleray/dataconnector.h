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
#include "../../common/math/vec3.h"
#include "../../common/math/vec2.h"
#include "../../common/math/color.h"

typedef unsigned short ushort;

using namespace embree;

typedef struct LightData {
    ushort lightId;
    ushort occluderId;
    Color radiance;
} LightData;

//Struct wrapper for all intersection related data
typedef struct IntersectData {
	Vec3fa position; //world position
	Color color;
	Vec3fa dir;

	ushort primitiveId;
	ushort shapeId; //TODO: Is this still being used
	ushort lightId;
	ushort interactionType;
    
    std::vector<LightData> lightData;
	
	IntersectData(const Vec3fa& position, const Vec3fa& dir,const Color& color, ushort primitiveId, ushort shapeId, ushort lightId, ushort interactionType) :
		position(position), dir(dir), color(color), primitiveId(primitiveId), shapeId(shapeId), lightId(lightId), interactionType(interactionType) {
	}
} IntersectData;

class PathData {
public:
	
	~PathData() {
		
	}
	//The camera sample data
	Vec2f pixel;
	Vec2f lens;
	float timestamp;
	ushort depth;
	
	Color throughput;
	Color radiance;
	
	//Variable sized (=< MAX_BOUNCE) data about its intersections
	std::vector<IntersectData> intersectionData;
};

class DataConnector {
private:
	bool pathSet = false;
	PathData currentPath;
    std::vector<LightData> currentLightData;
	
	typedef bool (*path_finished)(PathData* newPath); // type for conciseness
	typedef void (*frame_finished)(size_t numPaths, size_t numRays);
    int nrLights = -1;
	path_finished pfCallback;
	frame_finished ffCallback;
public:
	//Constructor
	static size_t IdCounter;
	size_t id;
	
	DataConnector(path_finished pfCallback, frame_finished ffCallback);
	DataConnector* Copy() {
		DataConnector* copy = new DataConnector(pfCallback,ffCallback);
        copy->SetNrLights(nrLights);
        return copy;
	}
//	void ProcessPath();
    void SetNrLights(int nrLights) {
        this->nrLights = nrLights;
    }
	void FinishFrame(size_t numPaths, size_t numRays);
	bool FinishPath(Color& color, Color& throughput);
	void StartPath(const Vec2f& pixel,const Vec2f& lens, float time);
//	void set_callback((void) (*newCallBack)(PathData*));
	void AddIntersectionData(const Vec3fa& pos, const Vec3fa& dir, Color& color, int primitive_id, ushort type);
    void AddLightData(ushort lightId, ushort occluderId, const Color& color);
};

#endif
