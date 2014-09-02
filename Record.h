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

typedef unsigned short ushort;


class RIVRecord {
public:
    std::string name;
protected:
    bool minMaxComputed;
public:
    virtual ~RIVRecord() {
        //do jack shit
    }
    virtual size_t Size() const = 0;
    virtual float ScaleValue(const size_t& row) = 0;
};

class RIVFloatRecord : public RIVRecord {
private:
    std::vector<float> values;
    std::pair<float,float> minMax;
public:
    RIVFloatRecord(const std::string& name) { this->name = name; };
    RIVFloatRecord(const std::string& name, const std::vector<float>& values) { this->name = name; this->values = values; minMaxComputed = false; };
    float Value(const size_t& i) const { return values[i]; };
    void SetValues(std::vector<float> _values) {
        values = _values;
        minMaxComputed = false;
    }
    size_t Size() const {
        return values.size();
    }
    float ScaleValue(const size_t& row) {
        std::pair<float,float> minMax = MinMax();
        const float& value = Value(row);
        return (value - minMax.first) / (minMax.second - minMax.first);
    }
    float Min() {
        return MinMax().first;
    }
    float Max() {
        return MinMax().second;
    }
    const std::pair<float,float>& MinMax() {
//        printf("MINMAX OF RECORD \"%s\" CALLED\n",name.c_str());
		if(!minMaxComputed) {
			float min = std::numeric_limits<float>::max();
			float max = std::numeric_limits<float>::min();
			for(float value : values) {
                if(value < min) {
                    min = value;
                }
                if(value > max) {
                    max = value;
                }
            }
			minMax = std::pair<float,float>(min,max);

			minMaxComputed = true;
		}
        return minMax;
	}
    std::vector<float>* GetValuesPointer() {
        return &values;
    }
    void Print() {
        printf("RIVFloatRecord %s has %lu records. ",name.c_str(), values.size());
        minMaxComputed ? printf(" (min,max) = (%f,%f)\n",minMax.first, minMax.second) : printf("no minmax yet computed.\n");
    }
};

class RIVUnsignedShortRecord : public RIVRecord {
private:
    std::pair<ushort,ushort> minMax;
    std::vector<ushort> values;
public:
    RIVUnsignedShortRecord(const std::string& name) {
        this->name = name;
        minMaxComputed = false;
    };
    RIVUnsignedShortRecord(const std::string& name, const std::vector<ushort>& values) {
        this->name = name;
        this->values = values;
    }
    RIVUnsignedShortRecord(std::string name_, ushort min, ushort max) {
        name = name_;
        minMax.first = min;
        minMax.second = max;
        minMaxComputed = true;
    };
    ushort Value(size_t i) { return values[i]; };
    void Print() {
        std::cout << "RIVRecord " << name << " containing " << values.size() << " " << typeid(int).name() << " values.\n";
    }
    std::vector<ushort>& GetValues() {
        return values;
    }
    void SetValues(std::vector<ushort> _values) {
        values = _values;
    }
    size_t Size() const {
        return values.size();
    }
    float ScaleValue(const size_t& row) {
        std::pair<ushort,ushort> minMax = MinMax();
        const ushort& value = Value(row);
        float scaleValue = (value - minMax.first) / (float)(minMax.second - minMax.first);
        return scaleValue;
    }
    float Min() {
        return MinMax().first;
    }
    float Max() {
        return MinMax().second;
    }
    const std::pair<ushort,ushort>& MinMax() {
		if(!minMaxComputed) {
			float min = std::numeric_limits<float>::max();
			float max = std::numeric_limits<float>::min();
            
			if(!values.empty()) {
				for(size_t i = 0 ; i < values.size() ; i++) {
					auto value = values[i];
					if(value > max) {
						max = value;
					}
					if(value < min) {
						min = value;
					}
				}
			}
			minMax = std::pair<float,float>(min,max);
			//TODO: min_max caching does not work properly, uncomment below to enable caching!
			minMaxComputed = true;
		}
		return minMax;
	}
};



#endif /* defined(__RIVDataSet__Record__) */
