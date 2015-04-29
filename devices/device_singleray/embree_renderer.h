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

	
	Handle<Device::RTCamera> camera = NULL;
	
	/* raw shapes data derived from g_prims */
	std::vector<Shape*> rawShapes;
	
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
	int g_numBuffers = 1;                   //!< number of buffers of the framebuffer
	bool g_rendered = false;                //!< set to true after rendering
	int g_refine = 1;                       //!< refinement mode
	float g_gamma = 1.0f;
	bool g_vignetting = false;
	bool g_fullscreen = false;
	size_t g_width = 256;
	size_t g_height = 256;
	std::string g_format = "RGB8";
	std::string g_rtcore_cfg = "";
	std::string g_outFileName = "";
	size_t g_num_frames = 1; // number of frames to render in output mode
	size_t g_numThreads = 1;
//	size_t g_verbose_output = 0;
	
	/* regression testing mode */
	bool g_regression = false;
	
	
	/* logging settings */
//	bool log_display = false;
//	bool g_profiling = false;
	
	/******************************************************************************/
	/*                            Object Creation                                 */
	/******************************************************************************/

	
	void setLight(Handle<Device::RTPrimitive> light);
	void createGlobalObjects();
	void clearGlobalObjects();
	
	void displayFunc();
public:
    std::vector<Handle<Device::RTPrimitive> > g_prims;
    Handle<Device::RTScene> g_render_scene = NULL;
    SingleRayDevice* g_single_device = NULL;
    
	Handle<Device::RTCamera> createCamera(const AffineSpace3f& space);
	void createScene();
	
	int getWidth();
	int getHeight();
	int getSamplesPerPixel();
	
    EMBREERenderer(DataConnector* dataConnector, const std::string& commandsFile,ushort numThreads);
    EMBREERenderer(const std::string& commandsFile);
    
    //Add a untextured shape to the singleray device
    void AddShape(const std::vector<Vec3f>& positions,const std::vector<Vec3f>& faces);

	std::string makeFileName(const std::string path, const std::string fileName);
	void outputMode(const std::string& fileName);
	void outputMode(const FileName& fileName);
    void outputMode(const char* fileName);
	
	Handle<Device::RTScene> GetScene();
	std::vector<Handle<Device::RTPrimitive>>* GetPrimitives() { return &g_prims; };
	std::vector<Shape*>* GetShapes();

    Vector3f GetCameraPosition();
    Vector3f GetCameraLookAt();
    int GetDepth();
    
	//Parser methods
	void parseDebugRenderer(Ref<ParseStream> cin, const FileName& path);
	void parsePathTracer(Ref<ParseStream> cin, const FileName& path);
	void parseCommandLine(Ref<ParseStream> cin, const FileName& path);
	
    void RenderNextFrame(Histogram2D<float>* pixelHistogram,bool datacallback = true);
	void RenderNextFrame(bool datacallback = true);
    
	std::string GetFormat() { return g_format; };
	Vec2<size_t> GetDimensions();
	
//	static EMBREERenderer* instance;
	static void displayFuncAccess(void);
	
	//Frame buffer mapping and umapping
	void* MapFrameBuffer();
	void UnmapFrameBuffer();
	
	bool RayPick(Ray& ray, float& x, float& y, float& z);
    
    Ref<SwapChain> GetSwapChain();
    void SetSwapChainWeight(float weight);
    void CopySwapChainTo(EMBREERenderer* renderer,float copyWeight);
    void CopySwapChainTo(EMBREERenderer* targetRenderer, const std::vector<std::vector<float>>& weights);
    void CopySwapChainTo(EMBREERenderer* targetRenderer, Histogram2D<float>* histogram, float weightScalar, bool inverse = false);
    
    std::vector<Ref<embree::Light>> GetLights();
    ushort GetNumLights();
};

#endif
