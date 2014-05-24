//
//  helper.h
//  Afstuderen
//
//  Created by Gerard Simons on 16/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__helper__
#define __Afstuderen__helper__

#include <iostream>
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

template<typename T>
void printMatrix(size_t rows, size_t columns, T* matrix) {
    for(size_t i = 0 ; i < rows ; i++) {
        std::cout << "[";
        for(size_t j = 0 ; j < columns ; j++) {
            std::cout << matrix[i+j];
            if(j != columns - 1) {
                std::cout << " ";
            }
        }
        std::cout << "]\n";
    }
}

float* linearInterpolateColor(float,const float[3], const float[3]);

#endif /* defined(__Afstuderen__helper__) */
