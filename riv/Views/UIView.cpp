//
//  RIVUIView.cpp
//  embree
//
//  Created by Gerard Simons on 22/01/15.
//
//

#include "UIView.h"
#include "../helper.h"

RIVUIView* RIVUIView::instance = NULL;
int RIVUIView::windowHandle = -1;

RIVUIView::RIVUIView(RIVDataSet<float,ushort>** dataset, int startX, int startY, int width, int height, int paddingX, int paddingY) :
	RIVDataView(dataset, startX, startY, width, height, paddingX, paddingY)
{
	if(instance) {
		throw std::runtime_error("Only 1 instance allowed of RIVUIView");
	}
	else {
		instance = this;
	}
	setupUI();
}
void RIVUIView::setupUI() {
	
	if(GluiHandle) {
		delete GluiHandle;
	}
	
	GluiHandle = new Glui2("g2Blue.cfg", NULL, NULL);
	
	float effectiveWidth = width - 2*paddingX;
	
	// Generate all the example GUI elements
	// Note that some of these need supporting buttons
	// to enable / test each individual feature
	g2Label* ComponentTitle = NULL;
	
	/*** g2Label ***/
	ComponentTitle = GluiHandle->AddLabel(20, 10, "g2Label");
	ComponentTitle->SetColor(0.0f, 0.0f, 0.0f);
	
	opacitySlider = GluiHandle->AddSlider(paddingX, paddingY);
	opacitySlider->SetWidth(effectiveWidth);
	
	printf("Setup UI was succesful...\n");
}
void RIVUIView::DrawInstance() {
	if(instance) {
		instance->Draw();
	}
}
void RIVUIView::ReshapeInstance(int newWidth, int newHeight) {
	if(instance) {
		instance->Reshape(newWidth, newHeight);
	}
}
void RIVUIView::Reshape(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void RIVUIView::Draw() {
	
	printHeader("UI View Redraw");
	
	glClearColor(0.92f, 0.94f, 0.97f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	GluiHandle->Render();
	
	glFlush();
	glutSwapBuffers();
}
void RIVUIView::Mouse(int button, int state, int x, int y) {
	if(instance) {
		instance->HandleMouse(button, state, x, y);
	}
}
void RIVUIView::Motion(int x, int y) {
	if(instance) {
		instance->HandleMouseMotion(x,y);
	}
}
bool RIVUIView::HandleMouse(int button, int state, int x, int y) {
	if(containsPoint(x, y)) {
		
		return true;
	}
	else return false;
}
bool RIVUIView::HandleMouseMotion(int x, int y) {
	return false;
}