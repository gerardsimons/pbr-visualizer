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


	//Struct wrapper for all intersection related data
	typedef struct IntersectData {
		float position[3]; //world position
		float color[3];
//		Spectrum throughput; //What was the throughput up until this point?
		unsigned short primitiveId;
		unsigned short shapeId;
		unsigned short lightId;
		unsigned short interactionType;
	} IntersectData;
	
	class PathData {
	public:
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
	static int ID_COUNTER;
	
	int dc_id;
	PathData* currentPath;
//	(void) (*callback)(PathData* a) = NULL;
public:
	static void ProcessPath();
	void StartPath();
//	void set_callback((void) (*newCallBack)(PathData*));
	void AddIntersectionData(float x, float y, float z, float r, float g, float b, int primitive_id);
};

#endif
