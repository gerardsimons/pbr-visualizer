//
//  embree_renderer.h
//  embree
//
//  Created by Gerard Simons on 11/11/14.
//
//

#ifndef embree_embree_renderer_h
#define embree_embree_renderer_h

#include "sys/platform.h"
#include "sys/filename.h"
#include "image/image.h"
#include "lexers/streamfilters.h"
#include "lexers/parsestream.h"
#include "device/loaders/loaders.h"
#include "common/math/affinespace.h"

#include "device_singleray/dataconnector.h"
#include "device_singleray/api/singleray_device.h"

using namespace embree;

class EMBREERenderer {
private:
	/******************************************************************************/
	/*                                  State                                     */
	/******************************************************************************/
	
	/* camera settings */
	Vector3f g_camPos    = Vector3f(0.0f,0.0f,0.0f);
	Vector3f g_camLookAt = Vector3f(1.0f,0.0f,0.0f);
	Vector3f g_camUp     = Vector3f(0,1,0);
	float g_camFieldOfView = 64.0f;
	float g_camRadius = 0.0f;
	
	/* rendering device and global handles */
	Handle<Device::RTRenderer> g_renderer = NULL;
	Handle<Device::RTToneMapper> g_tonemapper = NULL;
	Handle<Device::RTFrameBuffer> g_frameBuffer = NULL;
	Handle<Device::RTImage> g_backplate = NULL;
	Handle<Device::RTScene> g_render_scene = NULL;
	std::vector<Handle<Device::RTPrimitive> > g_prims;
	
	Handle<Device::RTScene> scene = NULL;
	
	/* Data connector */
	DataConnector* dataConnector = NULL;
	
	/* rendering settings */
	std::string g_scene = "default";
	std::string g_accel = "default";
	std::string g_builder = "default";
	std::string g_traverser = "default";
	int g_depth = -1;                       //!< recursion depth
	int g_spp = 1;                          //!< samples per pixel for ordinary rendering
	
	/* output settings */
	int g_numBuffers = 2;                   //!< number of buffers of the framebuffer
	bool g_rendered = false;                //!< set to true after rendering
	int g_refine = 1;                       //!< refinement mode
	float g_gamma = 1.0f;
	bool g_vignetting = false;
	bool g_fullscreen = false;
	size_t g_width = 512;
	size_t g_height = 512;
	std::string g_format = "RGBA8";
	std::string g_rtcore_cfg = "";
	std::string g_outFileName = "";
	size_t g_num_frames = 1; // number of frames to render in output mode
	size_t g_numThreads = 0;
//	size_t g_verbose_output = 0;
	
	/* regression testing mode */
	bool g_regression = false;
	
	
	/* logging settings */
//	bool log_display = false;
//	bool g_profiling = false;
	
	/******************************************************************************/
	/*                            Object Creation                                 */
	/******************************************************************************/
	Handle<Device::RTCamera> createCamera(const AffineSpace3f& space);
	Handle<Device::RTScene> createScene();
	
	void setLight(Handle<Device::RTPrimitive> light);
	void createGlobalObjects();
	void clearGlobalObjects();
public:
	/* Constructor */
//	EMBREERenderer() {
//		
//	};
//	EMBREERenderer(DataConnector* dataConnector);
//	EMBREERenderer(DataConnector* dataConnector, int argc, char **argv);
	EMBREERenderer(DataConnector* dataConnector, const std::string& commandsFile);
	
	std::string makeFileName(const std::string path, const std::string fileName);
	void outputMode(const FileName& fileName);

	//Parser methods
	void parseDebugRenderer(Ref<ParseStream> cin, const FileName& path);
	void parsePathTracer(Ref<ParseStream> cin, const FileName& path);
	void parseCommandLine(Ref<ParseStream> cin, const FileName& path);
	
	void RenderNextFrame();
};

	void someVeryFunnyAndStrangeFunctionThatDoesNothingAtAll();

#endif
