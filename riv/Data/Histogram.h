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
#include <limits>

#include "TupleIterator.h"
#include "SampleSet.h"
#include "../Grid.h"

//histogram implementation. Counts the values in the given vector and divides by the number of samples to normalize
template<typename T,typename U = int>
class Histogram {
    
private:
    
    //Use ints because we also want to have negative counts when using difference of two histograms
    std::map<unsigned int,U> hist;
    
    bool cdfStale = true;
    std::map<unsigned int,float> cdf; //Cumulative normalized distribution
    size_t nrElements;
    
    T lowerBound;
    T upperBound;
    
    T* maxValue = NULL;
    T* minValue = NULL;
    
    unsigned int bins = 0;
    float binWidth = 0;
    
    //Count the values and divide by the total number to get a normalized histogram
    void count(const std::vector<T>& ts) {
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
        if(upperBound == lowerBound) {
            return 0.5F;
        }
        else return (float)(upperBound - lowerBound) / bins;
    }
public:
    std::string name;
    //    Histogram& operator= (const Histogram& other) {
    //        hist = other.hist;
    //
    //        upperBound = other.upperBound;
    //        lowerBound = other.lowerBound;
    //
    //    bins = other.bins;
    
    //        nrElements = other.nrElements;
    //        cdf = other.cdf;
    //        cdfStale = other.cdfStale;
    //
    //        if(other.maxValue) {
    //            maxValue = new T(*other.maxValue);
    //            delete other.maxValue;
    //        }
    //        if(other.minValue) {
    //            minValue = new T(*other.minValue);
    //            delete other.minValue;
    //        }
    //
    //        return *this;
    //    }
    ~Histogram() {
        
    }
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
    Histogram(const std::string& name, T lowerBound, T upperBound, unsigned int bins, size_t nrElements) : name(name), lowerBound(lowerBound), upperBound(upperBound), bins(bins), binWidth(determineBinWidth()), nrElements(nrElements) {
        
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
    U MaximumValue() {
        U max = 0;
        for(auto iter : hist) {
            if(iter.second > max) {
                max = iter.second;
            }
        }
        return max;
    }
    U MinimumValue() {
        U min = -1;
        for(auto iter : hist) {
            if(iter.second < min) {
                min = iter.second;
            }
        }
        return min;
    }
    T MaximumNormalizedValue() {
        if(nrElements) {
            return MaximumValue() / nrElements;
        }
        else return 0;
    }
    //    float Mean() {
    //        for(int i = 0 ; i < bins ; i++) {
    //            int thisValue = BinValue(i);
    //            int rightValue = right.BinValue(i);
    //            int diff = (thisValue - rightValue);
    //            result.Set(i,diff);
    //
    //        }
    //    }
    unsigned int SampleBin() {
        if(cdfStale) {
            ComputeCDF();
        }
        
        float R = random() / (float)RAND_MAX;
        
        for(int i = 0 ; i < bins - 1 ; ++i) {
            if(R <= cdf[i]) {
                return i;
            }
        }
        return bins - 1;
    }
    float Sample() {
        unsigned int bin = SampleBin();
        
        //Jitter the result within the bin
        float R = rand() / (float)RAND_MAX;
        float offset = R * binWidth;
        float sampledValue = (bin * binWidth + lowerBound) + offset;
        return sampledValue;
    }
    void ComputeCDF() {
        cdf.clear();
        float cummulative = 0;
        for(unsigned int i = 0 ; i < bins ; ++i) {
            cummulative += NormalizedValue(i);
            cdf[i] = cummulative;
        }
        cdfStale = false;
    }
    U BinValue(unsigned int bin) {
        return hist[bin];
    }
    float ScaledValue(unsigned int bin) {
        U max = MaximumValue();
        if(max) {
            return hist[bin] / (float)max;
        }
        else return 0;
    }
    float GetBinWidth() {
        return binWidth;
    }
    void Set(int bin, U count) {
        nrElements += count - hist[bin];
        hist[bin] = count;
    }
    unsigned int Add(const T& value) {
        
        unsigned int bin = BinForValue(value);
        
        ++hist[bin];
        ++nrElements;
        
        cdfStale = true;
        
        return bin;
    }
    unsigned int Add(const T& value, size_t number) {
        
        unsigned int bin = BinForValue(value);
        
        hist[bin] += number;
        nrElements += number;
        
        cdfStale = true;
        
        return bin;
    }
    void AddToBin(unsigned int bin, size_t number) {
        hist[bin] += number;
        nrElements += number;
        
        cdfStale = true;
    }
    unsigned int NumberOfBins() {
        return bins;
    }
    Histogram operator-(Histogram& right) {
        if(right.bins != bins) {
            throw std::runtime_error("The histograms should have the same number of bins");
        }
        
        //		printf("left - right = \n\n");
        //		printf("left = \n");
        //		Print();
        //		printf("right = \n");
        //		right.Print();
        //		printf("\n");
        
        T lowerBound = std::min(LowerBound(),right.LowerBound());
        T upperBound = std::max(UpperBound(),right.UpperBound());
        
        Histogram result(name,lowerBound,upperBound,bins,nrElements - right.NumberOfElements());
        
        //Use normalized values!
        for(int i = 0 ; i < bins ; i++) {
            U thisValue = BinValue(i);
            U rightValue = right.BinValue(i);
            int diff = std::abs(thisValue - rightValue);
            result.Set(i,diff);
            
        }
        //		printf("Result = \n");
        //		result.Print();
        return result;
    }
    Histogram operator+(Histogram& right) {
        if(right.bins != bins) {
            throw std::runtime_error("The histograms should have the same number of bins");
        }
        
        T lowerBound = std::min(LowerBound(),right.LowerBound());
        T upperBound = std::max(UpperBound(),right.UpperBound());
//        size_t sumElements = nrElements + right.NumberOfElements();
        Histogram result(name,lowerBound,upperBound,bins,0);
        
        //Use normalized values!
        for(unsigned int i = 0 ; i < bins ; i++) {
            U thisValue = BinValue(i);
            U rightValue = right.BinValue(i);
            U diff = (thisValue + rightValue);
            result.Set(i,diff);
            
        }
        
        return result;
    }
    Histogram BooleanHistogram() {
        
        Histogram result(name,lowerBound,upperBound,bins,0);
        
        //Use normalized values!
        for(unsigned int i = 0 ; i < bins ; i++) {
            U thisValue = BinValue(i);
            U boolValue = (int)(bool)thisValue;
            result.Set(i,boolValue);
        }
        
        return result;
    }
    int BinForValue(const T& value) {
        //		T valueClamped = value;
        if(value <= lowerBound) {
            return 0;
        }
        else if(value >= upperBound) {
            return bins - 1;
        }
        T delta = upperBound - lowerBound;
        double interpolated = (double)(value - lowerBound) / (delta);
        unsigned int bin = std::floor(interpolated * (bins));
        //		//When the value is exeactly on the upper edge it floors incorrectly to bin = bins
        //		if(bin == bins) {
        //			bin = bins - 1;
        //		}
        return bin;
    }
    float DistanceTo(Histogram* right) {
        if(right->bins != bins) {
            throw std::runtime_error("The histograms should have the same number of bins");
        }
        
        float totalDiff = 0;
        
        //		printf("DistanceTo:\n");
        //		Print();
        //		right->Print();
        //Use normalized values!
        for(int i = 0 ; i < bins ; i++) {
            float thisValue = NormalizedValue(i);
            float rightValue = right->NormalizedValue(i);
            float diff = std::abs((thisValue - rightValue));
            totalDiff += diff;
        }
        //		printf("totalDiff = %f\n",totalDiff);
        return totalDiff;
    }
    float NormalizedValue(int i) {
        if(nrElements)
            return BinValue(i) / (float) nrElements;
        else
            return 0;
    }
    void Clear() {
        //        printf("Clear histogram %s\n",name.c_str());
        hist.clear();
        nrElements = 0;
        cdfStale = true;
    }
    void Print() {
        int maxBarSize = 48;
        printf("Histogram %s has %zu elements in %zu bins :\n",name.c_str(),nrElements,bins);
        
        int binIndex = BinForValue(lowerBound);
        
        float binStart = lowerBound;
        
        for(unsigned int bin = 0 ; bin < bins ; ++bin) {
            //		for(auto it : hist) {
            
            printf("%.2f - %.2f\t : ",binStart,binStart + binWidth);
            float normalValue = NormalizedValue(bin);
            int barSize = normalValue * maxBarSize;
            int tabs = (maxBarSize - barSize) / 4.F; //4 is the tabwidth in spaces
            for(int i = 0 ; i < barSize ; i++) {
                std::cout << "#";
            }
            for(int i = 0 ; i < tabs ; ++i) {
                std::cout << "\t";
            }
            std::cout << hist[bin];
            std::cout << " (" << normalValue << ")" << std::endl;
            
            binStart += binWidth;
            
            ++binIndex;
        }
        //
        //		for(float i = lowerBound ; i < upperBound ; i += binWidth) {
        //			printf("%.2f - %.2f\t : ",i,i+binWidth);
        //			float normalValue = NormalizedValue(binIndex);
        //			int barSize = normalValue * maxBarSize;
        //			int tabs = maxBarSize / 4 - barSize / 4  + 1; //4 is the tabwidth in spaces
        //			for(int i = 0 ; i < barSize ; i++) {
        //				std::cout << "#";
        //			}
        //			for(int i = 0 ; i < tabs ; ++i) {
        //				std::cout << "\t";
        //			}
        //			int value = BinValue(binIndex);
        //			std::cout << value;
        //			std::cout << " (" << value / (float)nrElements << ")" << std::endl;
        //			++binIndex;
        //		}
        std::cout << std::endl;
    }
    size_t NumberOfElements() const {
        return nrElements;
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
    ~HistogramSet() {
        //		tuple_for_each(histograms, [&](auto tHistograms) {
        //			deletePointerVector(tHistograms);
        //		});
    }
    HistogramSet() {
        
    }
    /** Copy Assignment Operator */
    HistogramSet& operator= (const HistogramSet& other)
    {
        histograms = other.histograms;
        return *this;
    }
    /** Copy Constructor */
    HistogramSet (const HistogramSet& other)
    {
        histograms = other.histograms;
    }
    const std::tuple<std::vector<Histogram<Ts>>...>& GetAllHistograms() {
        return histograms;
    }
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
        auto hist = histogramRegister[name];
        if(hist) {
            return hist;
        }
        else {
            auto tHistograms = GetHistograms<T>();
            
            for(size_t i = 0 ; i < tHistograms->size() ; ++i) {
                auto hist = tHistograms->at(i);
                if(hist.name == name) {
                    histogramRegister[name] = &tHistograms->at(i);
                    return &tHistograms->at(i);
                }
            }
        }
        return NULL;
    }
    template<typename T>
    void AddToHistogram(const std::string name,const T& value) {
        //		printf("name = %s\n",name.c_str());
        Histogram<T>* histogram = NULL;
        histogram = GetHistogram<T>(name);
        //TODO: Create histogram when set to dynamic and histogram was not found
        histogram->Add(value);
    }
    
    template<typename T>
    void AddHistogram(const Histogram<T>& hist) {
        std::vector<Histogram<T>>* histograms = GetHistograms<T>();
        
        histograms->push_back(hist);
        auto newHistogram = &histograms->at(histograms->size() - 1);
        std::get<std::map<std::string,Histogram<T>*>>(histogramRegisters)[hist.name] = newHistogram;
    }
    
    //This assumes the right hand operand histogram set is a superset of this histogram set, throws a runtime_error if a histogram in this set could not be found.
    HistogramSet operator-(HistogramSet<Ts...>& other) {
        //		return compare<Ts...>();
        
        float total = 0;
        HistogramSet result;
        
        tuple_for_each(histograms, [&](auto tHistograms) {
            for(auto histogram : tHistograms) {
                
                
                auto otherHistogram = other.GetHistogram<decltype(histogram->LowerBound())>(histogram->name);
                if(!otherHistogram) {
                    throw std::runtime_error("Histogram " + histogram->name + " not found in right hand operand");
                }
                
                auto diff = (*histogram) - (*otherHistogram);
                result.AddHistogram(diff);
            }
        });
        
        return total;
    }
    
    float DistanceTo(HistogramSet<Ts...>& other) {
        float total = 0;
        int nrHistograms = 0;
        
        tuple_for_each(histograms, [&](auto tHistograms) {
            
            for(auto& histogram : tHistograms) {
                auto otherHistogram = other.GetHistogram<decltype(histogram.LowerBound())>(histogram.name);
                if(!otherHistogram) {
                    throw std::runtime_error("Histogram " + histogram.name + " not found in right hand operand");
                }
                ++nrHistograms;
                total += histogram.DistanceTo(otherHistogram);
            }
        });
        return total / nrHistograms;
    }
    
    void Print() {
        printf("Histogram set : \n");
        tuple_for_each(histograms, [&](auto tHistograms) {
            for(auto& histogram : tHistograms) {
                histogram.Print();
            }
        });
    }
    void Join(const HistogramSet& other) {
        tuple_for_each(other.histograms, [&](auto tHistograms) {
            for(auto& histogram : tHistograms) {
                this->AddHistogram(histogram);
            }
        });
    }
    void Clear() {
        tuple_for_each(histograms, [&](auto tHistograms) {
            for(auto& histogram : tHistograms) {
                histogram.Clear();
            }
        });
    }
};

template <typename T>
class Histogram2D {
private:
    std::map<unsigned int,Histogram<T>> histograms;
    std::map<unsigned int,float> cdf;
    
