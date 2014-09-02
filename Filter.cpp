#include "Filter.h"

Filter::~Filter() {
	printf("Filter base class destructor called.");
	delete &attributeName;
}

RangeFilter::~RangeFilter() {
	delete &minValue;
	delete &maxValue;
}

RangeFilter::RangeFilter(std::string attributeName, float minValue, float maxValue) : Filter(attributeName) {
    
    //Normalize
    if(minValue <= maxValue) {
        this->minValue = minValue;
        this->maxValue = maxValue;
    } else {
        this->minValue = maxValue;
        this->maxValue = minValue;
    }
}

bool RangeFilter::PassesFilter(std::string attribute,float value) {
	if(attribute == attributeName) {
        bool passes = (value >= minValue && value <= maxValue);
        return passes;
	}
	return true;
}

bool RangeFilter::PassesFilter(std::string attribute,unsigned short value) {
	if(attribute == attributeName) {
        return (value >= minValue && value <= maxValue);
	}
	return true;
}