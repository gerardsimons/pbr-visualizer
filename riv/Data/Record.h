//
//  Record.h
//  RIVDataSet
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __RIVDataSet__Record__
#define __RIVDataSet__Record__

#include <vector>
#include <iostream>
#include <limits>
#include <typeinfo>

#include "Histogram.h"

typedef unsigned short ushort;

template<typename T>
class RIVRecord {
public:
	std::vector<T> values;
	std::pair<T,T> minMax;
	const std::string name;
protected:
	bool minMaxComputed = false;
	//When set to true, any values falling outside the predetermined bounds will be clamped
	bool clampOutliers = false;
public:
    ~RIVRecord() {
        //do jack shit
    }
	RIVRecord(const std::string& name) : name(name) {
		minMaxComputed = false;
	}
	RIVRecord(const std::string& name, const T& min, const T& max, bool clampOutliers) : name(name), clampOutliers(clampOutliers) {
		minMax = std::pair<T,T>(min,max);
		minMaxComputed = true;
	}
	RIVRecord(const std::string& name, const T& min, const T& max) : name(name) {
		minMax = std::pair<T,T>(min,max);
		minMaxComputed = true;
	}
	RIVRecord(const std::string& name, const std::vector<T>& values) : name(name), values(values) {
		minMaxComputed = false;
	};
	void Reserve(const size_t N) {
		values.reserve(N);
	}
	size_t Size() const {
		return values.size();
	}
	T Min() {
		return MinMax().first;
	}
	T Max() {
		return MinMax().second;
	}
	float Value(size_t i) const {
		return values[i];
	}
	void AddValue(const T& value) {
		if(!clampOutliers) {
			values.push_back(value);
			minMaxComputed = false;
		}
		else {
			const std::pair<T,T>& minmax = MinMax();
			values.push_back(std::max(std::min(value,minmax.second),minmax.first));
		}
	}
	std::pair<T,T> MinMax() {
		if(!minMaxComputed) {
			float min = std::numeric_limits<T>::max();
			float max = std::numeric_limits<T>::min();
			for(float value : values) {
				if(value < min) {
					min = value;
				}
				if(value > max) {
					max = value;
				}
			}
			minMax = std::pair<T,T>(min,max);
			minMaxComputed = true;
		}
		return minMax;
	}
	float RelativeValue(const size_t& row) {
		std::pair<T,T> minMax = MinMax();
		T value = Value(row);
		return (value - minMax.first) / (float)(minMax.second - minMax.first);
	}
	void AppendRecord(RIVRecord* otherRecord) {
		if(otherRecord->name != name) {
			throw std::runtime_error("Can only add two records of the same name");
		}
		for(size_t i = 0 ; i < otherRecord->Size() ; ++i) {
			AddValue(otherRecord->Value(i));
		}
	}
	void SetValue(size_t row, T newValue) {
		values[row] = newValue;
	}
	void Clear() {
		values.clear();
		if(!clampOutliers) {
			minMaxComputed = false;
		}
	}
	std::vector<T>* GetValues() {
		return &values;
	}
	RIVRecord* CloneStructure() {
		return new RIVRecord<T>(name,Min(),Max(),clampOutliers);
	}
	Histogram<T> CreateHistogram(const T& lowerBound, const T& upperBound, int bins) {
        if(typeid(lowerBound) == typeid(unsigned short)) {
            return Histogram<T>(name,lowerBound,upperBound,upperBound - lowerBound);
        }
        return Histogram<T>(name,lowerBound,upperBound,bins);
	}
	Histogram<T> CreateHistogram(size_t bins) {
		const std::pair<T,T>& minMax = MinMax();
		
//		return Histogram<T>(name, values, minMax.first,minMax.second,bins);
//		if(typeid(minMax.first) == typeid(unsigned short)) {
//			return CreateHistogram(minMax.first, minMax.second, minMax.second - minMax.first);
//		}
		return CreateHistogram(minMax.first, minMax.second, bins);
	}
    Histogram<T> CreateEmptyHistogram(size_t bins) {
        if(typeid(minMax.first) == typeid(unsigned short)) {
            return Histogram<T>(name,minMax.first,minMax.second, minMax.second - minMax.first);
        }
        return Histogram<T>(name,minMax.first, minMax.second, bins);
    }
};

typedef RIVRecord<float> RIVFloatRecord;
typedef RIVRecord<unsigned short> RIVShortRecord;

#endif