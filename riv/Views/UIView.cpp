//
//  RIVUIView.cpp
//  embree
//
//  Created by Gerard Simons on 22/01/15.
//
//

#include "UIView.h"
#include "../helper.h"
#include "../Configuration.h"

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

const std::string RIVUIView::rgbColorChoiceRadiance = "Radiance";
const std::string RIVUIView::rgbColorChoicePosition = "Position";

RIVUIView::RIVUIView(RIVDataSet<float,ushort>** datasetOne, ParallelCoordsView* pcView, RIV3DView* sceneView, RIVImageView* renderView, int startX, int startY, int width, int height, int paddingX, int paddingY) :
RIVDataView(datasetOne, startX, startY, width, height, paddingX, paddingY),
parallelCoordsView(pcView), sceneView(sceneView), renderView(renderView)
{
	if(instance) {
		throw std::runtime_error("Only 1 instance allowed of RIVUIView");
	}
	else {
		instance = this;
	}
	selectedDataset = datasetOne;
	setupUI();
}
RIVUIView::RIVUIView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, ParallelCoordsView* pcView, RIV3DView* sceneView, RIVImageView* renderView, int startX, int startY, int width, int height, int paddingX, int paddingY) :
RIVDataView(datasetOne, datasetTwo, startX, startY, width, height, paddingX, paddingY),
parallelCoordsView(pcView), sceneView(sceneView), renderView(renderView)
{
	if(instance) {
		throw std::runtime_error("Only 1 instance allowed of RIVUIView");
	}
	else {
		instance = this;
	}
	
	selectedDataset = datasetOne;
	
	setupUI();
}
std::string RIVUIView::stringFromProperty(RIVColorProperty** colorProperty) {
	if(colorProperty == NULL || *colorProperty == NULL) {
		return "NULL";
	}
	if(typeid(*colorProperty) == typeid(RIVColorRGBProperty<float>) || typeid(*colorProperty) == typeid(RIVColorRGBProperty<ushort>)) {
		return rgbColorPropertyText;
	}
	else if(typeid(*colorProperty) == typeid(RIVEvaluatedColorProperty<float>) || typeid(*colorProperty) == typeid(RIVEvaluatedColorProperty<ushort>)) {
		return linearColorPropertyText;
	}
	else if(typeid(**colorProperty) == typeid(RIVFixedColorProperty)) {
		return fixedColorPropertyText;
	}
	else {
		throw std::runtime_error("Unsupported color property");
	}
}
void RIVUIView::OnValueChanged(RIVUIDropdown* source, int selectedIndex, const std::string& selectedText) {
	if(source == viewDropdown) {
		//Get the color property of the selected view
		
		//The user changed the selected view
		if(selectedText == pcViewText) {
			
			printf("Selected the parallel coordinates plot view\n");
			
			tableDropdown->SetValues({pathTableText,rayTableText});
			selectedView = parallelCoordsView;
			
//			std::string selectedTable = tableDropdown->GetSelectedValue();
//			std::string selectedColor = colorTypeDropdown->GetSelectedValue();
		}
		else if(selectedText == sceneViewText) {
			printf("Selected the scene view\n");
			selectedView = sceneView;
			tableDropdown->SetValues({rayTableText}); //The scene view only supports coloring based on intersection table
		}
		else {
			//Unknown view
		}
		rendererDropdown->SetSelectedValue(0);
		tableDropdown->SetSelectedValue(0);
	}
	else if(source == rendererDropdown) {
		if(selectedText == rendererOneText) {
			selectedRenderer = 0;
			printf("Selected renderer #1\n");
		}
		else if(selectedText == rendererTwoText) {
			selectedRenderer = 1;
			printf("Selected renderer #2\n");
		}
	}
	else if(source == tableDropdown) {
		if(selectedText == pathTableText) {
			selectedTable = PATHS_TABLE;
			selectedColorProperty = selectedView->GetRayColor(selectedRenderer);
		}
		else if(selectedText == rayTableText) {
			selectedTable = INTERSECTIONS_TABLE;
			selectedColorProperty = selectedView->GetRayColor(selectedRenderer);
		}
		std::string colorTypeString = stringFromProperty(selectedColorProperty);
		colorTypeDropdown->SetSelectedValue(colorTypeString);
	}
	else if(source == colorTypeDropdown) {
		selectedColorType = selectedText;
		if(selectedText == rgbColorPropertyText) {
			rgbRecordsDropdown->Show();
			if(selectedTable == PATHS_TABLE) {
				rgbRecordsDropdown->SetValues({rgbColorChoiceRadiance});
			}
			else if(selectedTable == INTERSECTIONS_TABLE) {
				rgbRecordsDropdown->SetValues({rgbColorChoiceRadiance,rgbColorChoicePosition});
			}
		}
		else if(selectedText == linearColorPropertyText) {
			
			recordOneDropdown->Show();
			recordOneText->Show();
			std::vector<std::string> values;
			//Fill the dropdown with the names of the records in
			auto& records = (*selectedDataset)->GetTable(selectedTable)->GetAllRecords();
			tuple_for_each(records, [&](auto tRecords) {
				for(auto record : tRecords) {
					values.push_back(record->name);
				}
			});
			recordOneDropdown->SetValues(values);
		}
		else if(selectedText == fixedColorPropertyText) {
			recordOneDropdown->Hide();
			recordOneText->Hide();
			rgbRecordsDropdown->Hide();
		}
	}
	else if(source == rgbRecordsDropdown) {
		auto table = (*selectedDataset)->GetTable(selectedTable);
		if(selectedText == rgbColorChoiceRadiance) {
			if(selectedTable == PATHS_TABLE) {
				delete *selectedColorProperty;
				*selectedColorProperty = new RIVColorRGBProperty<float>(table,table->GetRecord<float>(PATH_R),table->GetRecord<float>(PATH_G),table->GetRecord<float>(PATH_B));
			}
			else if(selectedTable == INTERSECTIONS_TABLE) {
				if(selectedText == rgbColorChoicePosition) {
					delete *selectedColorProperty;
					*selectedColorProperty = new RIVColorRGBProperty<float>(table,table->GetRecord<float>(POS_X),table->GetRecord<float>(POS_Y),table->GetRecord<float>(POS_Z));
				}
				else if(selectedText == rgbColorChoiceRadiance) {
					delete *selectedColorProperty;
					*selectedColorProperty = new RIVColorRGBProperty<float>(table,table->GetRecord<float>(INTERSECTION_R),table->GetRecord<float>(INTERSECTION_G),table->GetRecord<float>(INTERSECTION_B));
				}
			}
		}
	}
	else if(source == recordOneDropdown) {
		if(selectedColorType == fixedColorPropertyText) {
			delete *selectedColorProperty;
		}
		else if(selectedColorType == linearColorPropertyText) {
			delete *selectedColorProperty;
		}
		else {
			throw std::runtime_error("Unsupported color type\n");
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
	int y = height - spacingY;
	
//	uiElements.reserve(10);
	
	viewText = new RIVUILabel("View: ",spacingX,y,textWidth,elementsHeight);
	uiElements.push_back(viewText);
	
	std::vector<std::string> dropdownChoices = (std::vector<std::string>){pcViewText,sceneViewText};
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
	
	colorTypeDropdown = new RIVUIDropdown(colorTypeText->x + colorTypeText->width, y, dropDownWidth, elementsHeight, {rgbColorPropertyText,fixedColorPropertyText,linearColorPropertyText});
	uiElements.push_back(colorTypeDropdown);
	
	y -= 2 * spacingY;
	
	recordOneText = new RIVUILabel("Data Records: ",20,y,textWidth,elementsHeight);
	uiElements.push_back(recordOneText);
	
	recordOneDropdown = new RIVUIDropdown(colorTypeText->x + colorTypeText->width, y, dropDownWidth, elementsHeight);
	uiElements.push_back(recordOneDropdown);
	
	rgbRecordsDropdown = new RIVUIDropdown(colorTypeText->x + colorTypeText->width, y, dropDownWidth, elementsHeight,{rgbColorChoicePosition,rgbColorChoiceRadiance});
	uiElements.push_back(rgbRecordsDropdown);
	rgbRecordsDropdown->Hide();
	
	viewDropdown->AddListener(this);
	colorTypeDropdown->AddListener(this);
	tableDropdown->AddListener(this);
	rendererDropdown->AddListener(this);
	rgbRecordsDropdown->AddListener(this);
	
	viewDropdown->SetSelectedValue(0);
	
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
	for(int i = 0 ; i < uiElements.size() ; ++i) {
		uiElements[i]->Draw();
	}
	
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