    T lowerBound;
    T upperBound;
    bool cdfStale = true;
    
    unsigned int xBins = 0;
    unsigned int yBins = 0;
    
    float binWidth = 0;
    size_t nrElements = 0;
public:
    //    Histogram2D& operator= (const Histogram2D& other)
    //    {
    //        if (this != &other) // protect against invalid self-assignment
    //        {
    //            lowerBound = other.lowerBound;
    //            upperBound = other.upperBound;
    //
    //            histograms = other.histograms;
    //
    //            cdf = other.cdf;
    //            cdfStale = other.cdfStale;
    //            binWidth = other.binWidth;
    //            nrElements = other.nrElements;
    //        }
    //        // by convention, always return *this
    //        return *this;
    //    }
    std::string name;
    std::pair<unsigned int,unsigned int> NumberOfBins() {
        return std::pair<unsigned int, unsigned int>(xBins,yBins);
    }
    T LowerBound() {
        return lowerBound;
    }
    T UpperBound() {
        return upperBound;
    }
    void Set(unsigned int binX, const Histogram<T>& newHistogram) {
        histograms[binX] = newHistogram;
        nrElements += newHistogram.NumberOfElements();
    }
    void SetBinValue(unsigned int binOne, unsigned int binTwo, int newValue) {
        Histogram<T>& histogram = histograms[binOne];
        size_t oldNrElements = histogram.NumberOfElements();
        
        histogram.Set(binTwo,newValue);
        size_t newNrElements = histogram.NumberOfElements();
        
        nrElements += (newNrElements - oldNrElements);
        
        //        printf("newNrElements = %zu\n",nrElements);
    }
    void Add(T valueOne, T valueTwo) {
        unsigned int bin = BinForValue(valueOne);
        histograms[bin].Add(valueTwo);
        ++nrElements;
    }
    void Add(T valueOne, T valueTwo, size_t magnitude) {
        unsigned int bin = BinForValue(valueOne);
        histograms[bin].Add(valueTwo, magnitude);
        nrElements += magnitude;
    }
    void Add(const std::pair<T,T>& values) {
        Add(values.first, values.second);
    }
    int BinForValue(const T& value) {
        if(value <= lowerBound) {
            return 0;
        }
        else if(value >= upperBound) {
            return xBins - 1;
        }
        T delta = upperBound - lowerBound;
        double interpolated = (double)(value - lowerBound) / (delta);
        unsigned int bin = std::floor(interpolated * (xBins));
        return bin;
    }
    Histogram2D() {
        
    }
    Histogram2D(T lowerBound, T upperBound, unsigned int bins) : upperBound(upperBound), lowerBound(lowerBound), xBins(bins), yBins(bins) {
        if(upperBound <= lowerBound) {
            throw std::runtime_error("Lower bound should be < upper bound");
        }
        if(bins) {
            throw std::runtime_error("Number of bins must be positive (> 0)\n");
        }
        binWidth = (upperBound - lowerBound) / (float)xBins;
        
        for(int i = 0 ; i < xBins ; ++i) {
            Histogram<T> newHistogram("2DHistogram x="+std::to_string(i), lowerBound, upperBound, yBins);
            histograms[i] = newHistogram;
        }
    }
    Histogram2D(const std::string& name, T lowerBound, T upperBound, unsigned int xBins, unsigned int yBins) : name(name), upperBound(upperBound), lowerBound(lowerBound), xBins(xBins), yBins(yBins) {
        if(!xBins) {
            throw std::runtime_error("Number of x bins must be positive (> 0)\n");
        }
        if(!yBins) {
            throw std::runtime_error("Number of y bins must be positive (> 0)\n");
        }
        if(upperBound <= lowerBound) {
            throw std::runtime_error("Lower bound should be < upper bound");
        }
        binWidth = (upperBound - lowerBound) / (float)xBins;
        
        for(int i = 0 ; i < xBins ; ++i) {
            histograms[i] = Histogram<T>("2DHistogram", lowerBound, upperBound, yBins);
        }
    }
    Histogram2D(T lowerBound, T upperBound, unsigned int xBins, unsigned int yBins) : name("<NONE>"), upperBound(upperBound), lowerBound(lowerBound), xBins(xBins), yBins(yBins) {
        if(!xBins) {
            throw std::runtime_error("Number of x bins must be positive (> 0)\n");
        }
        if(!yBins) {
            throw std::runtime_error("Number of y bins must be positive (> 0)\n");
        }
        if(upperBound <= lowerBound) {
            throw std::runtime_error("Lower bound should be < upper bound");
        }
        binWidth = (upperBound - lowerBound) / (float)xBins;
        
        for(int i = 0 ; i < xBins ; ++i) {
            histograms[i] = Histogram<T>("2DHistogram", lowerBound, upperBound, yBins);
        }
    }
    Histogram2D(Grid& grid) {
        xBins = grid.GetWidth();
        yBins = grid.GetHeight();
        
        lowerBound = 0;
        upperBound = grid.GetWidth();
        
        nrElements = 0;
        
        for(int x = 0 ; x < xBins ; ++x) {
            auto newHistogram = Histogram<T>("2DHistogram", 0, grid.GetHeight(), yBins);
            
            for(int y = 0 ; y < yBins ; ++y) {
                int filled = grid.IsFilled(x,y);
                
                newHistogram.Set(y,filled);
                
                nrElements += filled;
            }
            
            histograms[x] = newHistogram;
        }
    }
    void Clear() {
        nrElements = 0;
        for(auto it : histograms) {
            it.second.Clear();
        }
    }
    int MaxBinValue() {
        int maxValue = 0;
        for(auto it : histograms) {
            int localMax = it.second.MaximumValue();
            if(localMax > maxValue) {
                maxValue = localMax;
            }
        }
        return maxValue;
    }
    int MinBinValue() {
        int minValue = std::numeric_limits<int>::max();
        for(auto it : histograms) {
            int localMin = it.second.MinimumValue();
            if(localMin < minValue) {
                minValue = localMin;
            }
        }
        return minValue;
    }
    unsigned int SampleBin() {
        if(cdfStale)
            ComputeCDF();
        
        float R = random() / (float)RAND_MAX;
        
        for(int i = 0 ; i < xBins ; ++i) {
            if(R <= cdf[i]) {
                return i;
            }
        }
        return xBins - 1;
    }
    std::pair<unsigned int, unsigned int> SampleBins() {
        std::pair<unsigned int, unsigned int> binsPair;
        
        if(cdfStale)
            ComputeCDF();
        
        float R = random() / (float)RAND_MAX;
        
        for(int i = 0 ; i < xBins ; ++i) {
            if(R <= cdf[i]) {
                binsPair.first = i;
                binsPair.second = histograms[i].SampleBin();
                return binsPair;
            }
        }
    }
    void ComputeCDF() {
        cdf.clear();
        float cummulative = 0;
        for(unsigned int i = 0 ; i < xBins ; ++i) {
            cummulative += histograms[i].NumberOfElements() / (float)nrElements;
            cdf[i] = cummulative;
        }
        cdfStale = false;
    }
    float Sample() {
        unsigned int bin = SampleBin();
        
        //Jitter the result within the bin
        float R = rand() / (float)RAND_MAX;
        float offset = R * binWidth;
        float sampledValue = (bin + lowerBound) + offset;
        return sampledValue;
    }
    float ScaledValue(unsigned int binX, unsigned int binY) {
        return histograms[binX].ScaledValue(binY);
    }
    std::pair<float,float> Sample2D() {
        
        if(nrElements) {
            std::pair<float,float> sample;
            
            unsigned int bin = SampleBin();
            
            //Jitter the result within the bin
            float R = rand() / (float)RAND_MAX;
            float offset = R * binWidth;
            float sampledValue = (bin * binWidth + lowerBound) + offset;
            
            sample.first = sampledValue;
            sample.second = histograms[bin].Sample();
            
            return sample;
        }
        return std::pair<float,float>(0,0);
    }
    int BinValue(unsigned int binX, unsigned int binY) {
        return histograms[binX].BinValue(binY);
    }
    Histogram<T>* BinValue(unsigned int binX) {
        return &histograms[binX];
    }
    float NormalizedValue(unsigned int binOne, unsigned int binTwo) {
        if(nrElements) {
            return BinValue(binOne,binTwo) / (float)nrElements;
        }
        return 0;
    }
    size_t NumberOfElements() {
        return nrElements;
    }
    float NormalizedMean() {
        return 1.F / (xBins * yBins);
    }
    float NormalizedVariance() {
        float var = 0;
        float mean = NormalizedMean();
        for(int i = 0 ; i < xBins ; i++) {
            for(int j = 0 ; j < yBins ; j++) {
                var += std::pow(NormalizedValue(i, j) - mean,2);
            }
        }
        return std::pow(var,0.5);
    }
    void PrintRaw() {
        printf("2D Histogram : \n");
        printf("Number of Elements = %zu\n",nrElements);
        //        printf("Mean = %f\n",Mean());
        //        printf("Variance = %f\n",Variance());
        if(nrElements) {
            for(int j = 0 ; j < yBins ; j++) {
                for(int i = 0 ; i < xBins ; i++) {
                    T normValue = BinValue(i, j);
                    std::cout << normValue << "\t";
                    //                    printf("%.2f\t",normValue);
                }
                printf("\n");
            }
        }
        else printf("<EMPTY>");
    }
    void Print() {
        printf("2D Histogram : \n");
        printf("Number of Elements = %zu\n",nrElements);
        printf("Mean = %f\n",NormalizedMean());
        printf("Variance = %f\n",NormalizedVariance());
        if(nrElements) {
            for(int j = 0 ; j < yBins ; j++) {
                for(int i = 0 ; i < xBins ; i++) {
                    float normValue = NormalizedValue(i, j);
                    printf("%.2f\t",normValue);
                }
                printf("\n");
            }
        }
        else printf("<EMPTY>");
    }
    void SmoothRectangular(unsigned int width, unsigned int height, const unsigned int repeat = 1) {
        unsigned int size = width * height;
        if(width <= xBins && height <= yBins) {
            for(int i = 0 ; i < repeat ; ++i) {
                Histogram2D<T> result = Histogram2D<T>(lowerBound,upperBound,xBins,yBins);
                for(unsigned int x = 0 ; x < xBins ; ++x) {
                    for(unsigned int y = 0 ; y < yBins ; ++y) {
                        int upperX = std::min(x + (width - 1) / 2,xBins - 1);
                        int lowerX = std::max((int)(x - (width - 1) / 2),0);
                        float sum = 0;
                        int binsSummed = 0;
                        for(int xFilter = lowerX ; xFilter <= upperX ; ++xFilter) {
                            unsigned int upperY = std::min(y + (height - 1) / 2,yBins - 1);
                            unsigned int lowerY = std::max((int)(y - (height - 1) / 2),0);
                            for(int yFilter = lowerY ; yFilter <= upperY ; ++yFilter) {
                                size_t value = BinValue(xFilter, yFilter);
                                //                            printf("Value at filter %d,%d = %zu\n",xFilter,yFilter,value);
                                sum += value;
                                ++binsSummed;
                            }
                        }
//                        if(sum > 0) {
//                            sum *= binsSummed;
//                        }
                        size_t average = std::round(sum / binsSummed);
                        //                    size_t average = std::ceil(sum / binsSummed);
                        
                        //                    int left = sum - average;
                        //                    size_t newValue = (size_t)
                        //                    printf("new value (%d,%d) = %zu\n",x,y,newValue);
                        result.SetBinValue(x,y,average);
                    }
                }
                
                int lost = this->NumberOfElements() - result.NumberOfElements();
                printf("Smoothing lost %d elements.\n",lost);
                
                *this = result;
            }
        }
        else throw std::runtime_error("Smoothing kernel size too big");
    }
    void SmoothRectangular(unsigned int size) {
        SmoothRectangular(size, size);
    }
    void GammaCorrection(double gamma) {
        
        Histogram2D<T> result = Histogram2D<T>(lowerBound,upperBound,xBins,yBins);
        
        for(int x = 0 ; x < xBins ; ++x) {
            for(int y = 0 ; y < yBins ; ++y) {
                
                int value = BinValue(x, y);
                
                int gammaCorrectedValue = std::pow(value,gamma);
                result.SetBinValue(x,y,gammaCorrectedValue);
            }
        }
        
        *this = result;
    }
    Histogram2D<T> operator+(Histogram2D<T>& right) {
        
        T lowerBound = std::min(LowerBound(),right.LowerBound());
        T upperBound = std::max(UpperBound(),right.UpperBound());
        
        Histogram2D<T> result = Histogram2D<T>(lowerBound,upperBound,xBins,yBins);
        
        for(int x = 0 ; x < xBins ; ++x) {
            auto sum = histograms[x] + *right.BinValue(x);
            result.Set(x,sum);
        }
        
        return result;
    }
    Histogram2D<T> BooleanHistogram() {
        Histogram2D<T> result = Histogram2D<T>(lowerBound,upperBound,xBins,yBins);
        
        for(int x = 0 ; x < xBins ; ++x) {
            auto booleanHist = histograms[x].BooleanHistogram();
            result.Set(x,booleanHist);
        }
        
        return result;
    }
    Grid ToGrid() {
        std::vector<std::vector<bool>> gridCells = std::vector<std::vector<bool>>(yBins,std::vector<bool>(xBins));
        
        for(int x = 0 ; x < xBins ; ++x) {
            for(int y = 0 ; y < yBins ; ++y) {
                gridCells[y][x] = (bool)BinValue(x,y);
            }
        }
        
        return Grid(gridCells);
    }
    Histogram2D<T> operator-(Histogram2D<T>& right) {
        
        T lowerBound = std::min(LowerBound(),right.LowerBound());
        T upperBound = std::max(UpperBound(),right.UpperBound());
        
        Histogram2D<T> result = Histogram2D<T>(lowerBound,upperBound,xBins,yBins);
        
        for(int x = 0 ; x < xBins ; ++x) {
            auto sum = histograms[x] - *right.BinValue(x);
            result.Set(x,sum);
        }
        
        return result;
    }
    Histogram2D operator*(Histogram2D& other) {
        std::pair<unsigned int,unsigned int> bins = other.NumberOfBins();
        
        if(bins.first != xBins || bins.second != yBins) {
            throw std::runtime_error("Number of bins of histograms must match");
        }
        
        Histogram2D<float> result(0,1,xBins,yBins);
        for(int xBin = 0 ; xBin < xBins ; ++xBin) {
            for(int yBin = 0 ; yBin < yBins ; ++yBin ) {
                
                //                float product = pixelDistributionTwo->BinValue(xBin, yBin) * throughputDistroTwo->BinValue(xBin, yBin);
                unsigned int thisValue = BinValue(xBin, yBin);
                unsigned int otherValue = other.BinValue(xBin, yBin);
                unsigned int product = thisValue * otherValue;
                //                printf("distro * throughput = %d * %d = %d\n",thisValue,otherValue,product);
                result.SetBinValue(xBin, yBin,product);
                
            }
        }
        
        return result;
    }
};
template <typename... Ts>
class Histogram2DSet {
private:
    std::tuple<std::vector<Histogram2D<Ts>>...> histograms;
    std::tuple<std::map<std::string,Histogram2D<Ts>*>...> histogramRegisters; //Maps a name to a histogram pointer
    
