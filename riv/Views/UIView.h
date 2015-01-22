//
//  UIView.h
//  embree
//
//  Created by Gerard Simons on 22/01/15.
//
//

#ifndef __embree__UIView__
#define __embree__UIView__

#include "DataView.h"

#include <Glui2/glui2.h>

class RIVUIView : public RIVDataView {
private:
	static RIVUIView* instance;
	void setupUI();
	
	// Global Glui2 Handle
	Glui2* GluiHandle = NULL;
	
	//GLUI elements
	g2Slider* opacitySlider = NULL;
public:
	static int windowHandle;
	
	RIVUIView(RIVDataSet<float,ushort>** dataset, int startX, int startY, int width, int height, int paddingX, int paddingY);
	
	static void DrawInstance();
	static void ReshapeInstance(int newWidth, int newHeight);
	static void Mouse(int button, int state, int x, int y);
	static void Motion(int x, int y);
	
	void Reshape(int newWidth, int newHeight);
	void Draw();
	bool HandleMouse(int button, int state, int x, int y);
	bool HandleMouseMotion(int x, int y);
};

#endif /* defined(__embree__UIView__) */
