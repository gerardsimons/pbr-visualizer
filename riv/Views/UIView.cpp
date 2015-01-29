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

const std::string RIVUIView::pcViewText = "Parallel Coordinates View";
const std::string RIVUIView::sceneViewText = "Scene View";

const std::string RIVUIView::rendererOneText = "Renderer #1";
const std::string RIVUIView::rendererTwoText = "Renderer #2";

const std::string RIVUIView::pathTableText = "Paths";
const std::string RIVUIView::rayTableText = "Rays";

const std::string RIVUIView::fixedColorPropertyText = "Fixed Color";
const std::string RIVUIView::rgbColorPropertyText = "RGB Color";
const std::string RIVUIView::linearColorPropertyText = "Linear Color";

const std::string RIVUIView::rgbColorChoiceOne = "Radiance";
const std::string RIVUIView::rgbColorChoiceTwo = "Position";

RIVUIView::RIVUIView(RIVDataSet<float,ushort>** dataset, ParallelCoordsView* pcView, RIV3DView* sceneView, RIVImageView* renderView, int startX, int startY, int width, int height, int paddingX, int paddingY) :
	RIVDataView(dataset, startX, startY, width, height, paddingX, paddingY),
	parallelCoordsView(pcView), sceneView(sceneView), renderView(renderView)
{
	if(instance) {
		throw std::runtime_error("Only 1 instance allowed of RIVUIView");
	}
	else {
		instance = this;
	}
	setupUI();
}
std::string RIVUIView::stringFromProperty(RIVColorProperty* colorProperty) {
	if(colorProperty == NULL) {
		return "NULL";
	}
	if(typeid(colorProperty) == typeid(RIVColorRGBProperty<float>) || typeid(colorProperty) == typeid(RIVColorRGBProperty<ushort>)) {
		return rgbColorPropertyText;
	}
	else if(typeid(colorProperty) == typeid(RIVEvaluatedColorProperty<float>) || typeid(colorProperty) == typeid(RIVEvaluatedColorProperty<ushort>)) {
		return linearColorPropertyText;
	}
}
void RIVUIView::OnValueChanged(RIVUIDropdown* source, int selectedIndex, const std::string& selectedText) {
	if(source == viewDropdown) {
		//Get the color property of the selected view
		
		//The user changed the selected view
		if(selectedText == pcViewText) {
			
			printf("Selected the parallel coordinates plot view\n");
			
			RIVColorProperty* pathColorOne = parallelCoordsView->GetPathColorOne();
			RIVColorProperty* pathColorTwo = parallelCoordsView->GetPathColorTwo();
			RIVColorProperty* rayColorOne = parallelCoordsView->GetRayColorOne();
			RIVColorProperty* rayColorTwo = parallelCoordsView->GetRayColorTwo();
			
			tableDropdown->SetValues({pathTableText,rayTableText});
			
			std::string selectedTable = tableDropdown->GetSelectedValue();
			std::string selectedColorColor = colorTypeDropdown->GetSelectedValue();
		}
		else if(selectedText == sceneViewText) {
			printf("Selected the scene view\n");
			
			tableDropdown->SetValues({rayTableText}); //The scene view only supports coloring based on intersection table
		}
		else {
			//Unknown view
		}
	}
	else if(source == rendererDropdown) {
		if(selectedText == rendererOneText) {
			selectedDataset = 0;		}
		else if(selectedText == rendererTwoText) {
			selectedDataset = 1;
		}
	}
	else if(source == tableDropdown) {
		if(selectedText == pathTableText) {
			RIVColorProperty* pathColorOne = selectedView->GetPathColorOne();
			RIVColorProperty* pathColorTwo = selectedView->GetPathColorTwo();
			
			selectedTable = pathTableText;
		}
		else if(selectedText == rayTableText) {
			RIVColorProperty* rayColorOne = selectedView->GetRayColorOne();
			RIVColorProperty* rayColorTwo = selectedView->GetRayColorTwo();
			
			selectedTable = rayTableText;
			
			
			
			selectedColorProperty = selectedView->GetRayColor(selectedDataset);
		}
		colorTypeDropdown->SetSelectedValue(stringFromProperty(selectedColorProperty));
	}
	else if(source == colorTypeDropdown) {
		if(selectedText == rgbColorPropertyText) {
			recordOneDropdown->Show();
			recordOneText->Show();
			recordTwoDropdown->Show();
			recordTwoText->Show();
			recordThreeDropdown->Show();
			recordThreeText->Show();
			
//			dynamic_cast<RIVColorRGBProperty<<#typename T#>>>(<#expression#>)
		}
		else if(selectedText == linearColorPropertyText) {
			recordOneDropdown->Show();
			recordOneText->Show();
			recordTwoDropdown->Hide();
			recordTwoText->Hide();
			recordThreeDropdown->Hide();
			recordThreeText->Hide();
		}
	}
}
void RIVUIView::setupUI() {
	
	float effectiveWidth = width - 2*paddingX;
	
	int elementsHeight = 20;
	int dropDownWidth = 200;
	int textWidth = 150;
	
	int spacingX = 20;
	int spacingY = 20;
	int y = height - 20;
	
	viewText = new RIVUILabel("View: ",spacingX,y,textWidth,elementsHeight);
	uiElements.push_back(viewText);
	
	std::vector<std::string> dropdownChoices = {pcViewText,sceneViewText};
	viewDropdown = new RIVUIDropdown(viewText->x + viewText->width, y, dropDownWidth, elementsHeight, dropdownChoices);
	uiElements.push_back(viewDropdown);
	
	y -= 2 * spacingY;
	
	rendererText = new RIVUILabel("Renderer: ",20,y,textWidth,elementsHeight);
	uiElements.push_back(rendererText);
	
	dropdownChoices = {rendererOneText,rendererTwoText};
	rendererDropdown = new RIVUIDropdown(viewText->x + viewText->width, y, dropDownWidth, elementsHeight, dropdownChoices);
	uiElements.push_back(rendererDropdown);
	
	y -= 2 * spacingY;
	
	tableText = new RIVUILabel("Data Table: ",20,y,textWidth,elementsHeight);
	uiElements.push_back(tableText);
	
	dropdownChoices = {"Paths","Intersections"};
	tableDropdown = new RIVUIDropdown(viewText->x + viewText->width, y, dropDownWidth, elementsHeight, dropdownChoices);
	uiElements.push_back(tableDropdown);
	
	y -= 2 * spacingY;
	
	colorTypeText = new RIVUILabel("Coloring Scheme: ",20,y,textWidth,elementsHeight);
	uiElements.push_back(colorTypeText);

	dropdownChoices = {"RGB Color","Linear Color"};
	colorTypeDropdown = new RIVUIDropdown(colorTypeText->x + colorTypeText->width, y, dropDownWidth, elementsHeight, dropdownChoices);
	uiElements.push_back(colorTypeDropdown);
	
	viewDropdown->AddListener(this);
	colorTypeDropdown->AddListener(this);
	tableDropdown->AddListener(this);
	rendererDropdown->AddListener(this);
	
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
	
//	glEnable(GL_DEPTH_TEST);
}

