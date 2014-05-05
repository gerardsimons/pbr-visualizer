#include "Filter.h"

Filter::~Filter() {
	printf("Filter base class destructor called.");
	delete &attributeName;
}

std::string Filter::GetAttributeName() {
	return attributeName;
}

RangeFilter::~RangeFilter() {
	delete &minValue;
	delete &maxValue;
}

RangeFilter::RangeFilter(std::string attributeName, float minValue, float maxValue) : Filter(attributeName) {
	this->minValue = minValue;
	this->maxValue = maxValue;
}

bool RangeFilter::PassesFilter(std::string attribute,float value) {
	if(attribute == attributeName) {
		 return (value >= minValue && value <= maxValue);
	}
	return true;
}