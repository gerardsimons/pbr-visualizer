//#include "DataSet.h"
//
//#include <algorithm>
//
//
////void RIVDataSet::AddData(string name, vector<int> values) {
////	int_records[name] = values;
////}
//
//void RIVDataSet::AddRecord(RIVRecord<float> record) {
//	float_records.push_back(record);
//}
//
//size_t RIVDataSet::NumberOfRecords() {
//	return float_records.size();
//}
//
//size_t RIVDataSet::NumberOfValuesPerRecord() {
//	//TODO: make sure all columns have the same number of records
//	if(!float_records.empty())
//	{
//		return float_records[0].size();
//	}
//	else {
//		printf("Empty record.");
//		return 0;
//	}
//}
//
////Get the value of record given by the indices
//float* RIVDataSet::GetRecordValue(int recordIndex, int valueIndex) {
//	RIVRecord<float> *record = &float_records[recordIndex];
//	
//	if(filtered_values.size() > 0) {
//		bool filtered = filtered_values[valueIndex];
//		if(filtered) return 0;
//	}
//	return record->Value(valueIndex);
//}
//
//RIVRecord<float>* RIVDataSet::GetRecord(int record) {
//	return &float_records[record];
//}
//
//void RIVDataSet::ApplyFilters() {
//	//filtered_value_indices.clear();
//	filtered_values.clear();
//	for(size_t i = 0 ; i < float_records.size() ; i++) {
//		RIVRecord<float> *record = &float_records[i];
//		for(size_t j = 0 ; j < record->size() ; j++) {
//			if(filtered_values[j] == false) {
//				float *value = record->Value(j);
//				for(size_t k = 0 ; k < filters.size() ; k++) {
//					Filter *filter = filters[k];
//					if(!filter->PassesFilter(record->name,*value)) {
//						//filtered_value_indices.push_back(j);
//						filtered_values[j] = true;
//						break; //If it does not pass this filter, it does not matter if it passes the other ones
//					}
//				}
//			}
//		}
//	}
//}
//
//void RIVDataSet::AddFilter(Filter* filter) {
//	filters.push_back(filter);
//}
//
//void RIVDataSet::ClearFilters() {
//	filters.clear();
//	filtered_values.clear();
//	//filtered_value_indices.clear();
//}
//
//bool RIVDataSet::HasFilters() {
//	return filters.size() != 0;
//}
//
