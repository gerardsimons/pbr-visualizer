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
#include <vector>
#include <algorithm>
#include <iterator>

#include <GLUT/GLUT.h>

void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );

template <typename T, size_t N>
class Vector {
    T values[N];
    
};

template <typename T>
class Sampler {
protected:
    T lowerBound;
    T upperBound;
    T range;
public:
    Sampler(T lowerBound_, T upperBound_) {
        if(upperBound_ <= lowerBound_) {
            throw "Invalid sampling boundaries\n";
        }
        lowerBound = lowerBound_;
        upperBound = upperBound_;
        range = upperBound - lowerBound;
    }
    
    virtual T RequestSample() {
        T range = upperBound - lowerBound;
        return (T)(rand() % range);
    }
};

//void printHeader(const std::string& text, int width = 50) {
//    
//    for(size_t i = 0 ; i < width ; ++width) {
//        std::cout << "*";
//    }
//    std::cout << "\n";
//    
//    int left = (width - text.size()) / 2;
//    for(size_t i = 0 ; i < left ; ++i) {
//        std::cout << " ";
//    }
//    std::cout << text;
//    int right = (width - text.size() - left);
//    for(size_t i = 0 ; i < left ; ++i) {
//        std::cout << " ";
//    }
//    std::cout << "\n";
//
//    for(size_t i = 0 ; i < width ; ++width) {
//        std::cout << "*";
//    }
//    std::cout << "\n";
//}

template<class T>
inline void deletePointerVector(std::vector<T*> v)
{
    for (typename std::vector<T*>::iterator i = v.begin(); i != v.end(); i++)
    {
        delete *i;
    }
}

void printDelimiterLine(char c = '*',int width = 50);

template<class T>
void printVar(const char* name, T var) {
    std::cout << name << " = " << var << std::endl;
}

template <typename T>
class NonReplacementSampler {
    std::vector<T> samplePool;
    std::vector<T>* origin;
    int requestCount = 0;
public:
    ~NonReplacementSampler() {
        samplePool.clear();
    }
    NonReplacementSampler() {
        //Do nothing
    }
    NonReplacementSampler(std::vector<T>* origin_) {
        origin = origin_;
        Reset();
    }
    NonReplacementSampler(T lowerbound, T upperbound) {
        if(upperbound < lowerbound) {
            throw "Invalid bounds.";
        }
        origin = new std::vector<T>;
        for(size_t i = lowerbound ; i < upperbound ; ++i) {
            origin->push_back(i);
        }
        Reset();
    }
    void Reset() {
        samplePool = (*origin);
    }
    T RequestSample() {
        ++requestCount;
        //        printf("Request #%d\n",requestCount);
        if(!samplePool.empty()) {
            size_t index = (T)(rand() % samplePool.size());
            T value = samplePool[index];
            samplePool.erase(samplePool.begin() + index);
            return value;
        }
        else {
            throw "Out of samples!";
        }
    }
};

unsigned long long
choose(unsigned long long n, unsigned long long k);

std::vector<std::vector<size_t>> generateCombinations(unsigned long long n, unsigned long long r, size_t requested);

std::vector<std::vector<size_t>> generateCombinations(std::vector<size_t> pool, unsigned long long r, size_t requested);

template <typename T>
size_t find(std::vector<T> haystack, T needle) {
    int i = 0 ;
    for(T value : haystack) {
        if(value == needle) {
            return i;
        }
        i++;
    }
    return -1;
}

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

template<typename T>
void printArray(T* array, size_t size) {
    std::cout << "[";
    for(size_t i = 0 ; i < size ; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << "]";
}

template <typename T>
bool arrayContains(T* array, size_t size, T value) {
    for(size_t i = 0 ; i < size ; i++) {
        if(array[i] == value) return true;
    }
    return false;
}

template <typename T>
void printVector(const std::vector<T>& values,std::string name = "") {
    
    std::cout << "vector " + name + " = [ ";
    for(size_t i = 0 ; i < values.size() ; ++i) {
        T value = values[i];
        std::cout << value << " ";
    }
    std::cout << "]\n";
}

void copy_buffer();
void copy_buffer_back_to_front();
unsigned long factorization(int N,unsigned long maxF);
template <typename T>
T* randInRange(T lower, T upper, size_t N) {
    //Sanity checks
    if(N > (upper - lower)) {
        throw "Numbers requested exceeds random range";
    }
    if(upper <= lower) {
        throw "Invalid range";
    }
    
    std::vector<int> pool;
    //Fill pool
    for(int i = lower ; i < upper ; ++i) {
        pool.push_back(i);
    }
    T* numbersDrawn = new T[N];
    //Draw N numbers from pool
    for(int j = 0 ; j < N ; ++j) {
        size_t index = (size_t)(rand() / (float)RAND_MAX * pool.size());
        numbersDrawn[j] = pool[index];
        pool.erase(pool.begin() + index);
    }
    pool.clear();
    return numbersDrawn;
}
int** combinationsInRange(int lower, int upper, unsigned int N);
int* drawNumbersFromPool(std::vector<int> pool, const int N);
float* linearInterpolateColor(float,const float[3], const float[3]);
float linearInterpolate(float,float,float);

#endif /* defined(__Afstuderen__helper__) */
