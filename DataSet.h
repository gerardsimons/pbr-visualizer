#ifndef DATASET_H
#define DATASET_H

#include "Record.h"
#include "Filter.h"
#include "RIVVector.h"

#include <map>
#include <string>
#include <vector>

template<typename... Ts>
class RIVDataSet {
public:
    RIVVector<RIVRecord<Ts...>> records;
    RIVVector<Filter*> filters;
    
};

//class RIVDataSet
//{
//private :
//	//map<string,vector<int>> int_records;
//    std::vector<RIVRecord<float>> float_records;
//
//    std::vector<Filter*> filters;
//
//    std::map<int,bool> filtered_values; //Whether or not the value was filtered by any applied filters
//	//vector<int> filtered_value_indices; //Record value indices that do not pass the filters. //CAUTION: Obsolete, not efficient
//
//public:
//	RIVDataSet(void);
//	~RIVDataSet(void);
//	//void AddData(string name,vector<int>);
//	void AddRecord(RIVRecord<float>);
//    std::pair<float,float>* MinMax(int);
//	size_t NumberOfRecords();
//	size_t NumberOfValuesPerRecord();
//	RIVRecord<float>* GetRecord(int);
//	float* GetRecordValue(int,int);
//	void AddFilter(Filter*);
//	bool HasFilters();
//	void ClearFilters();
//	void ApplyFilters();
//};

#endif

