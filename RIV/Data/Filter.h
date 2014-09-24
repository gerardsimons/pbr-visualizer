
#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>
#include "Table.h"

class RIVTable;

namespace riv {
	class Filter
	{
	protected:
		std::vector<std::string> attributes;
		Filter(const std::string& attributeName) { attributes.push_back(attributeName); };
		Filter(const std::vector<std::string>& attributes) { this->attributes = attributes; };
	public:
		virtual bool PassesFilter(const std::string& name, float value) = 0;
		virtual bool PassesFilter(const std::string& name, unsigned short value) = 0;
		virtual bool PassesFilter(RIVTable* table, size_t row) = 0;
		//Determines whether this filter is targeted at the given table
		virtual bool AppliesToAttribute(const std::string& name);
		virtual bool AppliesToTable(const RIVTable* table);
		std::vector<std::string> const* GetAttributes() const {
			return &attributes;
		}
		virtual void Print() = 0;
	};

	class RangeFilter : public Filter {
	private:
		float minValue;
		float maxValue;
	public :
		RangeFilter(const std::string& attributeName, float minValue, float maxValue);
		bool PassesFilter(RIVTable* table, size_t row);
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
			printf("RangeFilter [attributeName = %s, (min,max) = (%f,%f)]\n",attributes[0].c_str(),minValue,maxValue);
		}
	};
	
	class DiscreteFilter : public Filter {
	private:
		float value;
//		float epsilon = 0.00001F;
	public:
		DiscreteFilter(const std::string& attributeName, float value);
		bool PassesFilter(RIVTable* table, size_t row);
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
			printf("DiscreteFilter [attributeName = %s, value = %f]\n",attributes[0].c_str(),value);
		}
	};
	
	class ConjunctiveFilter : public Filter {
	private:
		std::vector<Filter*> filters;
	public:
		~ConjunctiveFilter();
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
}
#endif

