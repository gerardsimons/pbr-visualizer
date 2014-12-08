//
//  Configuration.h
//  embree
//
//  Created by Gerard Simons on 08/12/14.
//
//

#ifndef embree_Configuration_h
#define embree_Configuration_h

const char* INTERSECTIONS_TABLE = "intersections";
const char* PATHS_TABLE = "paths";

//* PATH RECORD NAMES *//
const char* RENDERER_ID = "renderer";
const char* PIXEL_X = "pixel_x";
const char* PIXEL_Y = "pixel_y";
const char* LENS_U = "lens U";
const char* LENS_V = "lens V";
const char* PATH_R = "radiance R";
const char* PATH_G = "radiance G";
const char* PATH_B = "radiance B";
const char* THROUGHPUT_R = "throughput R";
const char* THROUGHPUT_G = "throughput G";
const char* THROUGHPUT_B = "throughput B";
const char* DEPTH = "depth";

//* INTERSECTIONS TABLE RECORD NAMES *//
const char* BOUNCE_NR = "bounce_nr";
const char* POS_X = "x";
const char* POS_Y = "y";
const char* POS_Z = "z";
const char* INTERSECTION_R = "R";
const char* INTERSECTION_G = "G";
const char* INTERSECTION_B = "B";
const char* PRIMITIVE_ID = "primitive ID";
const char* INTERACTION_TYPE = "interaction_type";
const char* SHAPE_ID = "shape ID";
const char* LIGHT_ID = "light ID";


#endif
