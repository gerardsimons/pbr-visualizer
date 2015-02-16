
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
    
    template<typename T>
    class CompoundFilter : public Filter{
    public:
        std::vector<SingularFilter<T>*> filters;
        CompoundFilter(const std::vector<SingularFilter<T>>& filters) : filters(filters) {
            
        }
        CompoundFilter() {
            
        }
        void AddFilter(riv::SingularFilter<T>* newFilter) {
            filters.push_back(newFilter);
        }
        virtual bool AppliesToAttribute(const std::string& name) {
            for(auto filter : filters) {
                if(filter->AppliesToAttribute(name)) {
                    return true;
                }
            }
            return false;
        }
        virtual bool AppliesToTable(const RIVTableInterface* table) {
            for(auto filter : filters) {
                if(filter->AppliesToTable(table)) {
                    return true;
                }
            }
            return false;
        }
    };
}
#endif

