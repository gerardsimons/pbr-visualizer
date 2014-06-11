//
//  Cluster.h
//  Afstuderen
//
//  Created by Gerard Simons on 02/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__Cluster__
#define __Afstuderen__Cluster__

#include <vector>
#include "Geometry.h"
#include "helper.h"
#include <algorithm>

class RIVCluster {
private:
    //Members are defined as rows of a given table, when memory runs low, these may be (partially) cleared
    std::vector<size_t> memberIndices;
    //medoidIndex should always be defined
    size_t medoidIndex;
    size_t backSwapIndex;
    
    //Pointers to the values
    std::vector<float>* xValues;
    std::vector<float>* yValues;
    std::vector<float>* zValues;
    
    //Cached properties
    Point3D* medoid;
//    bool costInvalid;
    double cost;

    int* sampledIndices;
    size_t* randomIndices = NULL;
    const size_t maxSample = 1000;
public:
    size_t id;
    ~RIVCluster() {
        delete medoid;
        memberIndices.clear();
    }
    RIVCluster(size_t medoidIndex_, std::vector<float>* xValues_, std::vector<float>* yValues_, std::vector<float>* zValues_, size_t id_)
    {
//        costInvalid = true;

        xValues = xValues_;
        yValues = yValues_;
        zValues = zValues_;
        id = id_;
        medoid = nullptr;
        medoidIndex = medoidIndex_;
    }
    void AddMember(const size_t& memberIndex) {
        memberIndices.push_back(memberIndex);
    }
    float DistanceTo(const Point3D& point) {
        return euclideanDistance(point.x, (*xValues)[medoidIndex], point.y, (*yValues)[medoidIndex], point.z, (*zValues)[medoidIndex]);
    }
    double Cost() {
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
    double LastCost() const {
        return cost;
    }
    void SwapMedoid(size_t memberIndex) {
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
    void SwapBack() {
        size_t temp = medoidIndex;
        medoidIndex = memberIndices[backSwapIndex];
        memberIndices[backSwapIndex] = temp;
//        if(medoid) {
//            delete medoid;
//        }
//        costInvalid = true;
    }
    Point3D GetMedoid() {

            return Point3D(xValues->at(medoidIndex),yValues->at(medoidIndex),zValues->at(medoidIndex));

//        return medoid;
    }
    size_t GetMedoidIndex() const {
        return medoidIndex;
    }
    size_t GetMemberIndex(const size_t& memberIndex ) const {
        return memberIndices[memberIndex];
    }
    size_t MembersSize() const {
//        printf("Cluster %zu has size %zu\n",id,memberIndices.size());
        return memberIndices.size();
    }
    size_t Size() const {
        return MembersSize() + 1;
    }
    //Does not include the medoid itself!
    std::vector<size_t> GetMemberIndices() {
        return memberIndices;
    }
    double Optimize() {
        printf("Optimizing cluster %zu...  ",id);
//        size_t changes = 1;
        double bestCost = Cost();
        double DEBUG_oldCost = bestCost;
        for(size_t i = 0 ; i < memberIndices.size() ; ++i) {
//            printf("Cost to beat = %f\n",bestCost);
//            printf("member index %zu ",i);
            SwapMedoid(i);

            double newCost = Cost();
            if(newCost < bestCost) {
//                printf(" is better");
//                printf("Cost to beat = %f\n",bestCost);
                bestCost = newCost; //Leave it
            }
            else {
//                printf(" is not better");
                SwapBack();
            }
        }
        printf("Cost %f > %f\n",DEBUG_oldCost,bestCost);
//            reporter::update(optimizeClusterTask);
//            std::cout << "\n************ CLUSTER OPTIMIZATION REPORT *************\n";
//            std::cout << *this;
//            std::cout << "\n******************************************************\n";
            //                printf("%zu changes made to clusters.\n",changes);
        return bestCost;
    }
    friend std::ostream& operator<<(std::ostream& os, const RIVCluster& c)
    {
        os << "cluster #" << c.id << " M = " << "(" << c.xValues->at(c.medoidIndex) << "," << c.yValues->at(c.medoidIndex) << "," << c.zValues->at(c.medoidIndex) << ") " << "with " << c.MembersSize() << " members: ";
        
        for(size_t i : c.memberIndices) {
            os << "(" << c.xValues->at(i) << "," << c.yValues->at(i) << "," << c.zValues->at(i) << ") ";
        }
        
        os << " cost = " << c.LastCost() << "\n";
        
        return os;
    }

};


#endif /* defined(__Afstuderen__Cluster__) */
