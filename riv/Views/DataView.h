#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "../Data/DataSet.h"
#include "../Geometry/Geometry.h"
#include "../Graphics/ColorProperty.h"
#include "../Graphics/SizeProperty.h"

#include <string>



class RIVDataSet;
class RIVColorProperty;

class RIVDataView
{
protected:
	RIVDataSet *dataset = NULL;
	
	RIVColorProperty* colorProperty = NULL;
	RIVSizeProperty* sizeProperty = NULL;
	
	//Whether the view should be redrawn
	bool dirty;
	
	//constructor
	RIVDataView(RIVDataSet *dataset, int x, int y, int width, int height, int paddingX, int paddingY, RIVColorProperty* colorProperty, RIVSizeProperty* sizeProperty) {
		this->x = x;
		this->y = y;
		
		this->width = width;
		this->height = height;
		
		this->paddingX = paddingX;
		this->paddingY = paddingY;
		
		this->colorProperty = colorProperty;
		this->sizeProperty = sizeProperty;
		
		this->dataset = dataset;
	};
	RIVDataView(RIVDataSet* dataset, int x, int y, int width, int height, int paddingX, int paddingY) {
		this->x = x;
		this->y = y;
		
		this->width = width;
		this->height = height;
		
		this->paddingX = paddingX;
		this->paddingY = paddingY;
		
		this->dataset = dataset;
	}
	RIVDataView(RIVDataSet* dataset, RIVColorProperty* colorProperty_, RIVSizeProperty* sizeProperty_) {
		this->dataset = dataset;
		colorProperty = colorProperty_;
		sizeProperty = sizeProperty_;
	};
	RIVDataView(RIVDataSet* dataset) {
		this->dataset = dataset;
	}
	~RIVDataView(void) { /* Delete some stuff I guess */ };
public:
	//properties
	int x,y;
	int width,height;
	int paddingX,paddingY = 0;
	bool isDragging;
	std::string identifier;
	//functions
	//		void SetData(RIVDataSet *newDataSet) { dataset = newDataSet; needsRedraw = true;}
	void Invalidate() { dirty = true; }
	bool IsDirty() { return dirty; };
	void ToViewSpaceCoordinates(int* xIn, int* yIn) {
		(*xIn) -= x;
		(*yIn) -= y;
	};
	void SetPadding(int paddingX, int paddingY) {
		this->paddingX = paddingX;
		this->paddingY = paddingY;
	}
	
	void SetColorProperty(RIVColorProperty* newColorProperty) {
		colorProperty = newColorProperty;
	}
	
	void SetSizeProperty(RIVSizeProperty* newSizeProperty) {
		sizeProperty = newSizeProperty;
	}
	
	//Should be converted to view space coordinates!
	bool containsPoint(int x, int y) {
		return x > 0 && x < width && y > 0 && y < height;
	}
	static void DrawInstances() { throw std::runtime_error("Draw instances not implemented."); };
	static void ReshapeInstances(int newWidth, int newHeight) { throw std::runtime_error("Reshape instances not implemented."); };
	static bool HandleKeyInstances(int keycode,int x,int y) { return false; }; //Not required
	
	//must implement
	virtual void Reshape(int newWidth, int newHeight) = 0;
	virtual void Draw() = 0;
	virtual bool HandleMouse(int button, int state, int x, int y) = 0;
	virtual bool HandleMouseMotion(int x, int y) = 0;
};


// your code
#endif

