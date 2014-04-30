

#include <map>
#include <string>
#include <vector>

using namespace std;

class RIVDataSet
{
private :
	map<string,vector<int>> int_values;
	map<string,vector<float>> float_values;
public:
	RIVDataSet(void);
	~RIVDataSet(void);
	bool AddData(string name,vector<int>*);
	bool AddData(string name,vector<float>*);
};

