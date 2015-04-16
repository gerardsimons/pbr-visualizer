// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "embree_renderer.h"
#include "devices/device_singleray/shapes/trianglemesh_normals.h"
#include "../../riv/helper.h"

#include <GLUT/GLUT.h>

//#include "api/instance.h"
//#include "api/handle.h"

using namespace embree;

//EMBREERenderer* EMBREERenderer::instance = NULL;

/******************************************************************************/
/*                            Object Creation                                 */
/******************************************************************************/
Handle<Device::RTCamera> EMBREERenderer::createCamera(const AffineSpace3f& space)
{
	/*! pinhole camera */
	if (g_camRadius == 0.0f)
	{
		Handle<Device::RTCamera> camera = g_device->rtNewCamera("pinhole");
		g_device->rtSetTransform(camera, "local2world", copyToArray(space));
		g_device->rtSetFloat1(camera, "angle", g_camFieldOfView);
		g_device->rtSetFloat1(camera, "aspectRatio", float(g_width) / float(g_height));
		g_device->rtCommit(camera);
		return camera;
	}
	/*! depth of field camera */
	else
	{
		Handle<Device::RTCamera> camera = g_device->rtNewCamera("depthoffield");
		g_device->rtSetTransform(camera, "local2world", copyToArray(space));
		g_device->rtSetFloat1(camera, "angle", g_camFieldOfView);
		g_device->rtSetFloat1(camera, "aspectRatio", float(g_width) / float(g_height));
		g_device->rtSetFloat1(camera, "lensRadius", g_camRadius);
		g_device->rtSetFloat1(camera, "focalDistance", length(g_camLookAt - g_camPos));
		g_device->rtCommit(camera);
		return camera;
	}
}

void EMBREERenderer::createScene()
{
	g_render_scene = g_device->rtNewScene(g_scene.c_str());
	g_device->rtSetString(g_render_scene,"accel",g_accel.c_str());
	g_device->rtSetString(g_render_scene,"builder",g_builder.c_str());
	g_device->rtSetString(g_render_scene,"traverser",g_traverser.c_str());
	for (size_t i=0; i<g_prims.size(); i++) {
		g_device->rtSetPrimitive(g_render_scene,i,g_prims[i]);
	}
	g_device->rtCommit(g_render_scene);
}

Handle<Device::RTScene> EMBREERenderer::GetScene() {
	return g_render_scene;
}

void EMBREERenderer::setLight(Handle<Device::RTPrimitive> light)
{
	if (!g_render_scene) return;
	g_device->rtSetPrimitive(g_render_scene,g_prims.size(),light);
	g_device->rtCommit(g_render_scene);
}

void EMBREERenderer::createGlobalObjects()
{
//	if(!g_renderer) {
//		g_renderer = g_device->rtNewRenderer("pathtracer");
//		if (g_depth >= 0) g_device->rtSetInt1(g_renderer, "maxDepth", g_depth);
//		g_device->rtSetInt1(g_renderer, "sampler.spp", g_spp);
//		g_device->rtCommit(g_renderer);
//	}
//	
	g_tonemapper = g_device->rtNewToneMapper("default");
	g_device->rtSetFloat1(g_tonemapper, "gamma", g_gamma);
	g_device->rtSetBool1(g_tonemapper, "vignetting", g_vignetting);
	g_device->rtCommit(g_tonemapper);
//
	g_frameBuffer = g_device->rtNewFrameBuffer(g_format.c_str(), g_width, g_height, g_numBuffers);
	g_backplate = NULL;
}

void EMBREERenderer::clearGlobalObjects() {
	g_renderer = null;
	g_tonemapper = null;
	g_frameBuffer = null;
	g_backplate = null;
	g_prims.clear();
	g_render_scene = null;
	rtClearTextureCache();
	rtClearImageCache();
//	delete g_device;
//	g_device = NULL;
}

