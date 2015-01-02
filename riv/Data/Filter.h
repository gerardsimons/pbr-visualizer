
#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>
#include <tuple>

#include "TupleIterator.h"
#include "Reference.h"
#include "TableInterface.h"

typedef unsigned short ushort;

//#include "Table.h"

//class RIVReference;
//class RIVTableInterface;

namespace riv {
	class Filter
	{
	private:
		static size_t fidCounter;
	protected:
		size_t fid; //The id of the filter, automatically created to be unique for every filter
	public:
		size_t GetId();
		virtual bool AppliesToAttribute(const std::string& name) = 0;
		virtual bool AppliesToTable(const RIVTableInterface* table) = 0;
		virtual void Print() {
			printf("Generic filter object\n");
		}
		
	};
	
	template <typename T>
	class SingularFilter : public Filter {
	protected:
		std::string attributeName;
		SingularFilter(const std::string& attributeName) : attributeName(attributeName) {
			
		}
	public:
		std::string GetAttribute() {
			return attributeName;
		}
		virtual bool PassesFilter(const T& value) = 0;
		virtual bool PassesFilter(const std::string& name, const T& value) = 0;
		bool AppliesToAttribute(const std::string& name) {
			return attributeName == name;
		}
		bool AppliesToTable(const RIVTableInterface* table) {
			return table->HasRecord(attributeName);
		}
//		bool AppliesToTable(const RIVTable* table);
		std::vector<std::string> GetAttributes();
	};
	
	template <typename T>
	class DiscreteFilter : public SingularFilter<T> {
	private:
		T value;
		//		float epsilon = 0.00001F;
	public:
		DiscreteFilter(const std::string& attributeName,const T& value) : SingularFilter<T>(attributeName), value(value) {
			
		}
		T GetValue() {
			return value;
		}
		bool PassesFilter(const std::string& name,const T& value) {
			return (SingularFilter<T>::AppliesToAttribute(name) && PassesFilter(value));
		}
		bool PassesFilter(const T& value) {
			bool pass = this->value == value;
//			if(pass) {
//				printf("DiscreteFilter pass : ");
//				std::cout << this->value << " = " << value << std::endl;
//			}
			return pass;
		}
		void Print() {
			printf("DiscreteFilter [attributeName = %s, value = ",SingularFilter<T>::attributeName.c_str());
			std::cout << value << std::endl;
		}
	};
	

	template <typename T>
	class RangeFilter : public SingularFilter<T> {
	private:
		T minValue;
		T maxValue;
	public :
		RangeFilter(const std::string& attributeName, T minValue, T maxValue) : SingularFilter<T>(attributeName) {
			if(minValue < maxValue) {
				this->minValue = minValue;
				this->maxValue = maxValue;
			}
			else { //Swap them
				this->minValue = maxValue;
				this->maxValue = minValue;
			}
		}
		bool PassesFilter(const T& value) {
			return (value >= minValue && value <= maxValue);
		}
		bool PassesFilter(const std::string& name, const T& value) {
			return SingularFilter<T>::AppliesToAttribute(name) && PassesFilter(value);
		}
		void Print() {
			std::cout << "RangeFilter on " << SingularFilter<T>::attributeName << " (" << minValue << " : " << maxValue << ")" << std::endl;
		}
	};
	

	template<typename... Ts>
	class CompoundFilter : public Filter {
	protected:

		std::tuple<std::vector<SingularFilter<Ts>*>...> singlefilters;
		std::vector<CompoundFilter<Ts...>*> compoundFilters;
		
		CompoundFilter(const std::vector<CompoundFilter<Ts...>*>& compoundFilters) : compoundFilters(compoundFilters) {
			
		}
//		template<typename T>
//		CompoundFilter(const std::vector<SingularFilter<T>>* singleFilter) {
//			GetSingularFilters<T>()->push_back(singleFilter);
//		}
		template<typename T>
		CompoundFilter(const std::vector<SingularFilter<T>*>& singleFilters) {
			processSingle(singleFilters);
		}
		CompoundFilter(const std::vector<SingularFilter<Ts>*>&... singleFilters) {
			processSingle(singleFilters...);
		}
		CompoundFilter(CompoundFilter<Ts...>* compoundFilter) {
			compoundFilters.push_back(compoundFilter);
		}
//		CompoundFilter(CompoundFilter<Ts...>* compoundFilter) {
//			compoundFilters.push_back(compoundFilter);
//		}
		template<typename T>
		CompoundFilter(const SingularFilter<T>* singularFilter) {
			
		}
		template <typename T>
		void processSingle(const std::vector<SingularFilter<T>*>& first) {
			for(auto newFilter : first) {
				GetSingularFilters<T>()->push_back(newFilter);
			}
		}
		template<typename U,typename... Us>
		void processSingle(const std::vector<SingularFilter<U>*>& first, const std::vector<SingularFilter<Us>>... others) {
			for(auto newFilter : first) {
				GetSingularFilters<U>()->push_back(newFilter);
			}
			processSingle<Us...>(others...);
		}
//		CompoundFilter(Filter* f) {
//			filters.push_back(f);
//		}
//		CompoundFilter( const CompoundFilter& other ) {
//			filters = other.filters;
//		}
		CompoundFilter& operator=( const CompoundFilter& assigned ) {
			//			GroupFilter newFilter(assigned.filters,assigned.sourceTable);
			//			filters = assigned.filters;
//			filters = assigned.filters;
			return *this;
		}
	public:
		~CompoundFilter() {
//			for(Filter* f : filters) {
//				delete f;
//			}
		}
		
