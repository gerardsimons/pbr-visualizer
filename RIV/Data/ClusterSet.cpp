//
//  ClusterSet.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 07/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//



//
//  ClusterSet.h
//  Afstuderen
//
//  Created by Gerard Simons on 07/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//


#include <fstream>
#include <iostream>
#include <algorithm>

#include "ClusterSet.h"
#include "Cluster.h"
//#include "reporter.h"



RIVClusterSet::~RIVClusterSet() {
    if(minMax)
        delete minMax;
    //Delete all the pointers and the vector itself
    
//    for(auto &it:clusters) delete it; clusters.clear();
    clusters.clear();
    if(interpolator)
        delete interpolator;
}
RIVClusterSet::RIVClusterSet() {
    //Do nothing
    minMax = NULL;
    interpolator = NULL;
}
RIVClusterSet::RIVClusterSet(std::vector<RIVCluster*>& clusters_) {
    clusters = clusters_;
    K = clusters.size();
    minMax = NULL;
    interpolator = NULL;
}
RIVClusterSet::RIVClusterSet(const size_t& K_, std::vector<float>* xValues_, std::vector<float>* yValues_, std::vector<float>* zValues_) {
    K = K_;
    xValues = xValues_;
    yValues = yValues_;
    zValues = zValues_;
    minMax = NULL;
    interpolator = NULL;
}
void RIVClusterSet::AddCluster(RIVCluster* cluster) {
    clusters.push_back(cluster);
}
RIVCluster* RIVClusterSet::GetCluster(const size_t& index) const {
    if(index < clusters.size()) {
        return clusters[index];
    }
    throw "Warning: requested cluster out of cluster set bounds.";
    return NULL;
}
//TODO: make cache map for this
RIVCluster* RIVClusterSet::ClusterForMemberIndex(const size_t& mIndex) const {
    for(RIVCluster* cluster : clusters) {
        if(cluster->HasMember(mIndex) || cluster->GetMedoidIndex() == mIndex) {
            return cluster;
        }
    }
    return NULL;
}
std::vector<RIVCluster*>& RIVClusterSet::GetClusters() {
    return clusters;
}
//std::vector<RIVCluster*>* RIVClusterSet::GetClustersPointer() {
//    return &clusters;
//}
//This should hold after assigning the members
void RIVClusterSet::SanityCheck() {
    for(size_t i = 0 ; i < clusters.size() ; ++i) {
        RIVCluster *ownerCluster = clusters[i];
        for(size_t memberIndex : ownerCluster->GetMemberIndices()) {
            Point3D memberPoint = Point3D(xValues->at(memberIndex),yValues->at(memberIndex),zValues->at(memberIndex));
//            std::cout << "member point = " << memberPoint;
            double ownerDistance = ownerCluster->DistanceTo(memberPoint);
            for(RIVCluster *otherCluster : clusters) {
                if(otherCluster->id != ownerCluster->id) {
                    double otherDistance = otherCluster->DistanceTo(memberPoint);
                    if(otherDistance < ownerDistance) {
                        throw "Some member is closer to another cluster.";
                    }
                }
            }
        }
    }
}
void RIVClusterSet::Initialize() {
    sampler = NonReplacementSampler<size_t>(0, xValues->size());
    for(auto &it:clusters) delete it; clusters.clear();
//    std::vector<size_t> debug_Sampled;
    for(size_t i = 0 ; i < K ; ++i) {
        size_t index = sampler.RequestSample();
//        debug_Sampled.push_back(index);
        RIVCluster* cluster = new RIVCluster(index, xValues, yValues, zValues, i);
        clusters.push_back(cluster);
    }
//    printf("sampled initialization indices = ");
//    printVector(debug_Sampled);
    initialized = true;
}
void RIVClusterSet::Initialize(std::vector<size_t> indices) {
    for(auto &it:clusters) delete it; clusters.clear();
    if(indices.size() != K) {
        throw "Invalid number of init indices supplied. Must match K";
    }
//    if(indices[0] == 0 && indices[1] == 1) {
//        
//    }
    for(size_t& i : indices) {
        RIVCluster* cluster = new RIVCluster(i,xValues,yValues,zValues,i);
        clusters.push_back(cluster);
    }
    initialized = true;
}
std::vector<size_t> RIVClusterSet::GetMedoidIndices() {
    std::vector<size_t> medoidIndices;
    for(RIVCluster *cluster : clusters) {
        medoidIndices.push_back(cluster->GetMedoidIndex());
    }
    return medoidIndices;
}
bool RIVClusterSet::SaveToFile(const std::string& fileName) {
    std::ofstream output;
    output.open (fileName.c_str());
    if(output.is_open()) {
        for(RIVCluster* child : clusters) {
            child->SaveToFileStream(output);
        }
        output.close();
    }
    return 0;
}
void RIVClusterSet::AssignMembers() {
    if(!initialized) {
        throw std::string("Initialization is required before assignment is possible.");
    }
    std::vector<size_t> allMedoids;
    for(RIVCluster* cluster : clusters) {
        allMedoids.push_back(cluster->GetMedoidIndex());
    }
    for(size_t i = 0 ; i < xValues->size() ; ++i) {
        float minDistance = 0.F;
        if(find(allMedoids, i) == -1) {
            RIVCluster* closestCluster = NULL;
            Point3D p = Point3D(xValues->at(i),yValues->at(i),zValues->at(i));
            for(RIVCluster *cluster : clusters) {
                double distance = cluster->DistanceTo(p);
                if(closestCluster == NULL || distance < minDistance) {
                    minDistance = distance;
                    closestCluster = cluster;
                }
            }
            closestCluster->AddMember(i);
        }
    }
    membersAssigned = true;
}
void RIVClusterSet::OptimizeClusters() {
    double DEBUG_OldCost = TotalCost();
    cost = 0;
    for(RIVCluster* cluster : clusters) {
        cost += cluster->Optimize();
    }
    printf("ClusterSet Cost %f > %f\n",DEBUG_OldCost,cost);
}
double RIVClusterSet::TotalCost() {
    if(!initialized) {
        throw std::string("Not yet initialized.");
    }
    if(!membersAssigned) {
        throw std::string("Members not yet assigned.");
    }
    cost = 0.F;
    for(RIVCluster *cluster : clusters) {
        double clusterCost = cluster->Cost();
        cost += clusterCost;
    }
    return cost;
}

