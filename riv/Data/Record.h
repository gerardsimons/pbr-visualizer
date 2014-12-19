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
    bool minMaxComputed;
	//When set to true, any values falling outside the predetermined bounds will be clamped
	bool clampOutliers = false;
public:
    ~RIVRecord() {
        //do jack shit
    }
	RIVRecord(const std::string& name) : name(name) {
		
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
	void Clear() {
		values.clear();
	}
	std::vector<T>* GetValues() {
		return &values;
	}
	RIVRecord* CloneStructure() {
		if(clampOutliers) {
			return new RIVRecord<T>(name,Min(),Max(),clampOutliers);
		}
		else {
			return new RIVRecord<T>(name);
		}
	}
	Histogram<T>* CreateHistogram(const T& lowerBound, const T& upperBound, int bins) {
		return new Histogram<T>(name, values, lowerBound,upperBound,bins);
	}
	Histogram<T>* CreateHistogram(size_t bins) {
		const std::pair<T,T>& minMax = MinMax();
		return new Histogram<T>(name, values, minMax.first,minMax.second,bins);
	}
};

typedef RIVRecord<float> RIVFloatRecord;
typedef RIVRecord<unsigned short> RIVShortRecord;

//
//class RIVFloatRecord : public RIVRecord {
//private:
//    std::vector<float> values;
//    std::pair<float,float> minMax;
//public:
//	RIVFloatRecord(const std::string& name) { this->name = name; minMaxComputed = false; };
//
//
//    void SetValues(std::vector<float> _values) {
//        values = _values;
//        minMaxComputed = false;
//    }
//    size_t Size() const {
//        return values.size();
//    }
//
//	void AddValue(float newValue) {
//		values.emplace_back(newValue);
//		minMaxComputed = false;
//	}
//
//    std::vector<float>* GetValuesPointer() {
//        return &values;
//    }
//    void Print() {
//        printf("RIVFloatRecord %s has %lu records. ",name.c_str(), values.size());
//        minMaxComputed ? printf(" (min,max) = (%f,%f)\n",minMax.first, minMax.second) : printf("no minmax yet computed.\n");
//    }
//};
//
//class RIVUnsignedShortRecord : public RIVRecord {
//private:
//    std::pair<ushort,ushort> minMax;
//    std::vector<ushort> values;
//public:
//    RIVUnsignedShortRecord(const std::string& name) {
//        this->name = name;
//        minMaxComputed = false;
//    };
//    RIVUnsignedShortRecord(const std::string& name, const std::vector<ushort>& values) {
//        this->name = name;
//        this->values = values;
//    }
//    RIVUnsignedShortRecord(std::string name_, ushort min, ushort max) {
//        name = name_;
//        minMax.first = min;
//        minMax.second = max;
//        minMaxComputed = true;
//    };
//    ushort Value(size_t i) { return values[i]; };
//    void Print() {
//        std::cout << "RIVRecord " << name << " containing " << values.size() << " " << typeid(int).name() << " values.\n";
//    }
//    std::vector<ushort>* GetValues() {
//        return &values;
//    }
//	void AddValue(ushort newValue) {
//		values.emplace_back(newValue);
//		minMaxComputed = false;
//	}
//    void SetValues(std::vector<ushort> _values) {
//        values = _values;
//    }
//    size_t Size() const {
//        return values.size();
//    }
//    float ScaleValue(const size_t& row) {
//        std::pair<ushort,ushort> minMax = MinMax();
//        const ushort& value = Value(row);
//        float scaleValue = (value - minMax.first) / (float)(minMax.second - minMax.first);
//        return scaleValue;
//    }
//    float Min() {
//        return MinMax().first;
//    }
//    float Max() {
//        return MinMax().second;
//    }
//    const std::pair<ushort,ushort>& MinMax() {
//		if(!minMaxComputed) {
//			float min = std::numeric_limits<float>::max();
//			float max = std::numeric_limits<float>::min();
//            
//			if(!values.empty()) {
//				for(size_t i = 0 ; i < values.size() ; i++) {
//					auto value = values[i];
//					if(value > max) {
//						max = value;
//					}
//					if(value < min) {
//						min = value;
//					}
//				}
//			}
//			minMax = std::pair<float,float>(min,max);
//			//TODO: min_max caching does not work properly, uncomment below to enable caching!
//			minMaxComputed = true;
//		}
//		return minMax;
//	}
//};
//


#endif /* defined(__RIVDataSet__Record__) */
