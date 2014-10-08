#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "../Data/DataSet.h"
#include "../Geometry/Geometry.h"
#include "../Graphics/ColorProperty.h"
#include "../Graphics/SizeProperty.h"
#include "DataController.h"

#include <string>

	class RIVDataSet;
	class RIVColorProperty;

	class RIVDataView
	{
	protected:
		DataController* dataController = NULL;
		
		RIVColorProperty* colorProperty = NULL;
		RIVSizeProperty* sizeProperty = NULL;

		bool needsRedraw;

		//constructor
		RIVDataView(DataController* dataController, int startX, int startY, int width, int height, int paddingX, int paddingY, RIVColorProperty* colorProperty, RIVSizeProperty* sizeProperty) {
			this->startX = startX;
			this->startY = startY;
			
			this->width = width;
			this->height = height;
			
			this->paddingX = paddingX;
			this->paddingY = paddingY;
			
			this->colorProperty = colorProperty;
			this->sizeProperty = sizeProperty;
			
			this->dataController = dataController;
		};
		RIVDataView(DataController* dataController, int startX, int startY, int width, int height, int paddingX, int paddingY) {
			this->startX = startX;
			this->startY = startY;
			
			this->width = width;
			this->height = height;
			
			this->paddingX = paddingX;
			this->paddingY = paddingY;
			
			this->dataController = dataController;
		}
		RIVDataView(DataController* dataController, RIVColorProperty* colorProperty_, RIVSizeProperty* sizeProperty_) {
			this->dataController = dataController;
			colorProperty = colorProperty_;
			sizeProperty = sizeProperty_;
		};
		RIVDataView(DataController* dataController) {
			this->dataController = dataController;
		}
		~RIVDataView(void) { /* Delete some stuff I guess */ };
	public:
		//properties
		int startX,startY;
		int width,height;
		int paddingX,paddingY = 0;
		bool isDragging;
		std::string identifier;
		//functions
//		void SetData(RIVDataSet *newDataSet) { dataset = newDataSet; needsRedraw = true;}
		void Invalidate() { needsRedraw = true; }
		
		void ToViewSpaceCoordinates(int* x, int* y) {
			(*x) -= startX;
			(*y) -= startY;
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
		//must implement
		virtual void Reshape(int newWidth, int newHeight) = 0;
		virtual void Draw() = 0;
		static void DrawInstance();
		static void ReshapeInstance(int newWidth, int newHeight);
		virtual bool HandleMouse(int button, int state, int x, int y) = 0;
		virtual bool HandleMouseMotion(int x, int y) = 0;
	};


// your code
#endif
