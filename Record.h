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
    virtual size_t Size() = 0;
};

class RIVFloatRecord : public RIVRecord {
    std::vector<float> values;
        std::pair<float,float> minMax;
public:
    RIVFloatRecord(std::string _name) { name = _name; };
    float Value(size_t i) { return values[i]; };
    void Print() {
        std::cout << "RIVRecord " << name << " containing " << values.size() << " " << typeid(float).name() << " values.\n";
    }
    void SetValues(std::vector<float> _values) {
        values = _values;
    }
    size_t Size() {
        return values.size();
    }
    
    const std::pair<float,float>& MinMax() {
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

class RIVUnsignedShortRecord : public RIVRecord {
private:
    std::pair<ushort,ushort> minMax;
public:
    RIVUnsignedShortRecord(std::string _name) { name = _name; };
    std::vector<ushort> values;
    ushort Value(size_t i) { return values[i]; };
    void Print() {
        std::cout << "RIVRecord " << name << " containing " << values.size() << " " << typeid(int).name() << " values.\n";
    }
    void SetValues(std::vector<ushort> _values) {
        values = _values;
        minMaxComputed = false;
    }
    size_t Size() {
        return values.size();
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
