#ifndef DATASET_H
#define DATASET_H

#include "Record.h"

#include <map>
#include <string>
#include <vector>

using namespace std;

class RIVDataSet
{
private :
	//map<string,vector<int>> int_records;
	vector<RIVRecord> float_records;
public:
	RIVDataSet(void);
	~RIVDataSet(void);
	//void AddData(string name,vector<int>);
	void AddRecord(RIVRecord);
	pair<float,float> MinMax(int);
	size_t NumberOfRecords();
	size_t NumberOfValuesPerRecord();
	RIVRecord* GetRecord(int);
};

#endif

