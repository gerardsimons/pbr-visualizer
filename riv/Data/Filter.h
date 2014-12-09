
#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>

typedef unsigned short ushort;


#include "Reference.h"

//class RIVReference;
class RIVTableInterface;

namespace riv {
	template <typename T>
	class Filter
	{
	private:
		size_t fidCounter = 1;
	protected:
		size_t fid; //The id of the filter, automatically created to be unique for every filter
	public:
		T type; //TODO: WARNING: This is a hack, I sometimes want to get decltype of the filter template argument, but am unsure how to do it any canonical way, so I do it like this
		size_t GetId();
		//Only used internally

//		virtual bool PassesFilter(RIVTableInterface* table,size_t row) = 0;
		virtual bool PassesFilter(const std::string& name, T value) = 0;
		virtual bool AppliesToAttribute(const std::string& name) = 0;
//		virtual bool AppliesToTable(const std::string& name) = 0;
//		virtual bool PassesFilter(const std::string& name, unsigned short value) = 0;
		//Determines whether this filter is targeted at the given table
//		virtual std::vector<std::string> GetAttributes() = 0;
		virtual void Print() {
			printf("Generic filter object\n");
		}
	};
	
	template <typename T>
	class SingularFilter : public Filter<T> {
	protected:
		std::string attributeName;
		SingularFilter(const std::string& attributeName) : attributeName(attributeName) {
			
		}
	public:
		std::string GetAttribute() {
			return attributeName;
		}
		virtual bool AppliesToAttribute(const std::string& name) {
			return attributeName == name;
		}
//		bool AppliesToTable(const RIVTable* table);
		std::vector<std::string> GetAttributes();
	};

	template <typename T>
	class RangeFilter : public SingularFilter<T> {
	private:
		T minValue;
		T maxValue;
	public :
		RangeFilter(const std::string& attributeName, T minValue, T maxValue) : SingularFilter<T>(attributeName), minValue(minValue), maxValue(maxValue) {
			
		}
		bool PassesFilter(const std::string& name, T value) {
			return (SingularFilter<T>::AppliesToAttribute(name) >= minValue && value <= maxValue);
		}
		void Print() {
//			printf("RangeFilter [attributeName = %s, (min,max) = (%f,%f)]\n",attributeName.c_str(),minValue,maxValue);
		}
	};
	
	template <typename T>
	class DiscreteFilter : public SingularFilter<T> {
	private:
		T value;
//		float epsilon = 0.00001F;
	public:
		DiscreteFilter(const std::string& attributeName, float value);
		float GetValue();
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
//			printf("DiscreteFilter [attributeName = %s, value = %f]\n",attributeName.c_str(),value);
		}
	};
	
	template<typename T>
	class CompoundFilter : public Filter<T> {
	protected:
		std::vector<Filter<T>*> filters;
		CompoundFilter(const std::vector<Filter<T>*>& filters) {
			this->filters = filters;
		}
		CompoundFilter(Filter<T>* f) {
			filters.push_back(f);
		}
		CompoundFilter( const CompoundFilter& other ) {
			filters = other.filters;
		}
		CompoundFilter& operator=( const CompoundFilter& assigned ) {
			//			GroupFilter newFilter(assigned.filters,assigned.sourceTable);
			//			filters = assigned.filters;
			filters = assigned.filters;
			return *this;
		}
		void Print() {
			printf("Compound filter containing : \n");
			for(Filter<T>* f : filters) {
				printf("\t");
				f->Print();
			}
		}
	public:
		~CompoundFilter() {
			for(Filter<T>* f : filters) {
				delete f;
			}
		}
		bool AppliesToAttribute(const std::string& name) {
			for(Filter<T>* filter : filters) {
				if(filter->AppliesToAttribute(name)) {
					return true;
				}
			}
			return false;
		}
		std::vector<Filter<T>*> GetFilters();
		size_t Size();
		void AddFilter(Filter<T>* newFilter);
		std::vector<std::string> GetAttributes();
	};
	
	template<typename T>
	class ConjunctiveFilter : public CompoundFilter<T> {
	public:
		ConjunctiveFilter(const std::vector<Filter<T>*>& filters);
		
	};
	
	template<typename T>
	class DisjunctiveFilter : public CompoundFilter<T> {
	public:
		DisjunctiveFilter(const std::vector<Filter<T>*>& filters);
		bool PassesFilter(const std::string &name, const T& value);
//		bool AppliesToTable(const RIVTable* table);
		std::vector<std::string> AllAttributes();
//		bool PassesFilter(RIVTable<T>* table, size_t row);
	};
	
	//This composite filter takes a table, searches its reffering rows to which its filters apply and tests the group of row to see if ALL the filters are met by atleast one row in the group
	//Of referring rows
	template<typename T>
	class GroupFilter : public CompoundFilter<T> {
	private:
		RIVReference* ref = NULL;
//		RIVTable* sourceTable = NULL;
		void fetchReferenceRows();
	public:
		~GroupFilter() {
//			for(Filter<T>* f : filters) {
////				delete f;
//			}
		}

//		GroupFilter(Filter<T>* filter, RIVTable* sourceTable);
//		GroupFilter(const std::vector<Filter*>& filters, RIVTable* sourceTable);
//		bool AppliesToTable(const RIVTable* table);
		bool PassesFilter(RIVTableInterface* table, size_t row) {
			for(riv::Filter<T>* f : CompoundFilter<T>::filters) {
				bool thisFilterPassed = false;
				std::pair<size_t*,ushort> refRows = ref->GetReferenceRows(row);
	//			printArray(refRows.first, refRows.second);
				if(refRows.first) {
					for(size_t i = 0 ; i < refRows.second ; ++i) {
						if(f->PassesFilter(ref->targetTable, refRows.first[i])) {
							thisFilterPassed = true;
							break;
						}
					}
				}
				//The filter was not passed by any reference row
				if(!thisFilterPassed) return false;
			}
			return true;
		}
		
		bool PassesFilter(const std::string& name, T value) {
			for(Filter<T>* filter : CompoundFilter<T>::filters) {
				if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
					return true;
				}
			}
			return false;
		}
//		void Print() {
//			printf("Group filter containing : \n");
//			for(Filter* f : filters) {
//				printf("\t");
//				f->Print();
//			}
//		}
	};
}
#endif

