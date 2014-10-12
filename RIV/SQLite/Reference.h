//
//  Reference.h
//  afstuderen_test
//
//  Created by Gerard Simons on 08/10/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_test__Reference__
#define __afstuderen_test__Reference__

#include <string>



namespace sqlite {
	class DataView;
	class Reference {
	public:
		DataView* fromView;
		DataView* toView;
		std::string fromColumnName;
		std::string toColumnName;
		Reference(std::string fromColumnName,std::string toColumnName,DataView* fromView,DataView* toView) {
			this->fromView = fromView;
			this->toView = toView;
			this->fromColumnName = fromColumnName;
			this->toColumnName = toColumnName;
		}
		Reference* Reverse() {
			return new Reference(toColumnName,fromColumnName,toView,fromView);
		}
	};
}

#endif /* defined(__afstuderen_test__Reference__) */
