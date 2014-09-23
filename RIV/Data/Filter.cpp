#include "Filter.h"
namespace riv {
	RangeFilter::RangeFilter(const std::string& attributeName, float minValue, float maxValue) : Filter(attributeName) {
		
		//Normalize
		if(minValue <= maxValue) {
			this->minValue = minValue;
			this->maxValue = maxValue;
		} else {
			this->minValue = maxValue;
			this->maxValue = minValue;
		}
	}

	bool RangeFilter::PassesFilter(const std::string& attribute,float value) {
		if(attribute == attributeName) {
			bool passes = (value >= minValue && value <= maxValue);
			return passes;
		}
		return true;
	}

	bool RangeFilter::PassesFilter(const std::string& attribute,unsigned short value) {
		if(attribute == attributeName) {
			return (value >= minValue && value <= maxValue);
		}
		return true;
	}
	
	//Discrete filter stuff
	DiscreteFilter::DiscreteFilter(const std::string& name, float value) : Filter(name) {
		this->value = value;
	}
	
	bool DiscreteFilter::PassesFilter(const std::string& name, float value) {
		throw "Not yet implemented.";
	}
	bool DiscreteFilter::PassesFilter(const std::string &name, unsigned short value) {
		return attributeName == name && value == this->value;
	}
}