    //When set to dynamic the set will create new histograms for values added to not yet exsistent histograms
    //	bool dynamic = true;
    
    //	std::tuple<std::vector<Ts>...> values;
    //	std::tuple<std::vector<Histogram<float>>,std::vector<Histogram<ushort>>> histograms;
    //	std::tuple<int> ints;
    
public:
//    ~Histogram2DSet() {
        //		tuple_for_each(histograms, [&](auto tHistograms) {
        //			deletePointerVector(tHistograms);
        //		});
//    }
    Histogram2DSet() {
        
    }
    /** Copy Assignment Operator */
//    Histogram2DSet& operator= (const Histogram2DSet& other)
//    {
//        histograms = other.histograms;
//        return *this;
//    }
    /** Copy Constructor */
//    Histogram2DSet (const Histogram2DSet& other)
//    {
//        histograms = other.histograms;
//    }
    const std::tuple<std::vector<Histogram2D<Ts>>...>& GetAllHistograms() {
        return histograms;
    }
    template<typename T>
    std::vector<Histogram2D<T>>* GetHistograms() {
        return &std::get<std::vector<Histogram2D<T>>>(histograms);
    }
    
    template<typename T>
    std::map<std::string,Histogram2D<T>*>& GetHistogramRegister() {
        return std::get<std::map<std::string,Histogram2D<T>*>>(histogramRegisters);
    }
    