std::vector<Shape*>* EMBREERenderer::GetShapes() {
	return &rawShapes;
}
Vector3f EMBREERenderer::GetCameraPosition() {
    return g_camPos;
}
Vector3f EMBREERenderer::GetCameraLookAt() {
    return g_camLookAt;
}
std::string EMBREERenderer::makeFileName(const std::string path, const std::string fileName)
{
	if (fileName[0] == '/') return fileName;
	if (path == "") return fileName;
	return path+"/"+fileName;
}
void EMBREERenderer::outputMode(const FileName& fileName)
{
	if (!g_renderer) throw std::runtime_error("no renderer set");
	
//	/* render image */
//	Handle<Device::RTCamera> camera = createCamera(AffineSpace3f::lookAtPoint(g_camPos, g_camLookAt, g_camUp));
//	Handle<Device::RTScene> scene = createScene();
	
//	g_device->rtSetInt1(g_renderer, "showprogress", 1);
//	g_device->rtCommit(g_renderer);
//	for (size_t i=0; i<g_num_frames; i++)
//		g_device->rtRenderFrame(g_renderer, camera, g_render_scene, g_tonemapper, g_frameBuffer, 0);
//	for (int i=0; i<g_numBuffers; i++)
//		g_device->rtSwapBuffers(g_frameBuffer);
	
	/* store to disk */
	void* ptr = g_device->rtMapFrameBuffer(g_frameBuffer,0);
	Ref<Image> image = null;
	if      (g_format == "RGB8"        )  image = new Image3c(g_width, g_height, (Col3c*)ptr);
	else if (g_format == "RGBA8"       )  image = new Image4c(g_width, g_height, (Col4c*)ptr);
	else if (g_format == "RGB_FLOAT32" )  image = new Image3f(g_width, g_height, (Col3f*)ptr);
	else if (g_format == "RGBA_FLOAT32")  image = new Image4f(g_width, g_height, (Col4f*)ptr);
	else throw std::runtime_error("unsupported framebuffer format: "+g_format);
	storeImage(image, fileName);
	g_device->rtUnmapFrameBuffer(g_frameBuffer,0);
	g_rendered = true;
}
void EMBREERenderer::outputMode(const std::string& fileName) {
	outputMode(FileName(fileName));
}
Vec2<size_t> EMBREERenderer::GetDimensions() {
	return Vec2<size_t>(g_width,g_height);
}
void EMBREERenderer::parseDebugRenderer(Ref<ParseStream> cin, const FileName& path)
{
	g_renderer = g_device->rtNewRenderer("debug");
	if (g_depth >= 0) g_device->rtSetInt1(g_renderer, "maxDepth", g_depth);
	g_device->rtSetInt1(g_renderer, "sampler.spp", g_spp);
	
	if (cin->peek() != "{") goto finish;
	cin->drop();
	
	while (cin->peek() != "}") {
		std::string tag = cin->getString();
		cin->force("=");
		if (tag == "depth") g_device->rtSetInt1(g_renderer, "maxDepth", cin->getInt());
		else std::cout << "unknown tag \"" << tag << "\" in debug renderer parsing" << std::endl;
	}
	cin->drop();
	
finish:
	g_device->rtCommit(g_renderer);
}

void EMBREERenderer::parsePathTracer(Ref<ParseStream> cin, const FileName& path)
{
	g_renderer = g_device->rtNewRenderer("pathtracer");
	if (g_depth >= 0) g_device->rtSetInt1(g_renderer, "maxDepth", g_depth);
	g_device->rtSetInt1(g_renderer, "sampler.spp", g_spp);
	if (g_backplate) g_device->rtSetImage(g_renderer, "backplate", g_backplate);
	
	if (cin->peek() != "{") goto finish;
	cin->drop();
	
	while (cin->peek() != "}") {
		std::string tag = cin->getString();
		cin->force("=");
        if      (tag == "depth"          ) {
            int depth = cin->getInt();
            g_depth = depth;
            g_device->rtSetInt1  (g_renderer, "maxDepth"       , depth  );
        }
		else if (tag == "spp"            ) g_device->rtSetInt1  (g_renderer, "sampler.spp"    , cin->getInt()  );
		else if (tag == "minContribution") g_device->rtSetFloat1(g_renderer, "minContribution", cin->getFloat());
		else if (tag == "backplate"      ) g_device->rtSetImage (g_renderer, "backplate", rtLoadImage(path + cin->getFileName()));
		else std::cout << "unknown tag \"" << tag << "\" in debug renderer parsing" << std::endl;
	}
	cin->drop();
	
finish:
	g_device->rtCommit(g_renderer);
}

