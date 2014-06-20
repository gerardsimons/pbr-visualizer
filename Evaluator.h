//
//  Interpolator.h
//  Afstuderen
//
//  Created by Gerard Simons on 12/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__Interpolator__
#define __Afstuderen__Interpolator__

#include <vector>
#include <algorithm>
#include <map>

template<typename T,typename U>
class Evaluator {
public:
    virtual U Evaluate(const T& value) = 0;
};

template<typename T,typename U>
class FixedEvaluator : public Evaluator<T,U> {
    U fixedValue;
public:
    FixedEvaluator(U value) {
        fixedValue = value;
    }
    U Evaluate(const T& value) {
        return fixedValue;
    }
};

template<typename T,typename U>
class DiscreteEvaluator : public Evaluator<T, U> {
private:
    std::map<T,U> discreteValuesMap;
public:
    DiscreteEvaluator(std::map<U,T>& discreteValuesMap_) {
        discreteValuesMap = discreteValuesMap_;
    }
    DiscreteEvaluator(const std::vector<T>& inputKeys, const std::vector<U>& outputValues) {
        if(inputKeys.size() != outputValues.size()) throw "keys size should match value size";
        for(size_t i = 0 ; i < inputKeys.size() ; ++i) {
            discreteValuesMap[inputKeys[i]] = outputValues[i];
        }
    }
    U Evaluate(const T& value) {
        if(discreteValuesMap.count(value) == 1) {
            return discreteValuesMap[value];
        }
        else throw "No such value";
    }
};

//Abstract base class
template<typename T>
class Interpolator : public Evaluator<T,float>{
private:
    void checkEquality() {
        T firstValue = values[0];
        //If all are equal to the first, they are also equal to each other
        for(size_t i = 1 ; i < values.size() ; ++i) {
            if(values[i] != firstValue) {
                allEqual = false;
                return;
            }
        }
        allEqual = true;
    }
protected:
    bool allEqual = false;
    std::vector<T> values;
    Interpolator(const T& min_,const T& max_) {
        values.push_back(min_);
        values.push_back(max_);
        checkEquality();
    }
    Interpolator(const std::vector<T>& values_) {
        if(values_.size() < 2) {
            throw "At least 2 values are required to interpolate.";
        }
        values = values_;
        std::sort(values.begin(), values.end());
        checkEquality();
//        if(values[0] != 0) {
//            std::vector<size_t>::iterator it;
//            it = values.begin();
//            it = values.insert ( it , 0 );
//        }
    }
public:
    virtual ~Interpolator() {
        
    }
};

template<typename T>
class LinearInterpolator : public Interpolator<T>{
protected:
public:
    LinearInterpolator(const T& min, const T& max) : Interpolator<T>(min,max) {
        // ... //
    }
    LinearInterpolator(const std::vector<T>& values) : Interpolator<T>(values) {
        // ... //
    }
    ~LinearInterpolator() {
        
    }
    float Evaluate(const T& value) {
        if(this->allEqual) {
            return 1.F;
        }
        if(value < this->values[0]) {
            return 0.F;
        }
        float weightDelta = 1.F / (this->values.size() - 1);
        float weight = 0.F;
        for(size_t i = 0 ; i < this->values.size() - 1 ; i++) {
            if(value >= this->values[i] && value <= this->values[i + 1]) {
                float interpolated = (value - this->values[i]) / (float)(this->values[i + 1] - this->values[i]) * weightDelta + weight;
                return interpolated;
            }
            weight += weightDelta;
        }
        return 1.F;
    }
};


template<typename T>
class DiscreteInterpolator : public Interpolator<T> {
public:
    DiscreteInterpolator(const std::vector<T>& values) : Interpolator<T>(values) {
        // ... //
    }
    float Evaluate(const T& value) {
        for(size_t i = 0 ; i < this->values.size() ; i++) {
            if(value == this->values[i]) {
                return i / (float)(this->values.size() - 1);
            }
        }
        throw "No such value.";
    }
};


#endif /* defined(__Afstuderen__Interpolator__) */
