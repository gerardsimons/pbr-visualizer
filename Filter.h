
#ifndef FILTER_H
#define FILTER_H

#include <string>

class Filter
{
protected:
	Filter(std::string attributeName) { this->attributeName = attributeName; };
public:
	virtual bool PassesFilter(std::string name, float value) = 0;
    std::string attributeName;
//	std::string GetAttributeName();
	~Filter(void);
    virtual void Print() = 0;
};

//class IndexFilter : public Filter<size_t> {
//private:
//    std::vector<size_t> filteredIndices;
//public:
//    IndexFilter(const std::string& attributeName, const std::vector<size_t>& filteredIndices);
//    bool PassesFilter(std::string name, size_t index);
//};

class RangeFilter : public Filter {
private:
	float minValue;
	float maxValue;
public :
	RangeFilter(std::string attributeName, float minValue, float maxValue);
	~RangeFilter();
	bool PassesFilter(std::string name, float value);
    bool PassesFilter(std::string name, unsigned short value);
    virtual void Print() {
        printf("RangeFilter attributeName = %s (min,max) = (%f,%f)\n",attributeName.c_str(),minValue,maxValue);
    }
};
#endif