/* Constructor */
EMBREERenderer::EMBREERenderer(DataConnector* dataConnector, const std::string& commandsFile, ushort numThreads)
{
	this->dataConnector = dataConnector;
	
	clearGlobalObjects();
	
	//We do not use Device::createDevice because it depends on the dynamic library, which makes things harder
	//More threads than one does not work

	g_device = new SingleRayDevice(numThreads,g_rtcore_cfg.c_str());
	g_device->SetDataConnector(dataConnector);
	
	g_single_device = dynamic_cast<SingleRayDevice*>(g_device);
	
	/*! create stream for parsing */
	FileName file = FileName() + commandsFile;
	parseCommandLine(new ParseStream(new LineCommentFilter(file, "#")), file.path());
	
	createGlobalObjects();
	
	createScene();
	
	//Create camera
	AffineSpace3f g_camSpace = AffineSpace3f::lookAtPoint(g_camPos, g_camLookAt, g_camUp);
	camera = createCamera(AffineSpace3f(g_camSpace.l,g_camSpace.p));
	
	for(Handle<Device::RTPrimitive> prim : g_prims) {
		Shape* shape = g_single_device->rtGetShape(prim);
		if(shape) {
			rawShapes.push_back(shape);
		}
	}
}
EMBREERenderer::EMBREERenderer(const std::string& commandsFile)
{
    
    clearGlobalObjects();
    
    //We do not use Device::createDevice because it depends on the dynamic library, which makes things harder
    //More threads than one does not work
    
    g_device = new SingleRayDevice(0,g_rtcore_cfg.c_str());
    g_single_device = dynamic_cast<SingleRayDevice*>(g_device);
    
    /*! create stream for parsing */
    FileName file = FileName() + commandsFile;
    parseCommandLine(new ParseStream(new LineCommentFilter(file, "#")), file.path());
    
    createGlobalObjects();
    
    createScene();
    
    //Create camera
    AffineSpace3f g_camSpace = AffineSpace3f::lookAtPoint(g_camPos, g_camLookAt, g_camUp);
    camera = createCamera(AffineSpace3f(g_camSpace.l,g_camSpace.p));
}
ushort EMBREERenderer::GetNumLights() {
    return g_single_device->rtGetLights(g_render_scene).size();
}
std::vector<Ref<embree::Light>> EMBREERenderer::GetLights() {
    return g_single_device->rtGetLights(g_render_scene);
}
void EMBREERenderer::RenderNextFrame() {
	g_device = g_single_device;
	g_device->rtRenderFrame(g_renderer,camera,g_render_scene,g_tonemapper,g_frameBuffer,1);
//	g_device->rtSwapBuffers(g_frameBuffer);
}
//Render next frame according to given pixel distribution
void EMBREERenderer::RenderNextFrame(Histogram2D<float>* pixelDistribution) {
    g_device = g_single_device;
    g_single_device->rtRenderFrame(g_renderer,camera,g_render_scene,g_tonemapper,g_frameBuffer,1,pixelDistribution);
    
    //If you use double buffers,, the user guided rendering (which only effects an area) will cause the other areas to be of two distinct states (e.g. it flips)
//    g_single_device->rtSwapBuffers(g_frameBuffer);
}
bool EMBREERenderer::RayPick(Ray& ray, float& x, float& y, float& z) {
	return g_single_device->rtPick(g_render_scene, ray, x, y, z);
}
Ref<SwapChain> EMBREERenderer::GetSwapChain() {
    return g_single_device->rtGetSwapChain(g_frameBuffer);
}
void EMBREERenderer::CopySwapChainTo(EMBREERenderer* targetRenderer, float weight) {
    SwapChain* thisSwapChain = g_single_device->rtGetSwapChain(g_frameBuffer).ptr;
    SwapChain* thatSwapChain = targetRenderer->GetSwapChain().ptr;
    
    Ref<FrameBuffer> thisBuffer = thisSwapChain->buffer();
    Ref<FrameBuffer> thatBuffer = thatSwapChain->buffer();
    
//    thatBuffer.ptr = thisBuffer.ptr;
//    thatSwapChain.ptr = thisSwapChain.ptr;

    //TODO: Support varying sizes, especially those with the same aspect ratio
    if(thisSwapChain->getWidth() != thatSwapChain->getWidth() || thisSwapChain->getHeight() != thatSwapChain->getHeight()) {
        printf("Sorry, different swapchain sizes not currently supported\n");
    }
    else {
        //Get the accumulation buffers
        AccuBuffer* thisAccuBuffer = thisSwapChain->accu().ptr;
        AccuBuffer* thatAccuBuffer = thatSwapChain->accu().ptr;
        thatSwapChain->clearAll();
        for(int x = 0 ; x < thisSwapChain->getWidth() ; ++x) {
            for(int y = 0 ; y < thisSwapChain->getHeight() ; ++y) {
                Vec4f thisPixel = thisAccuBuffer->getRaw(x, y);
//                Color thisColor = Color(thisPixel.x,thisPixel.y,thisPixel.z);
                Color thisColor = thisAccuBuffer->get(x, y);
//                Color thisColor = thisAccuBuffer->get(x, y);
                float diffWeight = thisPixel.w / weight;
                thisPixel.x /= diffWeight;
                thisPixel.y /= diffWeight;
                thisPixel.z /= diffWeight;
                thisPixel.w = weight;
                
//                thatAccuBuffer->update(x, y, thisColor, weight, true);
                thatAccuBuffer->set(x, y, thisPixel);
                thatBuffer->set(x, y, thisColor);
//                thatAccuBuffer->set(x, y, thisPixel);
            }
        }
    }
}
void EMBREERenderer::CopySwapChainTo(EMBREERenderer *targetRenderer, Histogram2D<float>* distro, float weightScalar, bool inverse) {
    std::vector<std::vector<float>> weights = std::vector<std::vector<float>>(targetRenderer->getWidth(), std::vector<float>(targetRenderer->getHeight()));
    auto bins = distro->NumberOfBins();
    
    for(int xBin = 0 ; xBin < bins.first ; ++xBin) {
        for(int yBin = 0 ; yBin < bins.second ; ++yBin) {
            float scaledValue = distro->ScaledValue(xBin, yBin);
            
            if(inverse) {
                scaledValue = 1-scaledValue;
            }
            weights[xBin][yBin] = scaledValue * weightScalar;
        }
    }
    
    CopySwapChainTo(targetRenderer, weights);
}
void EMBREERenderer::CopySwapChainTo(EMBREERenderer* targetRenderer, const std::vector<std::vector<float>>& weights) {
    SwapChain* thisSwapChain = g_single_device->rtGetSwapChain(g_frameBuffer).ptr;
    SwapChain* thatSwapChain = targetRenderer->GetSwapChain().ptr;
    
    Ref<FrameBuffer> thisBuffer = thisSwapChain->buffer();
    Ref<FrameBuffer> thatBuffer = thatSwapChain->buffer();
    
    //    thatBuffer.ptr = thisBuffer.ptr;
    //    thatSwapChain.ptr = thisSwapChain.ptr;
    
    print2DVector(weights,"weights matrix");
    
    //TODO: Support varying sizes, especially those with the same aspect ratio
    if(thisSwapChain->getWidth() != thatSwapChain->getWidth() || thisSwapChain->getHeight() != thatSwapChain->getHeight()) {
        printf("Sorry, different swapchain sizes not currently supported\n");
        return;
    }
    if(thisSwapChain->getWidth() != weights.size() && thisSwapChain->getHeight() != weights[0].size()) {
        throw std::runtime_error("Weights matrix dimensions do not match swapchain dimensions.");
    }
    else {
        //Get the accumulation buffers
        AccuBuffer* thisAccuBuffer = thisSwapChain->accu().ptr;
        AccuBuffer* thatAccuBuffer = thatSwapChain->accu().ptr;
        thatSwapChain->clearAll();
        for(int x = 0 ; x < thisSwapChain->getWidth() ; ++x) {
            for(int y = 0 ; y < thisSwapChain->getHeight() ; ++y) {
                
                float weight = weights[x][y];
                
                Vec4f thisPixel = thisAccuBuffer->getRaw(x, y);
                //                Color thisColor = Color(thisPixel.x,thisPixel.y,thisPixel.z);
                Color thisColor = thisAccuBuffer->get(x, y);
                //                Color thisColor = thisAccuBuffer->get(x, y);
                float diffWeight = thisPixel.w / weight;
                thisPixel.x /= diffWeight;
                thisPixel.y /= diffWeight;
                thisPixel.z /= diffWeight;
                thisPixel.w = weight;
                
                //                thatAccuBuffer->update(x, y, thisColor, weight, true);
                thatAccuBuffer->set(x, y, thisPixel);
                thatBuffer->set(x, y, thisColor);
                //                thatAccuBuffer->set(x, y, thisPixel);
            }
        }
    }
}
void EMBREERenderer::parseCommandLine(Ref<ParseStream> cin, const FileName& path)
{
	// file name to write to -- "" means "display mode"
	while (true)
	{
		std::string tag = cin->getString();
		if (tag == "") return;
		
		/* parse command line parameters from a file */
		if (tag == "-c") {
			FileName file = path + cin->getFileName();
			parseCommandLine(new ParseStream(new LineCommentFilter(file, "#")), file.path());
		}
		
		/* read model from file */
		else if (tag == "-i") {
			std::vector<Handle<Device::RTPrimitive> > prims = rtLoadScene(makeFileName(path,cin->getFileName()));
			
			g_prims.insert(g_prims.end(), prims.begin(), prims.end());
		}
		
		/* triangulated sphere */
		else if (tag == "-trisphere")
		{
			Handle<Device::RTShape> sphere = g_device->rtNewShape("sphere");
			const Vector3f P = cin->getVector3f();
			g_device->rtSetFloat3(sphere, "P", P.x, P.y, P.z);
			g_device->rtSetFloat1(sphere, "r", cin->getFloat());
			g_device->rtSetInt1(sphere, "numTheta", cin->getInt());
			g_device->rtSetInt1(sphere, "numPhi", cin->getInt());
			g_device->rtCommit(sphere);
			
			Handle<Device::RTMaterial> material = g_device->rtNewMaterial("matte");
			g_device->rtSetFloat3(material, "reflection", 1.0f, 0.0f, 0.0f);
			g_device->rtCommit(material);
			g_prims.push_back(g_device->rtNewShapePrimitive(sphere, material, NULL));
		}
		
		/* ambient light source */
		else if (tag == "-ambientlight") {
			Handle<Device::RTLight> light = g_device->rtNewLight("ambientlight");
			const Color L = cin->getColor();
			g_device->rtSetFloat3(light, "L", L.r, L.g, L.b);
			g_device->rtCommit(light);
			g_prims.push_back(g_device->rtNewLightPrimitive(light, NULL, NULL));
		}
		
		/* point light source */
		else if (tag == "-pointlight") {
			Handle<Device::RTLight> light = g_device->rtNewLight("pointlight");
			const Vector3f P = cin->getVector3f();
			const Color I = cin->getColor();
			g_device->rtSetFloat3(light, "P", P.x, P.y, P.z);
			g_device->rtSetFloat3(light, "I", I.r, I.g, I.b);
			g_device->rtCommit(light);
			g_prims.push_back(g_device->rtNewLightPrimitive(light, NULL, NULL));
		}
		
		else if (tag == "-masked_pointlight") {
			Handle<Device::RTLight> light = g_device->rtNewLight("pointlight");
			const Vector3f P = cin->getVector3f();
			const Color I = cin->getColor();
			int illumMask = cin->getInt();
			int shadowMask = cin->getInt();
			g_device->rtSetFloat3(light, "P", P.x, P.y, P.z);
			g_device->rtSetFloat3(light, "I", I.r, I.g, I.b);
			g_device->rtCommit(light);
			Handle<Device::RTPrimitive> prim = g_device->rtNewLightPrimitive(light, NULL, NULL);
			g_device->rtSetInt1(prim,"illumMask",illumMask);
			g_device->rtSetInt1(prim,"shadowMask",shadowMask);
			g_device->rtCommit(prim);
			g_prims.push_back(prim);
		}
		
		/* directional light source */
		else if (tag == "-directionallight" || tag == "-dirlight") {
			Handle<Device::RTLight> light = g_device->rtNewLight("directionallight");
			const Vector3f D = cin->getVector3f();
			const Color E = cin->getColor();
			g_device->rtSetFloat3(light, "D", D.x, D.y, D.z);
			g_device->rtSetFloat3(light, "E", E.r, E.g, E.b);
			g_device->rtCommit(light);
			g_prims.push_back(g_device->rtNewLightPrimitive(light, NULL, NULL));
		}
		
		/* distant light source */
		else if (tag == "-distantlight") {
			Handle<Device::RTLight> light = g_device->rtNewLight("distantlight");
			const Vector3f D = cin->getVector3f();
			const Color L = cin->getColor();
			g_device->rtSetFloat3(light, "D", D.x, D.y, D.z);
			g_device->rtSetFloat3(light, "L", L.r, L.g, L.b);
			g_device->rtSetFloat1(light, "halfAngle", cin->getFloat());
			g_device->rtCommit(light);
			g_prims.push_back(g_device->rtNewLightPrimitive(light, NULL, NULL));
		}
		
		/* spot light source */
		else if (tag == "-spotlight") {
			Handle<Device::RTLight> light = g_device->rtNewLight("spotlight");
			const Vector3f P = cin->getVector3f();
			const Vector3f D = cin->getVector3f();
			const Color I = cin->getColor();
			const float angleMin = cin->getFloat();
			const float angleMax = cin->getFloat();
			g_device->rtSetFloat3(light, "P", P.x, P.y, P.z);
			g_device->rtSetFloat3(light, "D", D.x, D.y, D.z);
			g_device->rtSetFloat3(light, "I", I.r, I.g, I.b);
			g_device->rtSetFloat1(light, "angleMin", angleMin);
			g_device->rtSetFloat1(light, "angleMax", angleMax);
			g_device->rtCommit(light);
			g_prims.push_back(g_device->rtNewLightPrimitive(light, NULL, NULL));
		}
		
		/* triangular light source */
		else if (tag == "-trianglelight") {
			Vector3f P = cin->getVector3f();
			Vector3f U = cin->getVector3f();
			Vector3f V = cin->getVector3f();
			Vector3f L = cin->getVector3f();
			
			Handle<Device::RTLight> light = g_device->rtNewLight("trianglelight");
			g_device->rtSetFloat3(light, "v0", P.x, P.y, P.z);
			g_device->rtSetFloat3(light, "v1", P.x + U.x, P.y + U.y, P.z + U.z);
			g_device->rtSetFloat3(light, "v2", P.x + V.x, P.y + V.y, P.z + V.z);
			g_device->rtSetFloat3(light, "L",  L.x, L.y, L.z);
			g_device->rtCommit(light);
			g_prims.push_back(g_device->rtNewLightPrimitive(light, NULL, NULL));
		}
		
		/* quad light source */
		else if (tag == "-quadlight")
		{
			Vector3f P = cin->getVector3f();
			Vector3f U = cin->getVector3f();
			Vector3f V = cin->getVector3f();
			Vector3f L = cin->getVector3f();
			
			Handle<Device::RTLight> light0 = g_device->rtNewLight("trianglelight");
			g_device->rtSetFloat3(light0, "v0", P.x + U.x + V.x, P.y + U.y + V.y, P.z + U.z + V.z);
			g_device->rtSetFloat3(light0, "v1", P.x + U.x, P.y + U.y, P.z + U.z);
			g_device->rtSetFloat3(light0, "v2", P.x, P.y, P.z);
			g_device->rtSetFloat3(light0, "L",  L.x, L.y, L.z);
			g_device->rtCommit(light0);
			g_prims.push_back(g_device->rtNewLightPrimitive(light0, NULL, NULL));
			
			Handle<Device::RTLight> light1 = g_device->rtNewLight("trianglelight");
			g_device->rtSetFloat3(light1, "v0", P.x + U.x + V.x, P.y + U.y + V.y, P.z + U.z + V.z);
			g_device->rtSetFloat3(light1, "v1", P.x, P.y, P.z);
			g_device->rtSetFloat3(light1, "v2", P.x + V.x, P.y + V.y, P.z + V.z);
			g_device->rtSetFloat3(light1, "L",  L.x, L.y, L.z);
			g_device->rtCommit(light1);
			g_prims.push_back(g_device->rtNewLightPrimitive(light1, NULL, NULL));
		}
		
		/* HDRI light source */
		else if (tag == "-hdrilight")
		{
			Handle<Device::RTLight> light = g_device->rtNewLight("hdrilight");
			const Color L = cin->getColor();
			g_device->rtSetFloat3(light, "L", L.r, L.g, L.b);
			g_device->rtSetImage(light, "image", rtLoadImage(path + cin->getFileName()));
			g_device->rtCommit(light);
			g_prims.push_back(g_device->rtNewLightPrimitive(light, NULL, NULL));
		}
		
		/* parse camera parameters */
		else if (tag == "-vp")     g_camPos         = Vector3f(cin->getVector3f());
		else if (tag == "-vi")     g_camLookAt      = Vector3f(cin->getVector3f());
		else if (tag == "-vd")     g_camLookAt      = g_camPos + cin->getVector3f();
		else if (tag == "-vu")     g_camUp          = cin->getVector3f();
		else if (tag == "-angle")  g_camFieldOfView = cin->getFloat();
		else if (tag == "-fov")    g_camFieldOfView = cin->getFloat();
		else if (tag == "-radius") g_camRadius      = cin->getFloat();
		
		/* frame buffer size */
		else if (tag == "-size") {
			g_width = cin->getInt();
			g_height = cin->getInt();
//			g_frameBuffer = g_device->rtNewFrameBuffer(g_format.c_str(), g_width, g_height, g_numBuffers);
		}
		
		/* set framebuffer format */
		else if (tag == "-framebuffer" || tag == "-fb") {
			g_format = cin->getString();
//			g_frameBuffer = g_device->rtNewFrameBuffer(g_format.c_str(), g_width, g_height, g_numBuffers);
		}
		
		/* full screen mode */
		else if (tag == "-fullscreen") g_fullscreen = true;
		
		/* refine rendering when not moving */
		else if (tag == "-refine") g_refine = cin->getInt();
		
		/* scene type to use */
		else if (tag == "-scene") g_scene = cin->getString();
		
		/* acceleration structure to use */
		else if (tag == "-accel") {
			g_accel = g_mesh_accel = cin->getString();
		}
		
		/* builder to use */
		else if (tag == "-builder") {
			g_builder = g_mesh_builder = cin->getString();
		}
		
		/* traverser to use */
		else if (tag == "-traverser") {
			g_traverser = g_mesh_traverser = cin->getString();
		}
		
		/* set renderer */
		else if (tag == "-renderer")
		{
			std::string renderer = cin->getString();
			if      (renderer == "debug"     ) parseDebugRenderer(cin, path);
			else if (renderer == "pt"        ) parsePathTracer(cin, path);
			else if (renderer == "pathtracer") parsePathTracer(cin, path);
			else throw std::runtime_error("(when parsing -renderer) : unknown renderer: " + renderer);
		}
		
		/* set gamma */
		else if (tag == "-gamma") {
			g_device->rtSetFloat1(g_tonemapper, "gamma", g_gamma = cin->getFloat());
			g_device->rtCommit(g_tonemapper);
		}
		
		/* set gamma */
		else if (tag == "-vignetting") {
			g_device->rtSetBool1(g_tonemapper, "vignetting", g_vignetting = cin->getInt());
			g_device->rtCommit(g_tonemapper);
		}
		
		/* set recursion depth */
		else if (tag == "-depth") {
			g_device->rtSetInt1(g_renderer, "maxDepth", g_depth = cin->getInt());
			g_device->rtCommit(g_renderer);
		}
		
		/* set samples per pixel */
		else if (tag == "-spp") {
			g_device->rtSetInt1(g_renderer, "sampler.spp", g_spp = cin->getInt());
			g_device->rtCommit(g_renderer);
		}
		
		/* set the backplate */
		else if (tag == "-backplate") {
			g_device->rtSetImage(g_renderer, "backplate", g_backplate = rtLoadImage(path + cin->getFileName()));
			g_device->rtCommit(g_renderer);
		}
		
		/* number of frames to render in output mode (used for benchmarking) */
		else if (tag == "-frames")
			g_num_frames = cin->getInt();
		
		/* render frame */
		else if (tag == "-o") {
			std::string fn = cin->getFileName();
			if (fn[0] == '/')
				g_outFileName = fn; //outputMode(path + cin->getFileName());
			else
				g_outFileName = path+fn; //outputMode(path + cin->getFileName());
		}
		
		/* display image */
		else if (tag == "-display")
			g_outFileName = ""; //displayMode();
		
		/* regression testing */
		else if (tag == "-regression")
		{
			g_refine = false;
			g_regression = true;
			Handle<Device::RTScene> scene;
//				GLUTDisplay(AffineSpace3f::lookAtPoint(g_camPos, g_camLookAt, g_camUp), 0.01f, scene);
		}
		
		else if (tag == "-version") {
			std::cout << "Embree renderer version 2.0" << std::endl;
			exit(1);
		}
		
		else if (tag == "-h" || tag == "-?" || tag == "-help" || tag == "--help")
		{
			std::cout << std::endl;
			std::cout << "Embree Version 2.0" << std::endl;
			std::cout << std::endl;
			std::cout << "  usage: embree -i model.obj -renderer debug -display" << std::endl;
			std::cout << "         embree -i model.obj -renderer pathtracer -o out.tga" << std::endl;
			std::cout << "         embree -c model.ecs -display" << std::endl;
			std::cout << std::endl;
			std::cout << "-renderer [debug,profile,pathtracer]" << std::endl;
			std::cout << "  Sets the renderer to use." << std::endl;
			std::cout << std::endl;
			std::cout << "-c file" << std::endl;
			std::cout << "  Parses command line parameters from file." << std::endl;
			std::cout << std::endl;
			std::cout << "-i file" << std::endl;
			std::cout << "  Loads a scene from file." << std::endl;
			std::cout << std::endl;
			std::cout << "-o file" << std::endl;
			std::cout << "  Renders and outputs the image to the file." << std::endl;
			std::cout << std::endl;
			std::cout << "-display" << std::endl;
			std::cout << "  Interactively displays the rendering into a window." << std::endl;
			std::cout << std::endl;
			std::cout << "-vp x y z" << std::endl;
			std::cout << "  Sets camera position to the location (x,y,z)." << std::endl;
			std::cout << std::endl;
			std::cout << "-vi x y z" << std::endl;
			std::cout << "  Sets camera lookat point to the location (x,y,z)." << std::endl;
			std::cout << std::endl;
			std::cout << "-vd x y z" << std::endl;
			std::cout << "  Sets camera viewing direction to (x,y,z)." << std::endl;
			std::cout << std::endl;
			std::cout << "-vu x y z" << std::endl;
			std::cout << "  Sets camera up direction to (x,y,z)." << std::endl;
			std::cout << std::endl;
			std::cout << "-fov angle" << std::endl;
			std::cout << "  Sets camera field of view in y direction to angle." << std::endl;
			std::cout << std::endl;
			std::cout << "-size width height" << std::endl;
			std::cout << "  Sets the width and height of image to render." << std::endl;
			std::cout << std::endl;
			std::cout << "-fullscreen" << std::endl;
			std::cout << "  Enables full screen display mode." << std::endl;
			std::cout << std::endl;
			std::cout << "-accel [bvh2,bvh4,bvh4.spatial].[triangle1,triangle1i,triangle4,...]" << std::endl;
			std::cout << "  Sets the spatial index structure to use." << std::endl;
			std::cout << std::endl;
			std::cout << "-gamma v" << std::endl;
			std::cout << "  Sets gamma correction to v (only pathtracer)." << std::endl;
			std::cout << std::endl;
			std::cout << "-depth i" << std::endl;
			std::cout << "  Sets the recursion depth to i (default 16)" << std::endl;
			std::cout << std::endl;
			std::cout << "-spp i" << std::endl;
			std::cout << "  Sets the number of samples per pixel to i (default 1) (only pathtracer)." << std::endl;
			std::cout << std::endl;
			std::cout << "-backplate" << std::endl;
			std::cout << "  Sets a high resolution back ground image. (default none) (only pathtracer)." << std::endl;
			std::cout << std::endl;
			
			std::cout << "-ambientlight r g b" << std::endl;
			std::cout << "  Creates an ambient light with intensity (r,g,b)." << std::endl;
			std::cout << std::endl;
			std::cout << "-pointlight px py pz r g b" << std::endl;
			std::cout << "  Creates a point light with intensity (r,g,b) at position (px,py,pz)." << std::endl;
			std::cout << std::endl;
			std::cout << "-distantlight dx dy dz r g b halfAngle" << std::endl;
			std::cout << "  Creates a distant sun light with intensity (r,g,b) shining into " << std::endl;
			std::cout << "  direction (dx,dy,dz) from the cone spanned by halfAngle." << std::endl;
			std::cout << std::endl;
			std::cout << "-trianglelight px py pz ux uy uz vx vy vz r g b" << std::endl;
			std::cout << "  Creates a triangle-light with intensity (r,g,b) spanned by the point " << std::endl;
			std::cout << "  (px,py,pz) and the vectors (vx,vy,vz) and (ux,uy,uz)." << std::endl;
			std::cout << std::endl;
			std::cout << "-quadlight px py pz ux uy uz vx vy vz r g b" << std::endl;
			std::cout << "  Creates a quad-light with intensity (r,g,b) spanned by the point " << std::endl;
			std::cout << "  (px,py,pz) and the vectors (vx,vy,vz) and (ux,uy,uz)." << std::endl;
			std::cout << std::endl;
			std::cout << "-hdrilight r g b file" << std::endl;
			std::cout << "  Creates a high dynamic range environment light from the image " << std::endl;
			std::cout << "  file. The intensities are multiplies by (r,g,b)." << std::endl;
			std::cout << std::endl;
			std::cout << "-trisphere px py pz r theta phi" << std::endl;
			std::cout << "  Creates a triangulated sphere with radius r at location (px,py,pz) " << std::endl;
			std::cout << "  and triangulation rates theta and phi." << std::endl;
			std::cout << std::endl;
			std::cout << "-[no]refine" << std::endl;
			std::cout << "  Enables (default) or disables the refinement display mode." << std::endl;
			std::cout << std::endl;
			std::cout << "-regression" << std::endl;
			std::cout << "  Runs a stress test of the system." << std::endl;
			std::cout << std::endl;
			std::cout << "-version" << std::endl;
			std::cout << "  Prints version number." << std::endl;
			std::cout << std::endl;
			std::cout << "-h, -?, -help, --help" << std::endl;
			std::cout << "  Prints this help." << std::endl;
			exit(1);
		}
		
		/* skip unknown command line parameter */
		else {
			std::cerr << "unknown command line parameter: " << tag << " ";
			while (cin->peek() != "" && cin->peek()[0] != '-') std::cerr << cin->getString() << " ";
			std::cerr << std::endl;
		}
	}
}

void* EMBREERenderer::MapFrameBuffer() {
	return g_device->rtMapFrameBuffer(g_frameBuffer);
}

void EMBREERenderer::UnmapFrameBuffer() {
	g_device->rtUnmapFrameBuffer(g_frameBuffer);
}

int EMBREERenderer::getWidth() {
	return g_width;
}

int EMBREERenderer::getHeight() {
	return g_height;
}

int EMBREERenderer::getSamplesPerPixel() {
	return g_spp;
}
int EMBREERenderer::GetDepth() {
    return g_depth;
}