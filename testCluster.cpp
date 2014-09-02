//
//  testCluster.h
//  Afstuderen
//
//  Created by Gerard Simons on 06/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef TEST_CLUSTER
#define TEST_CLUSTER

#include "ClusterSet.h"
#include "Cluster.h"
//#include <stdio.h>
//#include <iostream>
//#include <math.h>

namespace {
    
    // The fixture for testing class Foo.
    class ClusterTest : public ::testing::Test {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.
        RIVCluster* cluster;
        RIVClusterSet* clusterSet;
        
        
        const float radius = 5.F;
        const int nrOfPoints = 10;
        
        std::vector<float> xValues;
        std::vector<float> yValues;
        std::vector<float> zValues;
        
        ClusterTest() {
            // You can do set-up work for each test here.
        }
        
        virtual ~ClusterTest() {
            // You can do clean-up work that doesn't throw exceptions here.
        }
        
        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:
        
        virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).
            
            //Center of circle
            float cX = 0;
            float cY = 0;
            float cZ = 0;
            
            const double radiansDelta = 2 * M_PI / nrOfPoints;
            double radian = 0.F;
            
            //Add the center of the circle to the values
            xValues.push_back(cX);
            yValues.push_back(cY);
            zValues.push_back(cZ);
            
            cluster = new RIVCluster(0, &xValues, &yValues, &zValues, 0);
            
            //On the z plane
            for(int i = 0 ; i < nrOfPoints ; ++i) {
                float x = cX + cos(radian) * radius;
                float y = cY + sin(radian) * radius;
                float z = 0;
                
                radian += radiansDelta;
                
                //Add the point on the circle to the values
                xValues.push_back(x);
                yValues.push_back(y);
                zValues.push_back(z);
                
                cluster->AddMember(i + 1); //Offset by 1 because medoid is the first
                
//                printf("Added point #%d\n",i);
            }

            std::cout << *cluster;
        }
        
        virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
            delete cluster;
        }
        
        // Objects declared here can be used by all tests in the test case for Foo.
    };
    
    TEST_F(ClusterTest,Initialization) {
        EXPECT_EQ(nrOfPoints,cluster->MembersSize());
        EXPECT_EQ(nrOfPoints + 1,cluster->Size());
    }
    
    TEST_F(ClusterTest,SwapMedoid) {
        const size_t medoidIndex = cluster->GetMedoidIndex();
        const size_t mIndex = cluster->GetMemberIndex(0);
        cluster->SwapMedoid(0);
        EXPECT_EQ(mIndex,cluster->GetMedoidIndex());
        EXPECT_EQ(medoidIndex,cluster->GetMemberIndex(0));
        cluster->SwapBack();
        EXPECT_EQ(medoidIndex,cluster->GetMedoidIndex());
        EXPECT_EQ(mIndex,cluster->GetMemberIndex(0));
    }
    
    TEST_F(ClusterTest,Cost) {
        float cost = cluster->Cost();
        EXPECT_EQ(cost,nrOfPoints * radius);
    }
    
    TEST_F(ClusterTest,Optimize) {
        cluster->Optimize();
        EXPECT_EQ(cluster->GetMedoidIndex(),0);
        
        for(size_t i = 1 ; i < cluster->MembersSize() ; ++i) {
            cluster->SwapMedoid(i);
            cluster->Optimize();
            EXPECT_EQ(cluster->GetMedoidIndex(),0);
        }
    }
    
    // Tests that the Foo::Bar() method does Abc.
//    TEST_F(FooTest, MethodBarDoesAbc) {
//        const std::string input_filepath = "this/package/testdata/myinputfile.dat";
//        const std::string output_filepath = "this/package/testdata/myoutputfile.dat";
////        Foo f;
//        EXPECT_EQ(0, 0);
//    }
//    
//    // Tests that Foo does Xyz.
//    TEST_F(FooTest, DoesXyz) {
//        // Exercises the Xyz feature of Foo.
//    }
    
}  // namespace

//int main(int argc, char **argv) {
//    printf("Google main test");
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}

#endif
