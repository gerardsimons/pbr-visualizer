//
//  Cluster.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 02/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "Cluster.h"
#include "../helper.h"
#include <fstream>


//K-means clustering in 3D space

RIVCluster::~RIVCluster() {
    delete medoid;
    memberIndices.clear();
}
RIVCluster::RIVCluster(const size_t& medoidIndex_) {
    medoidIndex = medoidIndex_;
}
RIVCluster::RIVCluster(size_t medoidIndex_, std::vector<float>* xValues_, std::vector<float>* yValues_, std::vector<float>* zValues_, size_t id_)
{
    //        costInvalid = true;
    
    xValues = xValues_;
    yValues = yValues_;
    zValues = zValues_;
    id = id_;
    medoid = nullptr;
    medoidIndex = medoidIndex_;
}
void RIVCluster::AddMember(const size_t& memberIndex) {
    memberIndices.push_back(memberIndex);
}
float RIVCluster::DistanceTo(const Point3D& point) {
    return euclideanDistance(point.x, (*xValues)[medoidIndex], point.y, (*yValues)[medoidIndex], point.z, (*zValues)[medoidIndex]);
}
double RIVCluster::Cost() {
    if(!memberIndices.empty()) {
        cost = 0;
        //Do we need to sample?
        if(memberIndices.size() < maxSample || maxSample == -1) {
            //                printf("Evaluating all samples for cost analysis\n");
            for(size_t index : memberIndices) {
                double thisCost = euclideanDistance((*xValues)[index], (*xValues)[medoidIndex], (*yValues)[index], (*yValues)[medoidIndex], (*zValues)[index], (*zValues)[medoidIndex]);
                cost += thisCost;
            }
        }
        else {
            //                printf("Evaluating %zu of %zu (%f%%) samples for cost analysis\n",maxSample,memberIndices.size(),maxSample / (float)memberIndices.size());
            NonReplacementSampler<size_t> sampler = NonReplacementSampler<size_t>(&memberIndices);
            for(size_t i = 0 ; i < maxSample ;i++) {
                //Find values from tabled
                size_t index = sampler.RequestSample();
                cost += euclideanDistance((*xValues)[index], (*xValues)[medoidIndex], (*yValues)[index], (*yValues)[medoidIndex], (*zValues)[index], (*zValues)[medoidIndex]);
                
            }
        }
        //            costInvalid = false;
    }
    return cost;
}
double RIVCluster::LastCost() const {
    return cost;
}
void RIVCluster::SwapMedoid(size_t memberIndex) {
    //        printf("Swapping medoid to %zu\n",memberIndices[memberIndex]);
    
    backSwapIndex = memberIndex;
    size_t temp = memberIndices[memberIndex];
    memberIndices[memberIndex] = medoidIndex;
    medoidIndex = temp;
    //        if(medoid) {
    //            delete medoid;
    //        }
    //        costInvalid = true;
}
void RIVCluster::SwapBack() {
    size_t temp = medoidIndex;
    medoidIndex = memberIndices[backSwapIndex];
    memberIndices[backSwapIndex] = temp;
    //        if(medoid) {
    //            delete medoid;
    //        }
    //        costInvalid = true;
}
Point3D RIVCluster::GetMedoid() {
    
    return Point3D(xValues->at(medoidIndex),yValues->at(medoidIndex),zValues->at(medoidIndex));
    
    //        return medoid;
}
size_t RIVCluster::GetMedoidIndex() const {
    return medoidIndex;
}
size_t RIVCluster::GetMemberIndex(const size_t& memberIndex ) const {
    return memberIndices[memberIndex];
}
size_t RIVCluster::MembersSize() const {
    //        printf("Cluster %zu has size %zu\n",id,memberIndices.size());
    return memberIndices.size();
}
size_t RIVCluster::Size() const {
    return MembersSize() + 1;
}
//Does not include the medoid itself!
std::vector<size_t> RIVCluster::GetMemberIndices() {
    return memberIndices;
}
double RIVCluster::Optimize() {
    printf("Optimizing cluster %zu...  ",id);
    double bestCost = Cost();
    for(size_t i = 0 ; i < memberIndices.size() ; ++i) {
        SwapMedoid(i);
        double newCost = Cost();
        if(newCost < bestCost) {
            bestCost = newCost;
        }
        else {
            SwapBack();
        }
    }
    printf("\n");
    return bestCost;
}
bool RIVCluster::HasMember(const size_t& memberIndex) {
    if(memberIndices.size() > 0) {
        for(size_t member : memberIndices) {
            if(member == memberIndex) {
                return true;
            }
        }
    }
    return false;
}

bool RIVCluster::SaveToFileStream(const std::ofstream& output) {
    return false;
}