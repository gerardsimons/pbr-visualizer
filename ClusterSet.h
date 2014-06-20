//
//  ClusterSet.h
//  Afstuderen
//
//  Created by Gerard Simons on 07/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__ClusterSet__
#define __Afstuderen__ClusterSet__

#include <iostream>
#include <algorithm>
#include "Cluster.h"
#include "Evaluator.h"
//#include "reporter.h"

class RIVClusterSet {
private:
    std::vector<RIVCluster*> clusters;
    std::pair<size_t,size_t>* minMax = NULL;
    LinearInterpolator<size_t>* interpolator; //Used for computing relative sizes
    
    std::vector<float>* xValues;
    std::vector<float>* yValues;
    std::vector<float>* zValues;

    //TODO: create map as a register of what index belongs to what clusters
    const static int MAX_COMBINATIONS = 10000;
    
    NonReplacementSampler<size_t> sampler;
    

    double cost = 0.F;
    bool initialized = false;
    bool membersAssigned = false;
    size_t K = 0;
public:
    RIVClusterSet(std::vector<RIVCluster*>& clusters_);
    RIVClusterSet(const size_t& K_, std::vector<float>* xValues_, std::vector<float>* yValues_, std::vector<float>* zValues_);
    ~RIVClusterSet();
    RIVClusterSet();
    void AddCluster(RIVCluster* cluster);
    
    RIVCluster* GetCluster(const size_t& index) const;
    std::vector<RIVCluster*>& GetClusters();
//    std::vector<RIVCluster*>* GetClustersPointer();
    
    void Initialize();
    void Initialize(std::vector<size_t> indices);
    bool SaveToFile(const std::string& fileName);
    std::vector<size_t> GetMedoidIndices();
    void AssignMembers();
    void OptimizeClusters();
    RIVCluster* ClusterForMemberIndex(const size_t& mIndex) const;
    double TotalCost();
    static RIVClusterSet MakeCluster(const size_t& maxRepeat, const size_t& K, std::vector<float>* xValues, std::vector<float>* yValues, std::vector<float>* zValues);
    std::pair<size_t,size_t>* MinMaxClusterSize();
    float RelativeSizeOf(RIVCluster* cluster);
    size_t MaxSize();
    size_t Size() const;
    void SanityCheck();
    friend std::ostream& operator<<(std::ostream& os, const RIVClusterSet& c)
    {
        os << "ClusterSet with " << c.clusters.size() << " members. Total cost = " << c.cost << "\n";
        os << " clusters : ";
        for(RIVCluster* cluster : c.clusters) {
            os << "\t" << (*cluster) << std::endl;
        }
        return os;
    }
};


#endif /* defined(__Afstuderen__ClusterSet__) */
