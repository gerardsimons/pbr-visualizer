#ifndef DATASET_H
#define DATASET_H

#include "Record.h"
#include "Filter.h"
#include "RIVVector.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>

//template<typename ...Ts>
//class RIVDataSet {
//private:
//    std::map<int,bool> filteredIndices;
//public:
//
//    RIVDataSet();
//    ~RIVDataSet();
//    
//    RIVVector<RIVRecord<Ts...>> records;
//    std::vector<Filter *> filters;
//    
//    std::map<int,bool> valueFilterMap;
//    
//    <typename T>
//    RIVRecord<T> GetRecord(int recordIndex) {
//        return records.template data<RIVRecord<T>();
//    };
//    
//    <typename T>
//    RIVValue<T> GetRecordValue(int recordIndex,int valueIndex) {
//        
//    }
//    
//    void AddFilter(const Filter &filter) {
//        filters.push_back(filter);  
//        for(auto i : records.template data<RIVRecord<Ts...>>()) {
//            std::cout << i.name << ": [";
//            for(size_t j = 0 ; j < i.values.size() ; j++) {
//                float *value = i->Value(j);
//                std::cout << &value << " ";
//                if(filteredIndices[j]) continue; //Already filtered
//                if(filter.PassesFilter(i.name, j) == false) {
//                    filteredIndices[j] = true;
//                }
//            }
//            std::cout << "]";
//        }
//    }
//};

class RIVDataSet
{
private :
	//map<string,vector<int>> int_records;
    std::vector<RIVRecord<float>> float_records;
    std::vector<RIVRecord<unsigned short>> short_records;

    std::vector<Filter*> filters;

    std::map<size_t,bool> filteredIndices;

public:
    
    void AddRecord(RIVRecord<float> record) {
        float_records.push_back(record);
    }
    
    void AddRecord(RIVRecord<unsigned short> record) {
        short_records.push_back(record);
    }
    
    void AddFilter(Filter* filter) {
        filters.push_back(filter);
        
        //See what value indices I should filter, add these to the map for fast retrieval later
        for(auto i : float_records) {
            for(size_t j = 0 ; j < i.values.size() ; j++) {
                if(filteredIndices[j]) continue; //Already filtered
                if(filter->PassesFilter(i.name, j) == false) {
                    filteredIndices[j] = true;
                }
            }
        }
        //Ahh the duplicated code, the horror
        for(auto i : short_records) {
            for(size_t j = 0 ; j < i.values.size() ; j++) {
                if(filteredIndices[j]) continue; //Already filtered
                if(filter->PassesFilter(i.name, j) == false) {
                    filteredIndices[j] = true;
                }
            }
        }
    }
    
    void ClearFilters() {
        filteredIndices.clear();
        filters.clear();
    }
    
    size_t TotalNumberOfRecords() {
        return float_records.size() + short_records.size();
    }
    
    size_t NumberOfFloatRecords() {
        return float_records.size();
    }
    
    size_t NumberOfShortRecords() {
        return short_records.size();
    }
    
    RIVRecord<float>* GetFloatRecord(size_t recordIndex) {
        return &float_records[recordIndex];
    }
    
    RIVRecord<unsigned short>* GetUnsignedIntRecord(size_t recordIndex) {
        return &short_records[recordIndex];
    }
    
    size_t NumberOfValuesPerRecord() {
        //TODO: make sure all columns have the same number of records
        if(!float_records.empty())
        {
            return float_records[0].values.size();
        }
        else {
            printf("Empty record.");
            return 0;
        }
    }
    
    //Get the value of record given by the indices
    float* GetRecordValue(int recordIndex, size_t valueIndex) {
        RIVRecord<float> *record = &float_records[recordIndex];
        
        if(filteredIndices.size() > 0) {
            bool filtered = filteredIndices[valueIndex];
            if(filtered) return 0;
        }
        return record->Value(valueIndex);
    }

//	void AddRecord(RIVRecord<float>);
//    void AddRecord(RIVRecord<unsigned short>);
//    
//    //std::pair<float,float>* MinMax(int);
//	size_t NumberOfRecords();
//	size_t NumberOfValuesPerRecord(); //Assuming all records have the same number of values
//    
//	RIVRecord<float>* GetRecord(int);
//	float* GetRecordValue(int,int);
//	void AddFilter(Filter*);
//	bool HasFilters();
//	void ClearFilters();
//	void ApplyFilters();
};

#endif

