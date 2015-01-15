#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "../Data/DataSet.h"
#include "../Geometry/Geometry.h"
#include "../Graphics/ColorProperty.h"
#include "../Graphics/SizeProperty.h"

#include <string>

	class RIVColorProperty;

	class RIVDataView
	{
	protected:
		RIVDataSet<float,ushort>** datasetOne = NULL;
		RIVDataSet<float,ushort>** datasetTwo = NULL;

		bool needsRedraw;

		//Configuration using only one renderer dataset
		RIVDataView(RIVDataSet<float,ushort>** datasetOne,int startX, int startY, int width, int height, int paddingX, int paddingY) {
			this->startX = startX;
			this->startY = startY;
			
			this->width = width;
			this->height = height;
			
			this->paddingX = paddingX;
			this->paddingY = paddingY;
			
			this->datasetOne = datasetOne;
		}
		RIVDataView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, int startX, int startY, int width, int height, int paddingX, int paddingY) {
			this->startX = startX;
			this->startY = startY;
			
			this->width = width;
			this->height = height;
			
			this->paddingX = paddingX;
			this->paddingY = paddingY;
			
			this->datasetOne = datasetOne;
			this->datasetTwo = datasetTwo;
		}
		RIVDataView(RIVDataSet<float,ushort>** datasetOne) {
			this->datasetOne = datasetOne;
		}
		RIVDataView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo) {
			this->datasetOne = datasetOne;
			this->datasetTwo = datasetTwo;
		}
		~RIVDataView(void) { /* Delete some stuff I guess */ };
	public:
		//properties
		int startX,startY;
		int width,height;
		int paddingX,paddingY = 0;

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

