
#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>
#include "Reference.h"
#include "Table.h"

class RIVTable;
class RIVReference;

namespace riv {
	class Filter
	{
	private:
		size_t fidCounter = 1;
	protected:
		size_t fid; //The id of the filter, automatically created to be unique for every filter
	public:
		size_t GetId();
		Filter();
		//Only used internally
		virtual bool PassesFilter(const std::string& name, float value) = 0;
		virtual bool PassesFilter(const std::string& name, unsigned short value) = 0;
		virtual bool PassesFilter(RIVTable* table, size_t row) = 0;
		//Determines whether this filter is targeted at the given table
		virtual bool AppliesToAttribute(const std::string& name) = 0;
		virtual bool AppliesToTable(const RIVTable* table) = 0;
		virtual std::vector<std::string> GetAttributes() = 0;
		virtual void Print() = 0;
	};
	
	class SingularFilter : public Filter {
	protected:
		std::string attributeName;
		SingularFilter(const std::string& attributeName);
	public:
		std::string GetAttribute();
		bool AppliesToAttribute(const std::string& name);
		bool AppliesToTable(const RIVTable* table);
		std::vector<std::string> GetAttributes();
	};

	class RangeFilter : public SingularFilter {
	private:
		float minValue;
		float maxValue;
	public :
		RangeFilter(const std::string& attributeName, float minValue, float maxValue);
		bool PassesFilter(RIVTable* table, size_t row);
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
			printf("RangeFilter [attributeName = %s, (min,max) = (%f,%f)]\n",attributeName.c_str(),minValue,maxValue);
		}
	};
	
	class DiscreteFilter : public SingularFilter {
	private:
		float value;
//		float epsilon = 0.00001F;
	public:
		DiscreteFilter(const std::string& attributeName, float value);
		float GetValue();
		bool PassesFilter(RIVTable* table, size_t row);
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
			printf("DiscreteFilter [attributeName = %s, value = %f]\n",attributeName.c_str(),value);
		}
	};
	
	class CompoundFilter : public Filter {
	protected:
		std::vector<Filter*> filters;
		CompoundFilter(const std::vector<Filter*> filters) {
			this->filters = filters;
		}
		CompoundFilter(Filter* f) {
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
	public:
		~CompoundFilter();
		bool AppliesToAttribute(const std::string& attributeName);
		bool AppliesToTable(const RIVTable* table);
		std::vector<Filter*> GetFilters();
		size_t Size();
		void AddFilter(Filter* newFilter);
		std::vector<std::string> GetAttributes();
	};
	
	class ConjunctiveFilter : public CompoundFilter {
	public:
		ConjunctiveFilter(const std::vector<Filter*>& filters);
		bool AppliesToTable(const RIVTable* table);
		bool PassesFilter(RIVTable* table, size_t row);
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
			printf("Conjunctive filter containing : \n");
			for(Filter* f : filters) {
				printf("\t");
				f->Print();
			}
		}
	};
	
	class DisjunctiveFilter : public CompoundFilter {
	public:
		DisjunctiveFilter(const std::vector<Filter*>& filters);
		bool PassesFilter(const std::string &name, unsigned short value);
		bool PassesFilter(const std::string &name, float value);
		bool AppliesToTable(const RIVTable* table);
		std::vector<std::string> AllAttributes();
		bool PassesFilter(RIVTable* table, size_t row);
		void Print() {
			printf("Disjunctive filter containing : \n");
			for(Filter* f : filters) {
				printf("\t");
				f->Print();
			}
		}
	};
	
	//This composite filter takes a table, searches its reffering rows to which its filters apply and tests the group of row to see if ALL the filters are met by atleast one row in the group
	//Of referring rows
	class GroupFilter : public CompoundFilter {
	private:
		RIVReference* ref = NULL;
		RIVTable* sourceTable = NULL;
		void fetchReferenceRows();
	public:
		~GroupFilter() {
			for(Filter* f : filters) {
//				delete f;
			}
		}

		GroupFilter(Filter* filter, RIVTable* sourceTable);
		GroupFilter(const std::vector<Filter*>& filters, RIVTable* sourceTable);
		bool AppliesToTable(const RIVTable* table);
		bool PassesFilter(RIVTable* table, size_t row);
		bool PassesFilter(const std::string &name, unsigned short value);
		bool PassesFilter(const std::string &name, float value);
		void Print() {
			printf("Group filter containing : \n");
			for(Filter* f : filters) {
				printf("\t");
				f->Print();
			}
		}
	};
}
#endif

