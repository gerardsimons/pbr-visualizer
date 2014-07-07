//
//  testClusterSet.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 07/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef TEST_CLUSTERSET
#define TEST_CLUSTERSET

#include <stdio.h>

#include "gtest.h"
#include "Cluster.h"
#include "ClusterSet.h"
#include "RIVData/DataFileReader.h"

namespace {
    class ClusterSetTest : public ::testing::Test {
    public:
        RIVClusterSet* clusterSet;
        std::vector<float> xValuesSeeds;
        std::vector<float> yValuesSeeds;
        std::vector<float> zValuesSeeds;
        const int K = 4;
        const float distanceSeed = 2.F;
        const int pointsPerSeed = 9;
        
        virtual void SetUp() {
//            std::vector<Point3D> seedPoints(4);
//            seedPoints[0] = Point3D(0,0,0);
//            seedPoints[1] = Point3D(10,0,0);
//            seedPoints[2] = Point3D(0,10,0);
//            seedPoints[3] = Point3D(0,0,10);
//            //            seedPoints[0] = Point3D(0,0,0);
//            std::vector<size_t> seedIndices;
//            
//            for(size_t mI = 0 ; mI < K ; mI++) {
//                xValuesSeeds.push_back(seedPoints[mI].x);
//                yValuesSeeds.push_back(seedPoints[mI].y);
//                zValuesSeeds.push_back(seedPoints[mI].z);
//                
//                seedIndices.push_back(mI);
//            }
//            
//            float radiansDelta = 2 * M_PI / pointsPerSeed;
//            float radians = 0;
//            
//            for(int i = 0 ; i < K ; i++) {
//
//                for(int j = 0 ; j < pointsPerSeed ; ++j ) {
//                    Point3D& seedPoint = seedPoints[i];
//
//                    //random jiggle
////                    float x = (rand() / RAND_MAX) * 2 * distanceSeed - distanceSeed + seedPoint.x;
////                    float y = (rand() / RAND_MAX) * 2 * distanceSeed - distanceSeed + seedPoint.y;
////                    float z = (rand() / RAND_MAX) * 2 * distanceSeed - distanceSeed + seedPoint.z;
//                    
//                    
//                    
//                    //Fixed distance
////                    float mod = (j % 4) * M_PI / 2.F;
//                    
////                    float x = distanceSeed * cos(mod) + seedPoint.x;
////                    float y = distanceSeed * sin(mod) + seedPoint.y;
//                    float x = distanceSeed * cos(radians) + seedPoint.x;
//                    float y = distanceSeed * sin(radians) + seedPoint.y;
//                    float z = seedPoint.z;
//                    
//                    radians += radiansDelta;
//                    
////                    float y = distanceSeed + seedPoint.y;
////                    float z = distanceSeed + seedPoint.z;
//                    
//                    xValuesSeeds.push_back(x);
//                    yValuesSeeds.push_back(y);
//                    zValuesSeeds.push_back(z);
//                }
//            }
//            printf("%zu test values generated.\n",xValuesSeeds.size());
//            
//            printf("Points generated : \n");
//            for(size_t i = 0 ; i < xValuesSeeds.size() ; ++i) {
//                Point3D point(xValuesSeeds[i],yValuesSeeds[i],zValuesSeeds[i]);
//                std::cout << point;
//            }
//            
//            clusterSet = new RIVClusterSet(K,&xValuesSeeds,&yValuesSeeds,&zValuesSeeds);
//            clusterSet->Initialize(seedIndices);
        }
    };
    
//    void ReadFromFile(std::string fileName) {
//        DataFileReader::ReadAsciiData(fileName, <#const BMPImage &image#>)
//    }
    
    TEST_F(ClusterSetTest,Initialization) {
        EXPECT_EQ((pointsPerSeed + 1) * K,xValuesSeeds.size());
        EXPECT_EQ((pointsPerSeed + 1) * K,yValuesSeeds.size());
        EXPECT_EQ((pointsPerSeed + 1) * K,zValuesSeeds.size());
        
        //Test there are no doubles
        std::map<size_t,bool> indexFound;
        
        //Assign members
        clusterSet->AssignMembers();
//        std::cout << *clusterSet;
        
        for(RIVCluster* cluster : clusterSet->GetClusters()) {
            for(size_t index : cluster->GetMemberIndices()) {
//                printf("index = %zu\n",index);
                bool found = indexFound[index];
                EXPECT_EQ(false,found);
                indexFound[index] = true;
            }
            size_t medoid = cluster->GetMedoidIndex();
//            printf("medoidIndex = %zu\n",medoid);
            EXPECT_EQ(false,indexFound[medoid]);
            indexFound[medoid] = true;
        }
        
        //Are all values accounted for?
        for(size_t i = 0 ; i < xValuesSeeds.size() ; ++i) {
            bool found = indexFound[i];
//            printf("indexfound[%zu]=%d\n",i,found);
            ASSERT_EQ(found,true);
        }
    }

    
    TEST_F(ClusterSetTest,Cluster) {
        RIVClusterSet makeClusterSet = RIVClusterSet::MakeCluster(1000, K, &xValuesSeeds, &yValuesSeeds, &zValuesSeeds);
        EXPECT_EQ(pointsPerSeed * distanceSeed * K,makeClusterSet.TotalCost());
    }
}

#endif