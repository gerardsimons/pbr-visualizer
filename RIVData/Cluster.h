//
//  Cluster.h
//  Afstuderen
//
//  Created by Gerard Simons on 02/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__Cluster__
#define __Afstuderen__Cluster__

#include "DataSet.h"

class RIVCluster {
private:
    RIVDataSet clusteredDataSet;
public:
    RIVCluster(RIVDataSet& dataset) {
        
    }
    
    void Cluster(unsigned int clusterSize);
};;

#endif /* defined(__Afstuderen__Cluster__) */
