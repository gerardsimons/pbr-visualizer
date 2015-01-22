//
//  SliderView.h
//  embree
//
//  Created by Gerard Simons on 03/01/15.
//
//

#ifndef __embree__SliderView__
#define __embree__SliderView__

#include "DataView.h"
#include "../Graphics/ColorProperty.h"

#include <stdio.h>
#include <set>

class RIVSliderView : public RIVDataView, public RIVDataSetListener {
private:
	static RIVSliderView* instance;
	
	int bottom = 0;
	int top = 0;
	int leftBound = 0;
	int rightBound = 0;
	
	int pointerWidth = 5;
	int histogramBins = 20;
	
	bool movePointers = false;
	
	int startPointerOneDragX;
	int startPointerTwoDragX;
	int startDragX;
	
	int* selectedPointer = NULL;
	
	int pointerOneX;
	int pointerTwoX;
	
	//A constrain on the selection of records to be used for membership computation is that they should be of the same table
	std::string selectedTable;
	std::string selectedMembershipTable;
	std::set<std::string> selectedRecords;
	
	HistogramSet<float,ushort>* distributionsOne;
	HistogramSet<float,ushort>* distributionsTwo;
	
	//What rows belong to which bin
	std::map<int,std::vector<size_t>> rowBinMembershipsOne;
	std::map<int,std::vector<size_t>> rowBinMembershipsTwo;
	
	Histogram<float> membershipHistogramOne;
	Histogram<float> membershipHistogramTwo;
	
	//Filter used last time
	riv::RowFilter* membershipFilterOne = NULL;
	riv::RowFilter* membershipFilterTwo = NULL;
	
//	//Points to either a path or intersection membership table, depending on the records selected
//	RIVTable<float,ushort>* membershipTableOne = NULL;
//	RIVTable<float,ushort>* membershipTableTwo = NULL;
//	
//	RIVTable<float,ushort>* pathMembershipOneTable;
//	RIVTable<float,ushort>* pathMembershipTwoTable;
//	
//	RIVTable<float,ushort>* isectMembershipOneTable;
//	RIVTable<float,ushort>* isectMembershipTwoTable;
	
	riv::ColorMap colorMap;
	
	int lastRowsFiltered = 0;
	
	void redisplayWindow();
	void createMembershipData(RIVDataSet<float,ushort>* datasetSource, bool isLeftSet);
	void createMembershipData();
	void resetPointers();
	void filterDataSets();
	//Filter a dataset according to its uniqueness
	void filterDataSet(RIVDataSet<float,ushort>* dataset, HistogramSet<float,ushort>* distributions, bool isLeftSet, float minBound, float maxBound,std::map<int,std::vector<size_t>>& rowBinMembership, riv::RowFilter*& existingFilter);
public:
	static int windowHandle;
	
	RIVSliderView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne,
				  HistogramSet<float,ushort>* distributionsTwo, const riv::ColorMap& colorMap);
	
	static void DrawInstance();
	static void ReshapeInstance(int width, int height);
	static void Mouse(int state,int button,int x,int y);
	static void Motion(int x, int y);
	
	void AddSelectedRecord(const std::string& tableName,const std::string& recordName);
	void RemoveSelectedRecord(const std::string &recordName);
	
	void OnFiltersChanged(RIVDataSet<float,ushort>* dataset);
	void OnDataChanged(RIVDataSet<float,ushort>* source);
	
	void Reshape(int newWidth, int newHeight);
	void Draw();
	bool HandleMouse(int button, int state, int x, int y);
	bool HandleMouseMotion(int x, int y);
};

#endif /* defined(__embree__SliderView__) */
