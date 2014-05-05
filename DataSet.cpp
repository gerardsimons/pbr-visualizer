#include "DataSet.h"

#include <algorithm>

RIVDataSet::RIVDataSet(void) {

}


RIVDataSet::~RIVDataSet(void) {

}

//void RIVDataSet::AddData(string name, vector<int> values) {
//	int_records[name] = values;
//}

void RIVDataSet::AddRecord(RIVRecord record) {
	float_records.push_back(record);
}

pair<float,float>* RIVDataSet::MinMax(int index) {
	RIVRecord *record = &float_records[index];
	return record->MinMax();
}

size_t RIVDataSet::NumberOfRecords() {
	//printf("float_records_size = %lu\n",float_records.size());
	return float_records.size();
}

size_t RIVDataSet::NumberOfValuesPerRecord() {
	//TODO: make sure all columns have the same number of records
	if(!float_records.empty())
	{
		return float_records[0].size();
	}
	else {
		printf("Empty record.");
		return 0;
	}
}

//Get the value of record given by the indices
float* RIVDataSet::GetRecordValue(int recordIndex, int valueIndex) {
	RIVRecord *record = &float_records[recordIndex];
	
	if(filtered_value_indices.size() > 0) {
		for(size_t i = 0 ; i < filtered_value_indices.size() ; i++) {
			int filtered_index = filtered_value_indices[i];
			if(filtered_index == valueIndex) {
				return 0; //Should be filtered out.
			}
		}
	}
	return record->Value(valueIndex);
}

RIVRecord* RIVDataSet::GetRecord(int record) {
	return &float_records[record];
}

void RIVDataSet::ApplyFilters() {
	for(size_t i = 0 ; i < float_records.size() ; i++) {
		RIVRecord *record = &float_records[i];
		for(size_t j = 0 ; j < record->size() ; j++) {
			float *value = record->Value(j);
			for(size_t k = 0 ; k < filters.size() ; k++) {
				Filter *filter = filters[k];
				if(record->name == filter->GetAttributeName() &&!filter->PassesFilter(record->name,*value)) {
					filtered_value_indices.push_back(j);
				}
			}
		}
	}
}

void RIVDataSet::AddFilter(Filter* filter) {
	filters.push_back(filter);
	ComputeFilteredIndices();
}

void RIVDataSet::ClearFilters() {
	filters.clear();
	filtered_value_indices.clear();
}

bool RIVDataSet::HasFilters() {
	return filters.size() != 0;
}

