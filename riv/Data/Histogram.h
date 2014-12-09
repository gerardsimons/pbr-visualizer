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
#include <string>
#include <cmath>

#include "TupleIterator.h"
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
	
	//Count the values and divide by the total number to get a normalized histogram
	void count(const std::vector<T>& ts) {
//		if(ts.size() == 0) {
//			throw std::runtime_error("Empty data vector");
//		}
		
		for(const T& t : ts) {
			Add(t);
		}
		
		nrElements = ts.size();
	}
	//Create the histogram from the given values and also use it to determine the natural bounds of the data
	void createFromData(const std::vector<T>& ts) {
//		if(ts.size() == 0) {
//			throw std::runtime_error("Empty data vector");
//		}
		
		count(ts);
		
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
	std::string name;
	
	Histogram() {
		nrElements = 0;
	}
	Histogram(const std::string& name, ushort lowerBound, ushort upperBound) : name(name), lowerBound(lowerBound), upperBound(upperBound) {
		nrElements = 0;
		bins = upperBound - lowerBound;
		binWidth = 1;
	}
	Histogram(const std::string& name, T lowerBound, T upperBound, unsigned int bins) : name(name), lowerBound(lowerBound), upperBound(upperBound), bins(bins), binWidth(determineBinWidth()) {
		nrElements = 0;
	}
	Histogram(const std::string& name, const std::vector<T>& ts, T lowerBound, T upperBound, unsigned int bins) : name(name), lowerBound(lowerBound), upperBound(upperBound), bins(bins), binWidth(determineBinWidth()) {
		count(ts);
	}
	Histogram(const std::string& name, const std::vector<T>& ts, unsigned int bins)  : name(name) {
		createFromData(ts,bins);
	}
	Histogram(const std::string& name, const SampleSet<T>& sampleset, unsigned int bins) : name(name){
		createFromData(sampleset.GetSamples(),bins);
	}
	Histogram(const std::string& name, const std::vector<T>& ts) : name(name){
		createFromData(ts);
	}
	Histogram(const std::string& name, const SampleSet<T>& sampleset) : name(name){
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
		
		if(right.bins != bins) {
			throw std::runtime_error("The histograms should have the same number of bins");
		}
		
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
		if(nrElements)
			return value(i) / (float) nrElements;
		else
			return 0;
	}
	void Print() {
		int maxBarSize = 48;
		printf("Histogram %s :\n",name.c_str());
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
		std::cout << std::endl;
	}
};

template <typename... Ts>
class HistogramSet {
private:
	std::tuple<std::vector<Histogram<Ts>>...> histograms;
	std::tuple<std::map<std::string,Histogram<Ts>*>...> histogramRegisters; //Maps a name to a histogram pointer
	
	//When set to dynamic the set will create new histograms for values added to not yet exsistent histograms
//	bool dynamic = true;
	
//	std::tuple<std::vector<Ts>...> values;
//	std::tuple<std::vector<Histogram<float>>,std::vector<Histogram<ushort>>> histograms;
//	std::tuple<int> ints;
	
public:

	
//	template<typename T>
//	std::vector<T> GetValue() {
//		return std::get<std::vector<T>>(values);
//	}
//
//	HistogramSet() {
//		
//	}
//	HistogramSet(const HistogramSet& other) {
//		histograms = other.histograms;
//		histogramRegisters = other.histogramRegisters;
//	}
//	HistogramSet& operator=(const HistogramSet& other) {
//		histograms = other.histograms;
//		histogramRegisters = other.histogramRegisters;
//		return *this;
//	}
	
	template<typename T>
	std::vector<Histogram<T>>* GetHistograms() {
		return &std::get<std::vector<Histogram<T>>>(histograms);
	}
	
	template<typename T>
	std::map<std::string,Histogram<T>*>& GetHistogramRegister() {
		return std::get<std::map<std::string,Histogram<T>*>>(histogramRegisters);
	}
	
	template<typename T>
	Histogram<T>* GetHistogram(const std::string& name) {
		std::map<std::string,Histogram<T>*>& histogramRegister = GetHistogramRegister<T>();
		return histogramRegister[name];
	}
	
	
	template<typename T>
	void AddToHistogram(const std::string name,const T& value) {
		Histogram<T>* histogram = GetHistogram<T>(name);
//		if(!histogram)
			//TODO: Create histogram when set to dynamic and histogram was not found
		histogram->Add(value);
	}
	
	template<typename T>
	void AddHistogram(const Histogram<T>& hist) {
		std::vector<Histogram<T>>* histograms = GetHistograms<T>();
		
		histograms->push_back(hist);
		std::get<std::map<std::string,Histogram<T>*>>(histogramRegisters)[hist.name] = &histograms->at(histograms->size() - 1);
	}
	
	//This assumes the right hand operand histogram set is a superset of this histogram set, throws a runtime_error if a histogram in this set could not be found.
	float operator-(HistogramSet<Ts...>& other) {
//		return compare<Ts...>();
		
		float total = 0;
		
		TupleForEach(histograms, [&](auto tHistograms) {
			for(auto& histogram : tHistograms) {
				bool found = false;
				TupleForEach(other.histograms, [&](auto otherTHistograms) {
					for(auto& otherHistogram : otherTHistograms) {
						if(otherHistogram.name == histogram.name) {
							total += (otherHistogram - histogram);
							found = true;
							break;
						}
					}
				});
				if(!found) {
					throw std::runtime_error("Histogram " + histogram.name + " not found in right hand operand");
				}
			}
		});
		
		return total;
	}
	
	void Print() {
		printf("Histogram set : \n");
		TupleForEach(histograms, [&](auto tHistograms) {
			for(auto& histogram : tHistograms) {
				histogram.Print();
			}
		});
	}
		
	void Join(const HistogramSet& other) {
		TupleForEach(other.histograms, [&](auto tHistograms) {
			for(auto& histogram : tHistograms) {
				AddHistogram(histogram);
			}
		});
	}
//
//	template<typename T>
//	std::vector<T>* GetHistograms() {
//		return &std::get<T>(histograms);
//	}
};

#endif /* defined(__embree__Histogram__) */
