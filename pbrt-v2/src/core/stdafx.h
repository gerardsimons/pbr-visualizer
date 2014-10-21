// header file for building precompiled headers under windows
// a no-op on other architectures
 
#if defined(_MSC_VER)

#include "../core/pbrt.h"
#include "../core/camera.h"
#include "../core/scene.h"
#include "../core/imageio.h"
#include "../core/intersection.h"
#include "../core/montecarlo.h"
#include "../core/sampler.h"
#include "../core/texture.h"
#include "integrator.h"

#endif // _MSC_VER
