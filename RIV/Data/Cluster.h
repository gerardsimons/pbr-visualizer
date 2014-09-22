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
#include "../helper.h"
#include <algorithm>
#include "../Geometry/Geometry.h"

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
    const size_t maxSample = 100;
public:
    size_t id;
    ~RIVCluster();
    RIVCluster(size_t medoidIndex_, std::vector<float>* xValues_, std::vector<float>* yValues_, std::vector<float>* zValues_, size_t id_);
    RIVCluster(const size_t& medoidIndex);
    void AddMember(const size_t& memberIndex);
    float DistanceTo(const Point3D& point);
    double Cost();
    double LastCost() const;
    void SwapMedoid(size_t memberIndex);
    bool SaveToFileStream(const std::ofstream& output);
    void SwapBack();
    Point3D GetMedoid();
    size_t GetMedoidIndex() const;
    size_t GetMemberIndex(const size_t& memberIndex ) const;
    //Does not include the medoid
    size_t MembersSize() const;
    bool HasMember(const size_t& mIndex);
    size_t Size() const;
    //Does not include the medoid!
    std::vector<size_t> GetMemberIndices();
    double Optimize();
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
