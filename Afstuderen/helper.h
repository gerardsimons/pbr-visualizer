//
//  helper.h
//  Afstuderen
//
//  Created by Gerard Simons on 16/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__helper__
#define __Afstuderen__helper__

#include <stdio.h>
#include <map>

template<typename T,typename U>
void printMap(std::map<T,U> map) {
    for(typename std::map<T, U>::const_iterator it = map.begin();
        it != map.end(); ++it)
    {
        std::cout << it->first << " : " << it->second << "\n";
    }
}

#endif /* defined(__Afstuderen__helper__) */