void RIVUIView::Draw() {
	
	printHeader("UI View Redraw");
	
	glClearColor(0.92f, 0.94f, 0.97f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	//Draw the UI elements
	for(RIVUIElement* uiElement : uiElements) {
		uiElement->Draw();
	}
//	colorTypeText->Draw();
//	colorTypeDropdown->Draw();
//	
//	viewText->Draw();
//	viewDropdown->Draw();
	
	glFlush();
	glutSwapBuffers();
}
void RIVUIView::redisplayWindow() {
	int currentWindow = glutGetWindow();
	glutSetWindow(RIVUIView::windowHandle);
	needsRedraw = true;
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
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
	y = height - y;
	if(containsPoint(x, y)) {
		//Most recently active elements are reorderd to the back
		for(int i = uiElements.size() - 1 ; i > 0 ; --i) {
			RIVUIElement* uiElement = uiElements[i];
			if(uiElement->Mouse(x, y, button, state)) {
				
				//This is very important, as the order determines the order of drawing, recently 'active' elements are added to the back so they are drawn on top
				uiElements.erase(uiElements.begin() + i);
				uiElements.push_back(uiElement);
				
				redisplayWindow();
				
				return true;
			}
		}
	}
	return false;
}
bool RIVUIView::HandleMouseMotion(int x, int y) {
	return false;
}
