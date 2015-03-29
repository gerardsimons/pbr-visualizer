//
//  Configuration.h
//  embree
//
//  Created by Gerard Simons on 08/12/14.
//
//

#ifndef embree_Configuration_h
#define embree_Configuration_h

extern const char* DATASET_ONE;
extern const char* DATASET_TWO;

extern const char* INTERSECTIONS_TABLE;
extern const char* PATHS_TABLE;
extern const char* LIGHTS_TABLE;
extern const char* PATH_MEMBERSHIP_TABLE;
extern const char* ISECT_MEMBERSHIP_TABLE;

extern const char* MEMBERSHIP;

//* PATH RECORD NAMES *//
extern const char* RENDERER_ID;
extern const char* PIXEL_X;
extern const char* PIXEL_Y;
extern const char* LENS_U;
extern const char* LENS_V;
extern const char* PATH_R;
extern const char* PATH_G;
extern const char* PATH_B;
extern const char* THROUGHPUT_R;
extern const char* THROUGHPUT_G;
extern const char* THROUGHPUT_B;
extern const char* DEPTH;

//* INTERSECTIONS TABLE RECORD NAMES *//
extern const char* BOUNCE_NR;
extern const char* POS_X;
extern const char* POS_Y;
extern const char* POS_Z;
extern const char* DIR_X;
extern const char* DIR_Y;
extern const char* DIR_Z;
extern const char* INTERSECTION_R;
extern const char* INTERSECTION_G;
extern const char* INTERSECTION_B;
extern const char* PRIMITIVE_ID;
extern const char* INTERACTION_TYPE;
extern const char* SHAPE_ID;
extern const char* OCCLUDER_COUNT;

extern const char* LIGHT_ID;
extern const char* OCCLUDER_ID;
extern const char* LIGHT_R;
extern const char* LIGHT_G;
extern const char* LIGHT_B;

extern const char* IMAGE_RADIANCE;
extern const char* IMAGE_THROUGHPUT;
extern const char* IMAGE_DEPTH;

#endif
