#ifndef DATASET_H
#define DATASET_H

#include "Record.h"
#include "Filter.h"

#include <map>
#include <string>
#include <vector>

using namespace std;

class RIVDataSet
{
private :
	//map<string,vector<int>> int_records;
	vector<RIVRecord> float_records;

	vector<Filter*> filters;
	vector<int> filtered_value_indices; //Record value indices that do not pass the filters.

	void ApplyFilters();
	
public:
	RIVDataSet(void);
	~RIVDataSet(void);
	//void AddData(string name,vector<int>);
	void AddRecord(RIVRecord);
	pair<float,float>* MinMax(int);
	size_t NumberOfRecords();
	size_t NumberOfValuesPerRecord();
	RIVRecord* GetRecord(int);
	float* GetRecordValue(int,int);
	void AddFilter(Filter*);
	bool HasFilters();
	void ClearFilters();
};

#endif

