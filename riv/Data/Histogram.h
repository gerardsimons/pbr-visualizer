//
//  Histogram.h
//  embree
//
//  Created by Gerard Simons on 01/12/14.
//
//

#ifndef __embree__Histogram__
#define __embree__Histogram__

#include <map>
#include <vector>
#include <exception>
#include <iostream>
#include <cmath>

#include "SampleSet.h"

//histogram implementation. Counts the values in the given vector and divides by the number of samples to normalize
template<typename T>
class Histogram {
private:
	std::map<T,size_t> hist;
	size_t nrElements;
	
	T lowerBound;
	T upperBound;
	
	unsigned int bins = 0;
	float binWidth = 0;
	
	void createFromData(const std::vector<T>& ts, unsigned int bins) {
		
		this->bins = bins;
		
		if(ts.size() == 0) {
			throw std::runtime_error("Empty data vector");
		}
		
		lowerBound = ts[0];
		upperBound = ts[0];
		for(const T& t : ts) {
			lowerBound = std::min(lowerBound,t);
			upperBound = std::max(upperBound,t);
		}
		
		for(const T& t : ts) {
			Add(t);
		}
		
		nrElements = ts.size();
	}
	void createFromData(const std::vector<T>& ts) {
		
		if(ts.size() == 0) {
			throw std::runtime_error("Empty data vector");
		}
		
		for(const T& t : ts) {
			++hist[t];
		}

		lowerBound = ts[0];
		upperBound = ts[0];
		
		for(auto it : hist) {
			lowerBound = std::min(lowerBound,it.first);
			upperBound = std::max(upperBound,it.first);
		}
		
		nrElements = ts.size();
	}
	float determineBinWidth() {
		return (float)(upperBound - lowerBound) / bins;
	}
	size_t value(T key) {
		return hist[key];
	}
public:
	Histogram() {
		nrElements = 0;
	}
	Histogram(ushort lowerBound, ushort upperBound) {
		nrElements = 0;
		bins = upperBound - lowerBound;
		binWidth = 1;
	}
	Histogram(T lowerBound, T upperBound, unsigned int bins) : lowerBound(lowerBound), upperBound(upperBound), bins(bins), binWidth(determineBinWidth()) {
		nrElements = 0;
	}
	Histogram(const std::vector<T>& ts, unsigned int bins) {
		createFromData(ts,bins);
	}
	Histogram(const SampleSet<T>& sampleset, unsigned int bins){
		createFromData(sampleset.GetSamples(),bins);
	}
	Histogram(const std::vector<T>& ts) {
		createFromData(ts);
	}
	Histogram(const SampleSet<T>& sampleset) {
		createFromData(sampleset.GetSamples());
	}
	T LowerBound() {
		return lowerBound;
	}
	T UpperBound() {
		return upperBound;
	}
	void Add(const T& value) {
		
		float interpolated = (float)(value - lowerBound) / (upperBound - lowerBound);
		unsigned int bin = round(interpolated);
		
		++hist[bin];
		++nrElements;
	}
	float operator-(Histogram& right) {
		
		T lowerBound = std::min(LowerBound(),right.LowerBound());
		T upperBound = std::max(UpperBound(),right.UpperBound());
		float totalDiff = 0;
		
		//Use normalized values!
		for(T i = lowerBound ; i <= upperBound ; i++) {
			float thisValue = NormalizedValue(i);
			float rightValue = right.NormalizedValue(i);
			float diff = std::abs((double)(thisValue - rightValue));
			totalDiff += diff;
		}
		return totalDiff;
		
	}
	float NormalizedValue(int i) {
		return value(i) / (float) nrElements;
	}
	void Print() {
		int maxBarSize = 48;
		for(int i = 0 ; i < bins ; ++i) {
			printf("%.1f - %.1f : ",(float)i * binWidth,float(i+1)*binWidth);
			float normalValue = NormalizedValue(i);
			int barSize = normalValue * maxBarSize;
			int tabs = maxBarSize / 4 - barSize / 4  + 1; //4 is the tabwidth in spaces
			for(int i = 0 ; i < barSize ; i++) {
				std::cout << "#";
			}
			for(int i = 0 ; i < tabs ; ++i) {
				std::cout << "\t";
			}
			std::cout << value(i) << std::endl;
		}
	}
};

template <typename... Ts>
class HistogramSet {
private:
	std::tuple<std::vector<Histogram<Ts>>...> histograms;
//	std::tuple<std::vector<Ts>...> values;
//	std::tuple<std::vector<Histogram<float>>,std::vector<Histogram<ushort>>> histograms;
//	std::tuple<int> ints;
	
public:

	
//	template<typename T>
//	std::vector<T> GetValue() {
//		return std::get<std::vector<T>>(values);
//	}
//	
	template<typename T>
	std::vector<Histogram<T>>* GetHistograms() {
		return &std::get<std::vector<Histogram<T>>>(histograms);
	}
	
	template<typename T>
	void AddHistogram(const Histogram<T>& hist) {
		std::get<std::vector<Histogram<T>>>(histograms).push_back(hist);
	}
	
//	template<typename U,typename... Us>
//	float compare(HistogramSet<Us...>& other) {
//		float result = 0;
//		std::vector<Histogram<U>>* hists = GetHistograms<U>();
//		std::vector<Histogram<U>>* otherhHists = other.GetHistograms<U>();
//		for(size_t i = 0 ; i < hists->size() ; ++i) {
//			result += hists->get(i) - otherhHists->get(i);
//		}
//		return result + compare<Us...>(other);
//	}

	template <typename...Us>
	struct comparison {
		
	};
	
	template<typename... Us>
	float compare() {
		return 1;
	}
	
	
	
	template<typename U,typename... Us>
	float compare() {
		return compare<Us...>();
	}
	
//	float compare() {
//		return 0;
//	}
	
	float operator-(HistogramSet<Ts...>& other) {
		return compare<Ts...>();
//		return 0;
	}
//
//	template<typename T>
//	std::vector<T>* GetHistograms() {
//		return &std::get<T>(histograms);
//	}
};

#endif /* defined(__embree__Histogram__) */
