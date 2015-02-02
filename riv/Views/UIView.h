//
//  UIView.h
//  embree
//
//  Created by Gerard Simons on 22/01/15.
//
//

#ifndef __embree__UIView__
#define __embree__UIView__

#include <vector>

#include "DataView.h"
#include "ImageView.h"
#include "ParallelCoordsView.h"
#include "3DView.h"
#include "../UI/RIVUILabel.h"
#include "../UI/RIVUIDropdown.h"

class RIVUIView : public RIVDataView, public RIVUIDropdownListener {
private:
	static RIVUIView* instance;
	void setupUI();
	
	RIVDataSet<float,ushort>** datasetOne;
	RIVDataSet<float,ushort>* datasetTwo;
	
	static const std::string pcViewText;
	static const std::string sceneViewText;
	
	static const std::string rendererOneText;
	static const std::string rendererTwoText;
	
	static const std::string pathTableText;
	static const std::string rayTableText;
	
	static const std::string fixedColorPropertyText;
	static const std::string rgbColorPropertyText;
	static const std::string linearColorPropertyText;
	
	static const std::string rgbColorChoiceRadiance;
	static const std::string rgbColorChoicePosition;
	
	RIVDataView* selectedView = NULL;
	int selectedRenderer = -1;
	RIVDataSet<float,ushort>** selectedDataset = NULL;
	std::string selectedTable;
	std::string selectedColorType;
	RIVColorProperty** selectedColorProperty = NULL;
	
	RIV3DView* sceneView;
	ParallelCoordsView* parallelCoordsView;
	RIVImageView* renderView;
	
	RIVUILabel* viewText;
	RIVUIDropdown* viewDropdown;
	
	RIVUILabel* rendererText;
	RIVUIDropdown* rendererDropdown;
	
	RIVUILabel* tableText;
	RIVUIDropdown* tableDropdown;
	
	RIVUILabel* colorTypeText;
	RIVUIDropdown* colorTypeDropdown;
	
	RIVUILabel* rgbRecordsText;
	RIVUIDropdown* rgbRecordsDropdown;
	
	RIVUILabel* recordOneText;
	RIVUIDropdown* recordOneDropdown;
	RIVUILabel* recordTwoText;
	RIVUIDropdown* recordTwoDropdown;
	RIVUILabel* recordThreeText;
	RIVUIDropdown* recordThreeDropdown;
	
	// dropdown choices
	std::vector<RIVUIElement*> uiElements;
	
	void onDropdownChanged(RIVUIDropdown* source, int newSelectedIndex, const std::string& newSelectedValue);
	void redisplayWindow();
	std::string stringFromProperty(RIVColorProperty** colorProperty);
public:
	static int windowHandle;
	
	RIVUIView(RIVDataSet<float,ushort>** dataset, ParallelCoordsView* parallelCoordsView, RIV3DView* sceneView, RIVImageView* renderView, int startX, int startY, int width, int height, int paddingX, int paddingY);
	RIVUIView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, ParallelCoordsView* parallelCoordsView, RIV3DView* sceneView, RIVImageView* renderView, int startX, int startY, int width, int height, int paddingX, int paddingY);
	
	void OnValueChanged(RIVUIDropdown* source, int selectedIndex, const std::string& selectedText);
	
	static void DrawInstance();
	static void ReshapeInstance(int newWidth, int newHeight);
	static void Mouse(int x, int y, int button, int state);
	static void Motion(int x, int y);
	
	void Reshape(int newWidth, int newHeight);
	void Draw();
	bool HandleMouse(int button, int state, int x, int y);
	bool HandleMouseMotion(int x, int y);
};

#endif /* defined(__embree__UIView__) */