		std::tuple<std::vector<SingularFilter<Ts>*>...>& GetAllSingularFilters() {
			return singlefilters;
		}
		template<typename T>
		std::vector<SingularFilter<T>*>* GetSingularFilters() {
			return &std::get<std::vector<SingularFilter<T>*>>(singlefilters);
		}
		std::vector<CompoundFilter<Ts...>*>* GetCompoundFilters() {
			return &compoundFilters;
		}
		bool AppliesToAttribute(const std::string& name) {
//			for(Filter* filter : filters) {
//				if(filter->AppliesToAttribute(name)) {
//					return true;
//				}
//			}
			return false;
		}
		bool AppliesToTable(const RIVTableInterface* table) {
			bool applies = false;
			bool continueTuple = true;
			tuple_for_each(singlefilters, [&](auto tSingleFilters) {
				if(continueTuple) {
					//Each filter should apply to the table
					for(auto& singleFilter : tSingleFilters) {
						applies = true;
						if(!singleFilter->AppliesToTable(table)) {
							applies = false;
						}
					}
					if(!applies) {
						continueTuple = false;
					}
				}
			});
			if(!applies){
				for(auto& compoundFilter : compoundFilters) {
					applies = true;
					if(!compoundFilter->AppliesToTable(table)) {
						return false;
					}
				}
			}
			return applies;
		}
		size_t Size() {
//			return filters.size();
			return 1;
		}
		template<typename T>
		void AddFilter(SingularFilter<T>* newFilter) {
			if(!newFilter) {
				GetSingularFilters<T>()->push_back(newFilter);
			}
		}
		void AddCompoundFilter(CompoundFilter<Ts...>* newFilter) {
			compoundFilters.push_back(newFilter);
		}
		void Print() {
//			printf("Compound filter containing : \n");
//			for(Filter* f : filters) {
//				printf("\t");
//				f->Print();
//			}
		}
		
		std::vector<std::string> GetAttributes();
	};
	
	template<typename... Ts>
	class ConjunctiveFilter : public CompoundFilter<Ts...> {
	public:
		template<typename T>
		ConjunctiveFilter(const std::vector<SingularFilter<T>*>& filters) : CompoundFilter<Ts...>(filters) {
			
		}
		template<typename T>
		ConjunctiveFilter(const SingularFilter<T>* filter) : CompoundFilter<Ts...>(filter) {
			
		}
		ConjunctiveFilter() {
			
		}
		bool PassesFilter(const std::string& name..., Ts... value) {
//			for(auto* filter : CompoundFilter<Ts...>::filters) {
//				if(filter->AppliesToAttribute(name) && !filter->PassesFilter(name,value)) {
//					return false;
//				}
//			}
			return true;
		}
	};

//	template<typename T>
//	class DisjunctiveFilter : public CompoundFilter {
//	public:
//		DisjunctiveFilter(const std::vector<Filter*>& filters);
//		bool PassesFilter(const std::string &name, const T& value);
////		bool AppliesToTable(const RIVTable* table);
//		std::vector<std::string> AllAttributes();
////		bool PassesFilter(RIVTable<T>* table, size_t row);
//	};
	
	//This composite filter takes a table, searches its reffering rows to which its filters apply and tests the group of row to see if ALL the filters are met by atleast one row in the group
	//Of referring rows
	template<typename... Ts>
	class GroupFilter : public CompoundFilter<Ts...> {
	private:
		RIVReference* ref = NULL;
//		RIVTable<Ts...>* sourceTable = NULL;
		void fetchReferenceRows();
	public:
		~GroupFilter() {
//			for(Filter* f : filters) {
////				delete f;
//			}
		}
		template<typename T>
		GroupFilter(const std::vector<riv::SingularFilter<T>*>& filters) : CompoundFilter<Ts...>(filters) {
			
		}
		GroupFilter(riv::CompoundFilter<Ts...>* filter) : CompoundFilter<Ts...>(filter) {
			
		}
//		bool AppliesToTable(const RIVTable* table);
//		bool PassesFilter(RIVTable<Ts...>* table, size_t row);
		
//		bool PassesFilter(const std::string& name, T value) {
//			for(Filter* filter : CompoundFilter::filters) {
//				if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
//					return true;
//				}
//			}
//			return false;
//		}
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