    template<typename T>
    Histogram2D<T>* GetHistogram(const std::string& name) {
        std::map<std::string,Histogram2D<T>*>& histogramRegister = GetHistogramRegister<T>();
        auto hist = histogramRegister[name];
        if(hist) {
            return hist;
        }
        else {
            auto tHistograms = GetHistograms<T>();
            
            for(size_t i = 0 ; i < tHistograms->size() ; ++i) {
                auto hist = tHistograms->at(i);
                if(hist.name == name) {
                    histogramRegister[name] = &tHistograms->at(i);
                    return &tHistograms->at(i);
                }
            }
        }
        return NULL;
    }
    template<typename T>
    void AddToHistogram(const std::string name,  const T& valueOne,const T& valueTwo,unsigned int magnitude = 1) {
        //        printf("name = %s\n",name.c_str());
        Histogram2D<T>* histogram = NULL;
        histogram = GetHistogram<T>(name);
        //TODO: Create histogram when set to dynamic and histogram was not found
        histogram->Add(valueOne,valueTwo,magnitude);
    }
    template<typename T>
    void AddHistogram(const std::string& name, const Histogram2D<T>& hist) {
        std::vector<Histogram2D<T>>* histograms = GetHistograms<T>();
        
        histograms->push_back(hist);
        auto newHistogram = &histograms->at(histograms->size() - 1);
        auto map = std::get<std::map<std::string,Histogram2D<T>*>>(histogramRegisters);
        map[name] = newHistogram;
    }
    void Print() {
        printf("2DHistogram set : \n");
        tuple_for_each(histograms, [&](auto tHistograms) {
            for(auto& histogram : tHistograms) {
                histogram.Print();
            }
        });
    }
    void Clear() {
        tuple_for_each(histograms, [&](auto tHistograms) {
            for(auto& histogram : tHistograms) {
                histogram.Clear();
            }
        });
    }
};


#endif /* defined(__embree__Histogram__) */
