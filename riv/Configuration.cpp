//
//  Configuration.cpp
//  embree
//
//  Created by Gerard Simons on 08/12/14.
//
//

#include "Configuration.h"

const char* DATASET_ONE = "renderer_one";
const char* DATASET_TWO = "renderer_two";

const char* INTERSECTIONS_TABLE = "intersections";
const char* PATHS_TABLE = "paths";
const char* LIGHTS_TABLE = "lights";
const char* PATH_MEMBERSHIP_TABLE = "path_membership";
const char* ISECT_MEMBERSHIP_TABLE = "isect_membership";

//* MEMBERSHIP RECORD NAME *//
const char* MEMBERSHIP = "membership";

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
const char* DIR_X = "x_dir";
const char* DIR_Y = "y_dir";
const char* DIR_Z = "z_dir";
const char* INTERSECTION_R = "R";
const char* INTERSECTION_G = "G";
const char* INTERSECTION_B = "B";
const char* PRIMITIVE_ID = "object ID";
const char* INTERACTION_TYPE = "interaction_type";
const char* SHAPE_ID = "shape ID";
const char* OCCLUDER_COUNT = "occlusions";
const char* GIZMO_ID = "gizmo_id";

//* RECORD NAMES OF THE LIGHTS TABLE *//
const char* LIGHT_ID = "light ID";
const char* OCCLUDER_ID = "occluder ID";
const char* LIGHT_R = "light R";
const char* LIGHT_G = "light G";
const char* LIGHT_B = "light B";

//* 2D DISTRIBUTION NAMES *//
const char* IMAGE_RADIANCE_R = "image_radiance_r";
const char* IMAGE_RADIANCE_G = "image_radiance_g";
const char* IMAGE_RADIANCE_B = "image_radiance_b";
const char* IMAGE_RADIANCE_AVG = "image_radiance_avg";
const char* IMAGE_THROUGHPUT = "image_throughput";
const char* IMAGE_DEPTH = "image_depth";
