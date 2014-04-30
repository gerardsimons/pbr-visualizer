#include "Record.h"

size_t RIVRecord::size() {
	return values.size();
}

std::pair<float,float> RIVRecord::MinMax() {
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

	return std::pair<float,float>(min,max);
	//return std::pair<float,float>(1.F,1.F);
}

float RIVRecord::Value(int index) {
	return values[index];
}
