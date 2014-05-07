//TODO: template?
#ifndef RIVRECORD_H
#define RIVRECORD_H

#include <stdio.h>
#include <string>
#include <vector>

class RIVRecord {
public:
	std::string name;
	virtual size_t Size() = 0;
}

class RIVFloatRecord
{

private:
	std::vector<float> values;
public:
	//Properties
	std::string name;
    
    bool min_max_computed; //Because null is not possible
    std::pair<float,float> min_max; //Cached min max

	//Constructor & Destructor
	RIVRecord(std::string _name, std::vector<float> _values) { name = _name; values = _values; min_max_computed = false; };
	~RIVRecord(void) { /* ... */ };

	size_t Size();
	
	float* Value(int index);
	std::pair<float,float>* MinMax();
};

class RIVIntegerRecord {
	std::vector<float> values;
}

#endif
