
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
	
	class RowFilter : public Filter {
	private:
		std::map<size_t,bool> filteredRows;
		std::string tableName;
	public:
		RowFilter(const std::string& tableName, const std::map<size_t,bool>& filteredRows) : filteredRows(filteredRows), tableName(tableName)  {
			
		}
		bool PassesFilter(size_t row) {
			return !filteredRows[row];
		}
		virtual bool AppliesToAttribute(const std::string& name) {
			return true;
		}
		virtual bool AppliesToTable(const RIVTableInterface* table) {
			return table->name == tableName;
		}
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
		
		std::vector<std::string> attributes;
		
		void createAttributes() {
			std::map<std::string,bool> attributesPresent;
			tuple_for_each(singlefilters, [&](auto tFilters) {
				for(auto filter : tFilters) {
					attributesPresent[filter->GetAttribute()] = true;
				}
			});
			for(auto filter : compoundFilters) {
				const std::vector<std::string>& otherAttributes = filter->GetAttributes();
				for(const std::string& attribute : otherAttributes) {
					attributesPresent[attribute] = true;
				}
			}
			for(auto it : attributesPresent) {
				attributes.push_back(it.first);
			}
		}
		
		CompoundFilter(const std::vector<CompoundFilter<Ts...>*>& compoundFilters) : compoundFilters(compoundFilters) {
			createAttributes();
		}
		template<typename T>
		CompoundFilter(const std::vector<SingularFilter<T>*>& singleFilters) {
			processSingle(singleFilters);
			createAttributes();
		}
		CompoundFilter(const std::vector<SingularFilter<Ts>*>&... singleFilters) {
			processSingle(singleFilters...);
			createAttributes();
		}
		CompoundFilter(CompoundFilter<Ts...>* compoundFilter) {
			compoundFilters.push_back(compoundFilter);
		}
//		template<typename T>
//		CompoundFilter(const SingularFilter<T>* singularFilter) {
//
//		}
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
		std::vector<std::string> GetAttributes() {
			return attributes;
		}
//		CompoundFilter& operator=( const CompoundFilter& assigned ) {
			//			GroupFilter newFilter(assigned.filters,assigned.sourceTable);
			//			filters = assigned.filters;
//			filters = assigned.filters;
//			return *this;
//		}
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
			for(const std::string& attribute : attributes) {
				if(attribute == name) {
					return true;
				}
			}
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
			//Sum of the singular filters and the compound filters vector
			size_t size = 0;
			tuple_for_each(singlefilters, [&](auto singularTFilters) {
				size += singularTFilters.size();
			});
			size += compoundFilters.size();
			return size;
		}
		template<typename T>
		void AddFilter(SingularFilter<T>* newFilter) {
			if(!newFilter) {
				GetSingularFilters<T>()->push_back(newFilter);
			}
			createAttributes();
		}
		void AddFilter(CompoundFilter<Ts...>* newFilter) {
			compoundFilters.push_back(newFilter);
			createAttributes();
		}
		void Print() {
//			printf("Compound filter containing : \n");
//			for(Filter* f : filters) {
//				printf("\t");
//				f->Print();
//			}
		}
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

	template<typename... Ts>
	class DisjunctiveFilter : public CompoundFilter<Ts...> {
	public:
		template <typename T>
		DisjunctiveFilter(const std::vector<SingularFilter<T>*>& filters) : CompoundFilter<Ts...>(filters) {
			
		}
		template <typename T>
		bool PassesFilter(const std::string &name, const T& value) {
			
		}
		template <typename T>
		bool PassesFilter(const T& value) {
			//Does it pass all the single filters
			bool filterPassed = true;
			tuple_for_each(CompoundFilter<Ts...>::singlefilters, [&](auto tSingleFilters) {
				if(filterPassed) {
					//Each filter should apply to the table
					for(auto& singleFilter : tSingleFilters) {
						if(!singleFilter->PassesFilter(value)) {
							filterPassed = false;
						}
					}
				}
			});
			if(!filterPassed) {
				return false;
			}
			else {
				for(auto& compoundFilter : CompoundFilter<Ts...>::compoundFilters) {
					if(!compoundFilter->PassesFilter(value)) {
						return false;
					}
				}
			}
			return true;
		}
//		bool AppliesToTable(const RIVTable* table);
		std::vector<std::string> AllAttributes();
		void Print() {
			std::cout << "DisjunctiveFilter containing : \n";
			std::cout << "SingularFilters:" << std::endl;
			tuple_for_each(CompoundFilter<Ts...>::singlefilters, [&](auto tSingleFilters) {
				//Each filter should apply to the table
				for(auto& singleFilter : tSingleFilters) {
					singleFilter->Print();
				}
			});
		}
//		bool PassesFilter(RIVTable<T>* table, size_t row);
	};
	
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

		void Print() {
//			printf("Group filter containing : \n");
			tuple_for_each(CompoundFilter<Ts...>::singlefilters, [&](auto tFilters) {
				for(auto* f : tFilters) {
					f->Print();
				}
			});

		}
	};
}
#endif

