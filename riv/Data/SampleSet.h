//
//  SampleSet.h
//  embree
//
//  Created by Gerard Simons on 01/12/14.
//
//

#ifndef embree_SampleSet_h
#define embree_SampleSet_h

#include <vector>

template<typename T>
class SampleSet {
private:
	std::vector<T> samples;
public:
	SampleSet() {
		
	}
	SampleSet(const std::vector<T>& samples) : samples(samples) {
		
	}
	SampleSet operator+(const SampleSet& otherSamples) {
		
		std::vector<T> combined = samples;
		combined.insert(combined.end(),otherSamples.samples.begin(),otherSamples.samples.end());
		
		return SampleSet<T>(combined);
	}
	void Print() {
		std::cout << "{";
		for(size_t i = 0 ; i < samples.size() - 1 ; ++i) {
			std::cout << samples[i] << ", ";
		}
		if(samples.size() > 0) std::cout << samples[samples.size() - 1];
		std::cout << "}" << std::endl;
	}
	SampleSet Resample(size_t N = 0) {
		if(N == 0) {
			N = samples.size();
		}
		std::vector<T> newSamples(N);
		for(size_t i = 0 ; i < N ; i++) {
			size_t index = rand() % samples.size();
			newSamples[i] = samples[index];
		}
		return SampleSet(newSamples);
	}
	const std::vector<T>& GetSamples() const {
		return samples;
	}
};


#endif
