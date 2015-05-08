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
#include <map>
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
	
    RIVColorProperty* colorPropertyOne;
    RIVColorProperty* colorPropertyTwo = NULL;
    
	std::vector<ParallelCoordsAxisInterface*> axisInterfaces;
	std::vector<std::pair<size_t,size_t>> axisOrder;
	
	const std::string tableName;
	std::tuple<std::vector<ParallelCoordsAxis<Ts>*>...> axes;
	
    ParallelCoordsAxisGroup(const std::string& tableName, RIVColorProperty* colorPropertyOne, RIVColorProperty* colorPropertyTwo) : tableName(tableName), colorPropertyOne(colorPropertyOne),colorPropertyTwo(colorPropertyTwo) {
		
	}
    ParallelCoordsAxisGroup(const std::string& tableName, RIVColorProperty* colorProperty) : tableName(tableName), colorPropertyOne(colorProperty)  {
        
    }
    
	void SwapAxes(size_t swapIndexOne, size_t swapIndexTwo) {
		auto axisOne = axisInterfaces[swapIndexOne];
		auto axisTwo = axisInterfaces[swapIndexTwo];
		
		auto xTemp = axisOne->x;
		
		axisOne->x = axisTwo->x;
		axisTwo->x = xTemp;
		
		axisInterfaces[swapIndexOne] = axisTwo;
		axisInterfaces[swapIndexTwo] = axisOne;
	}
	
	void Reorder(int oldPosition, int newPosition) {
		//How much should the other axes scoot over?
		if(oldPosition < axisInterfaces.size() - 1 && newPosition < axisInterfaces.size() - 1 && newPosition != oldPosition) {
			
			axisInterfaces[oldPosition]->x = axisInterfaces[newPosition]->x;
			if(newPosition > oldPosition) {
				for(int i = newPosition + 1 ; i > newPosition ; --i) {
					axisInterfaces[i]->x = axisInterfaces[i-1]->x;
				}
			}
			else {
				for(int i = newPosition ; i < oldPosition ; ++i) {
					printf("Shift axis %s from %d to ",axisInterfaces[i]->name.c_str(),axisInterfaces[i]->x);
					axisInterfaces[i]->x = axisInterfaces[i+1]->x;
					printf("%d.\n",axisInterfaces[i+1]->x);
				}
			}
			auto copy = axisInterfaces[oldPosition];
			axisInterfaces.erase(axisInterfaces.begin() + oldPosition);
			axisInterfaces.insert(axisInterfaces.begin() + newPosition,copy);
			
			//It has a right neighbor, shift all those right neighbors to the right
//			if(newPosition < axisInterfaces.size() - 1) {
//				for(int i = 0 ; i < axisInterfaces.size() ; ++i) {
//					
//				}
//			}
//			else {
//				
//			}
		}
	}
	
    RIVColorProperty* GetColorProperty(ushort datasetId) {
        if(datasetId) {
            return colorPropertyTwo;
        }
        else return colorPropertyOne;
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
		axisInterfaces.push_back(axis);
		return tAxes->at(tAxes->size() - 1);
	}
	
	template<typename U>
	ParallelCoordsAxis<U>* CreateAxis(RIVRecord<U>* recordOne,RIVRecord<U>* recordTwo, int x, int y, int axisWidth, int axisHeight, U min, U max, const std::string& name, int divisionCount,const  Histogram<U>& histogramOne, const Histogram<U>& histogramTwo) {
		std::vector<ParallelCoordsAxis<U>*>* tAxes = GetAxes<U>();
		auto axis = new ParallelCoordsAxis<U>(x,y,axisWidth,axisHeight,min,max,name,recordOne,recordTwo,divisionCount,histogramOne, histogramTwo);
		tAxes->push_back(axis);
		axisInterfaces.push_back(axis);
		return tAxes->at(tAxes->size() - 1);
	}

};

#endif /* defined(__Afstuderen__ParallelCoordsAxisGroup__) */