#include "DataSet.h"

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

pair<float,float> RIVDataSet::MinMax(int index) {
	RIVRecord *record = &float_records[index];
	return record->MinMax();
}

size_t RIVDataSet::NumberOfRecords() {
	printf("float_records_size = %lu\n",float_records.size());
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

//Get the records for a given index
RIVRecord* RIVDataSet::GetRecord(int columnIndex) {
	return &float_records[columnIndex];
}
