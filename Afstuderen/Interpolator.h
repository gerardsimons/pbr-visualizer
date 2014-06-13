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

//Abstract base class
template<typename T>
class Interpolator {
private:
protected:
    std::vector<T> values;
    Interpolator(const T& min_,const T& max_) {
        values.push_back(min_);
        values.push_back(max_);
    }
    Interpolator(const std::vector<T>& values_) {
        if(values_.size() < 2) {
            throw "At least 2 values are required to interpolate.";
        }
        values = values_;
        std::sort(values.begin(), values.end());
//        if(values[0] != 0) {
//            std::vector<size_t>::iterator it;
//            it = values.begin();
//            it = values.insert ( it , 0 );
//        }
    }
public:
    virtual float Interpolate(const T& value) const = 0;
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
    float Interpolate(const T& value) const {
        if(value < this->values[0]) {
            return 0.F;
        }
        for(size_t i = 0 ; i < this->values.size() - 1 ; i++) {
            if(value >= this->values[i] && value <= this->values[i + 1]) {
                float interpolated = (value + this->values[i]) / (float)(this->values[i + 1] + this->values[i]);
                return interpolated;
            }
            
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
    float Interpolate(const T& value) const {
        for(size_t i = 0 ; i < this->values.size() ; i++) {
            if(value == this->values[i]) {
                return i / (float)(this->values.size() - 1);
            }
        }
        throw "No such value.";
    }
};


#endif /* defined(__Afstuderen__Interpolator__) */
