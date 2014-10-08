//
//  Selector.h
//  afstuderen_test
//
//  Created by Gerard Simons on 07/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__Selector__
#define __afstuderen_test__Selector__

#include <string>

namespace sqlite {
	//Interface for all selectors
	class Selector {
	public:
		virtual std::string generateSQL() = 0;
	};

	//A basic WHERE <ATTRIBUTE_NAME = VALUE> selector
	template <typename T>
	class ValueSelector : public Selector{
	private:
		std::string attributeName;
		T value;
	};

	template <typename T>
	class RangeSelector {
	private:
		T min;
		T max;
	};

	class CustomSelector {
	private:
		const std::string query;
	public:
		CustomSelector(const std::string& customQuery) : query(customQuery) {
			
		}
	};
}

#endif /* defined(__afstuderen_test__Selector__) */
