//
//  HeatmapView.h
//  Afstuderen
//
//  Created by Gerard Simons on 23/08/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__HeatmapView__
#define __Afstuderen__HeatmapView__

#include "DataView.h"

 
	class RIVHeatMapView : public RIVDataView {
	public:
		//Virtual functions from RIVDataView
		void Draw();
		void Reshape(int width, int height);
		bool HandleMouse(int,int,int,int);
		bool HandleMouseMotion(int,int);
		
		static RIVHeatMapView* instance;
		
		RIVHeatMapView(DataController* dataController) : RIVDataView(dataController) {
			instance = this;
			computeHeatMap();
		};
		
	//    //implement virtual functions prescribed by DataSetListener
	//    virtual void OnDataSetChanged();
//		void SetData(RIVDataSet *newDataSet) { dataset = newDataSet; needsRedraw = true; computeHeatMap(); }
		static void ReshapeInstance(int width, int height);
		static void DrawInstance();
		static void Mouse(int button, int state, int x, int y);
		static void Motion(int x, int y);
	private:
		ushort xMax, yMax;
		
		//Unique methods
		void computeHeatMap();
		void heatMapColor(float heatmapValue);
	};


#endif /* defined(__Afstuderen__HeatmapView__) */
