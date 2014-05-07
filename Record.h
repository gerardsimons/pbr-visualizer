//TODO: template?
#ifndef RIVRECORD_H
#define RIVRECORD_H

#include <stdio.h>
#include <string>
#include <vector>

template <class T>
class RIVRecord
{

private:
	std::vector<T> values;
public:
	//Name of the data
	std::string name;
    bool min_max_computed; //Because null is not possible


    std::pair<T,T> min_max; //Cached min max

	//Constructor & Destructor
	RIVRecord(std::string _name, std::vector<T> _values) { name = _name; values = _values; min_max_computed = false; };
	~RIVRecord(void) { /* ... */ };

	size_t size() {
		return values.size();
	}

	//Templated methods
	T* Value(int index) {
		return &values[index];
	}

	std::pair<T,T>* MinMax() {
		if(!min_max_computed) {
			float min = std::numeric_limits<float>::max();
			float max = std::numeric_limits<float>::min();

			if(!values.empty()) {
				for(size_t i = 0 ; i < values.size() ; i++) {
					float value = values[i];
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
		return &min_max;
	}
};

#endif
