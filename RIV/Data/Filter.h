
#ifndef FILTER_H
#define FILTER_H

#include <string>

namespace riv {
	class Filter
	{
	protected:
		Filter(const std::string& attributeName) { this->attributeName = attributeName; };
	public:
		virtual bool PassesFilter(const std::string& name, float value) = 0;
		virtual bool PassesFilter(const std::string& name, unsigned short value) = 0;
		std::string attributeName;
		~Filter(void);
		virtual void Print() = 0;
	};

	class RangeFilter : public Filter {
	private:
		float minValue;
		float maxValue;
	public :
		RangeFilter(const std::string& attributeName, float minValue, float maxValue);
		~RangeFilter();
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
			printf("RangeFilter [attributeName = %s, (min,max) = (%f,%f)]\n",attributeName.c_str(),minValue,maxValue);
		}
	};
	
	class DiscreteFilter : public Filter {
	private:
		float value;
//		float epsilon = 0.00001F;
	public:
		DiscreteFilter(const std::string& attributeName, float value);
		
		bool PassesFilter(const std::string& name, float value);
		bool PassesFilter(const std::string& name, unsigned short value);
		void Print() {
			printf("DiscreteFilter [attributeName = %s, value = %f]\n",attributeName.c_str(),value);
		}
	};
}
#endif