//RIVClusterSet::Refine(RIVClusterSet* clusterSet) {
//    
//}

RIVClusterSet RIVClusterSet::MakeCluster(const size_t& maxRepeat, const size_t& K, std::vector<float>* xValues, std::vector<float>* yValues, std::vector<float>* zValues) {
    if(xValues->size() < K) {
        throw "More clusters requested than values given.";
    }
    
    unsigned long long nrOfCombinations = choose(xValues->size(), K);
    
    size_t repeat = std::min(maxRepeat,(size_t)nrOfCombinations);
    
    std::vector<std::vector<size_t> > initializationIndices;
    bool useCombinations = nrOfCombinations < MAX_COMBINATIONS;
    
    //Generate combinations of starting indices
    if(useCombinations) {
        initializationIndices = generateCombinations(xValues->size(), K, repeat);
    }
    //Only use combinations if not excessive
    
    
    //        printf("Repeating optimization %zu times.\n",initializationIndices.size());
//    std::string taskName = "Cluster K=" + std::to_string(K) + " repeat=" + std::to_string(repeat);
//    reporter::startTask(taskName,repeat);
//    printf("start %s [",taskName.c_str());
//    size_t updateRounds = ceil(repeat / 100.0);
    //        reporter::startTask(taskName,repeat);
    
    RIVClusterSet bestClusterSet;
    double bestCost = std::numeric_limits<double>::max();
    
    for(size_t i = 0 ; i < repeat ; i++) { //Try each combination
        RIVClusterSet clusterSet = RIVClusterSet(K,xValues,yValues,zValues);
        
        if(useCombinations) {
            clusterSet.Initialize(initializationIndices[i]);
        }
        else { //Just use random chance (with replacement)
            clusterSet.Initialize();
        }
        
        //Assign the other values to closest cluster
        clusterSet.AssignMembers();

        //Optimize clusters
        clusterSet.OptimizeClusters();

        if(clusterSet.cost < bestCost) {
            //                bestClusters = clusters;
            bestCost = clusterSet.cost;
            bestClusterSet = clusterSet;
//            printf("New best clustering cost = %f\n",clusterSet.cost);
//            std::cout << bestClusterSet;
        }
//        reporter::update(taskName,1.F);
    }
//    printf(" DONE]\n");
    printf("Medoids : ");
    printVector(bestClusterSet.GetMedoidIndices());
    //        clusters = bestClusters;
//    reporter::stop(taskName);
//    printf("Best clustering: ");
//    for(RIVCluster *cluster : bestClusterSet.GetClusters()) {
//        //                    printf("%d ",cluster->GetMedoidIndex());
//        std::cout << "\t#" << cluster->id << " size = " << cluster->Size();
//    }
//    std::cout << bestClusterSet;
//    printf("\n");
    return bestClusterSet;
}
std::pair<size_t,size_t>* RIVClusterSet::MinMaxClusterSize() {
    if(!minMax && !interpolator) {
        size_t minSize = std::numeric_limits<size_t>::max();
        size_t maxSize = std::numeric_limits<size_t>::min();
        for(RIVCluster* cluster : clusters) {
            size_t clusterSize = cluster->Size();
            if(clusterSize < minSize) {
                minSize = clusterSize;
            }
            if(clusterSize > maxSize) {
                maxSize = clusterSize;
            }
        }
        minMax = new std::pair<size_t,size_t>(minSize,maxSize);
        interpolator = new LinearInterpolator<size_t>(minMax->first,minMax->second);
    }
    return minMax;
}
float RIVClusterSet::RelativeSizeOf(RIVCluster* cluster) {
    MinMaxClusterSize();
    size_t thisClusterSize = cluster->Size();
    return interpolator->Evaluate(thisClusterSize);
}
size_t RIVClusterSet::MaxSize() {
    MinMaxClusterSize();
    return minMax->second;
}
size_t RIVClusterSet::Size() const{
    return clusters.size();
}





