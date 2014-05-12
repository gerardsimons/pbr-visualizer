//TODO: template?
#ifndef RIVRECORD_H
#define RIVRECORD_H

#include <stdio.h>
#include <string>
#include <vector>

#include "RIVVector.h"

//template<typename ...Ts>
//class RIVRecord {
//public:
//    std::string name;
//    std::vector<Ts...> values;
//    RIVRecord(std::string name) {
//        this->name = name;
//    }
//    
//    RIVRecord();
//    
//    template<typename T>
//    void AddValue(T value) {
//        values.push_back(value);
//    }
//};


template <typename T>
class RIVRecord
{

private:

public:
	//Name of the data
	std::string name;
    std::vector<T> values;
    
    bool min_max_computed; //Because null is not possible

    std::pair<T,T> min_max; //Cached min max

	//Constructor & Destructor
	RIVRecord(std::string _name) { name = _name; };
    ~RIVRecord() {
        /* Destroy some stuff? */
    }
    
    void SetValues(std::vector<T> values) {
        this->values = values;
    }

	//Templated methods
	T* Value(size_t index) {
		return &values[index];
	}

	std::pair<T,T>& MinMax() {
		if(!min_max_computed) {
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
			min_max = std::pair<float,float>(min,max);
			//TODO: min_max caching does not work properly, uncomment below to enable caching!
			min_max_computed = true;
		}
		return min_max;
	}
};


#endif
