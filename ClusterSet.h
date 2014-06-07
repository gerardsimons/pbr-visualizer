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
//#include "reporter.h"

class RIVClusterSet {
private:
    std::vector<RIVCluster*> clusters;
    std::pair<size_t,size_t>* minMax = NULL;
    LinearInterpolator<size_t>* interpolator; //Used for computing relative sizes
    
    std::vector<float>* xValues;
    std::vector<float>* yValues;
    std::vector<float>* zValues;
    
    const int maxCombinations = 10000;
    
    NonReplacementSampler<size_t> sampler;
    
    bool initialized = false;
    bool membersAssigned = false;
    size_t K = 0;
public:
    ~RIVClusterSet() {
        if(minMax)
            delete minMax;
        //Delete all the pointers and the vector itself
        
        for(auto &it:clusters) delete it; clusters.clear();
        if(interpolator)
            delete interpolator;
    }
    RIVClusterSet() {
        //Do nothing
        minMax = NULL;
        interpolator = NULL;
    }
    RIVClusterSet(std::vector<RIVCluster*>& clusters_) {
        clusters = clusters_;
        K = clusters.size();
    }
    RIVClusterSet(const size_t& K_, std::vector<float>* xValues_, std::vector<float>* yValues_, std::vector<float>* zValues_) {
        K = K_;
        xValues = xValues_;
        yValues = yValues_;
        zValues = zValues_;
    }
    void AddCluster(RIVCluster* cluster) {
        clusters.push_back(cluster);
    }
    RIVCluster* GetCluster(size_t index) {
        if(index < clusters.size()) {
            return clusters[index];
        }
        throw "Warning: requested cluster out of cluster set bounds.";
        return NULL;
    }
    std::vector<RIVCluster*> GetClusters() {
        return clusters;
    }
    void Initialize() {
        sampler = NonReplacementSampler<size_t>(0, xValues->size());
        for(auto &it:clusters) delete it; clusters.clear();
        for(size_t i = 0 ; i < K ; ++i) {
            size_t index = sampler.RequestSample();
            RIVCluster* cluster = new RIVCluster(index, xValues, yValues, zValues, i);
            clusters.push_back(cluster);
        }
        initialized = true;
    }
    void Initialize(std::vector<size_t> indices) {
        for(auto &it:clusters) delete it; clusters.clear();
        if(indices.size() != K) {
            throw "Invalid number of init indices supplied. Must match K";
        }
        for(size_t& i : indices) {
            RIVCluster* cluster = new RIVCluster(i,xValues,yValues,zValues,i);
            clusters.push_back(cluster);
        }
        initialized = true;
    }
    void AssignMembers() {
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
                bool isMedoid = false;
                for(RIVCluster *&cluster : clusters) {
                    float distance = cluster->DistanceTo(p);
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
    void OptimizeClusters() {
        for(RIVCluster* cluster : clusters) {
            cluster->Optimize();
        }
    }
    double TotalCost() {
        if(!initialized) {
            throw std::string("Not yet initialized.");
        }
        if(!membersAssigned) {
            throw std::string("Members not yet assigned.");
        }
        double cost = 0.F;
        for(RIVCluster *cluster : clusters) {
            cost += cluster->Cost();
        }
        return cost;
    }
    void Cluster(const size_t& maxRepeat) {
        unsigned long long nrOfCombinations = choose(xValues->size(), K);

        size_t repeat = std::min(maxRepeat,(size_t)nrOfCombinations);
        
        std::vector<std::vector<size_t>> initializationIndices;
        bool useCombinations = nrOfCombinations < maxCombinations;
        
        //Generate combinations of starting indices
        if(useCombinations) {
            initializationIndices = generateCombinations(xValues->size(), K, repeat);
        }
        //Only use combinations if not excessive

        
//        for(std::vector<size_t> c : initializationIndices)
//            printVector(c);
        
//        std::vector<RIVCluster*> bestClusters;
        
        double bestCost = std::numeric_limits<double>::max();
        
//        printf("Repeating optimization %zu times.\n",initializationIndices.size());
        std::string taskName = "Cluster K=" + std::to_string(K) + " repoeat=" + std::to_string(repeat);
        printf("start %s",taskName.c_str());
//        reporter::startTask(taskName,repeat);
        
        for(size_t i = 0 ; i < repeat ; i++) { //Try each combination
//            printf("Initialization indices = ");
//            printVector(indices);
//            printf("\nPoints = ");
//            for(size_t i : indices) {
//                std::cout << Point3D(xValues->at(i),yValues->at(i),zValues->at(i));
                
//            }
            //Reinitialize
            
            if(useCombinations) {
                Initialize(initializationIndices[i]);
            }
            else { //Just use random chance (with replacement)
                Initialize();
            }
            
            //Assign the other values to closest cluster
            AssignMembers();
            
            //Optimize clusters
            OptimizeClusters();
            
            //What is the cost?
            double cost = TotalCost();
            
            if(cost < bestCost) {
//                bestClusters = clusters;
                bestCost = cost;
//                printf("New best clustering = ");
//                for(RIVCluster *cluster : clusters) {
                    //                    printf("%d ",cluster->GetMedoidIndex());
//                    std::cout << *cluster;
//                }
//                printf("\n");
            }
//            reporter::update(taskName,1.F);
        }
//        clusters = bestClusters;
//            reporter::stop(taskName);
        printf("Best clustering = ");
        for(RIVCluster *cluster : clusters) {
            //                    printf("%d ",cluster->GetMedoidIndex());
            std::cout << *cluster;
        }
        printf("\n");
    }
    std::pair<size_t,size_t>* MinMaxClusterSize() {
        if(!minMax) {
            size_t minSize = std::numeric_limits<size_t>::max();
            size_t maxSize = std::numeric_limits<size_t>::min();
            for(RIVCluster* cluster : clusters) {
                size_t clusterSize = cluster->MembersSize();
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
    float RelativeSizeOf(RIVCluster* cluster) {
        MinMaxClusterSize();
        size_t thisClusterSize = cluster->Size();
        return interpolator->Interpolate(thisClusterSize);
    }
    size_t MaxSize() {
        MinMaxClusterSize();
        return minMax->second;
    }
    size_t Size() const{
        return clusters.size();
    }
    friend std::ostream& operator<<(std::ostream& os, const RIVClusterSet& c)
    {
        os << "ClusterSet with " << c.Size() << " members";
//        os << " clusters : ";
//        for(RIVCluster* cluster : c.clusters) {
//            os << (*cluster << std::endl;
//        }
        return os;
    }
};


#endif /* defined(__Afstuderen__ClusterSet__) */
