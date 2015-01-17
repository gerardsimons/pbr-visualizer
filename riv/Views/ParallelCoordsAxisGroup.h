//
//  ParallelCoordsAxisGroup.h
//  Afstuderen
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__ParallelCoordsAxisGroup__
#define __Afstuderen__ParallelCoordsAxisGroup__

#include "ParallelCoordsAxis.h"
#include "../Data/Table.h"

#include <tuple>
#include <vector>

template<typename... Ts>
class ParallelCoordsAxisGroup {
private:
public:
	
	bool isSelected = false;
	
	~ParallelCoordsAxisGroup() {
		tuple_for_each(axes, [&](auto tAxes) {
//			deletePointerVector(tAxes);
		});
	}
	
	std::vector<ParallelCoordsAxisInterface*> axisOrder;
	std::string tableName;
	std::tuple<std::vector<ParallelCoordsAxis<Ts>*>...> axes;
	
	ParallelCoordsAxisGroup(const std::string& tableName) : tableName(tableName) {
		
	}
	
	void SwapAxes(size_t swapIndexOne, size_t swapIndexTwo) {
		auto oldAxis = axes[swapIndexOne];
		
		axes[swapIndexOne] = axes[swapIndexTwo];
		axes[swapIndexTwo] = oldAxis;
	
		axes[swapIndexTwo]->x = axes[swapIndexOne]->x;
		axes[swapIndexOne]->x = oldAxis->x;
	}
	
	template<typename U>
	std::vector<ParallelCoordsAxis<U>*>* GetAxes() {
		return &std::get<std::vector<ParallelCoordsAxis<U>*>>(axes);
	}
	
	template<typename U>
	void AddAxis(ParallelCoordsAxis<U*>& newAxis) {
		auto axes = GetAxes<U>();
		axes->push_back(newAxis);
	}
	
	template<typename U>
	ParallelCoordsAxis<U>* CreateAxis(RIVRecord<U>* record, int x, int y, int axisWidth, int axisHeight, U min, U max, const std::string& name, int divisionCount,const Histogram<U>& histogramOne) {
		std::vector<ParallelCoordsAxis<U>*>* tAxes = GetAxes<U>();
		auto axis = new ParallelCoordsAxis<U>(x,y,axisWidth,axisHeight,min,max,name,record,divisionCount,histogramOne);
		tAxes->push_back(axis);
		axisOrder.push_back(axis);
		return tAxes->at(tAxes->size() - 1);
	}
	
	template<typename U>
	ParallelCoordsAxis<U>* CreateAxis(RIVRecord<U>* record, int x, int y, int axisWidth, int axisHeight, U min, U max, const std::string& name, int divisionCount,const  Histogram<U>& histogramOne, const Histogram<U>& histogramTwo) {
		std::vector<ParallelCoordsAxis<U>*>* tAxes = GetAxes<U>();
		auto axis = new ParallelCoordsAxis<U>(x,y,axisWidth,axisHeight,min,max,name,record,divisionCount,histogramOne, histogramTwo);
		tAxes->push_back(axis);
		axisOrder.push_back(axis);
		return tAxes->at(tAxes->size() - 1);
	}
	
//	ParallelCoordsAxis<ushort>* CreateAxis(RIVRecord<ushort>* record, int x, int y, int axisWidth, int axisHeight, ushort min, ushort max, const std::string& name, int divisionCount, int bins) {
//		std::vector<ParallelCoordsAxis<ushort>*>* tAxes = GetAxes<ushort>();
//		tAxes->push_back(new ParallelCoordsAxis<ushort>(x,y,axisWidth,axisHeight,min,max,name,record,divisionCount,));
//		return tAxes->at(tAxes->size() - 1);
//	}
	
//    ParallelCoordsAxisGroup* connectedGroup = 0;
//    ParallelCoordsAxis* connectorAxis;
	
};

#endif /* defined(__Afstuderen__ParallelCoordsAxisGroup__) */