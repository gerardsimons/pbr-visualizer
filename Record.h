//TODO: template?

#include <stdio.h>
#include <string>
#include <vector>


class RIVRecord
{

private:
	std::vector<float> values;
public:
	//Properties
	std::string name;

	//Constructor & Destructor
	RIVRecord(std::string _name, std::vector<float> _values) { name = _name; values = _values; };
	~RIVRecord(void) { /* ... */ };

	size_t size();
	float Value(int index);
	std::pair<float,float> MinMax();
};
