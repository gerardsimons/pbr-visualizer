
#ifndef FILTER_H
#define FILTER_H

#include <string>

class Filter
{
protected:
	std::string attributeName;
	Filter(std::string attributeName) { this->attributeName = attributeName; };
public:
	virtual bool PassesFilter(std::string name, float value) = 0;
	std::string GetAttributeName();
	~Filter(void);
};

class RangeFilter : public Filter {
private:
	float minValue;
	float maxValue;
public :
	RangeFilter(std::string attributeName, float minValue, float maxValue);
	~RangeFilter();
	bool PassesFilter(std::string name, float value);
};
#endif

