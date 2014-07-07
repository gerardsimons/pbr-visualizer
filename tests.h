//
//  tests.h
//  Afstuderen
//
//  Created by Gerard Simons on 20/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef Afstuderen_tests_h
#define Afstuderen_tests_h

#include "Table.h"

//Unofficial testing
bool tests(int argc, char** argv) {
    //    std::vector<size_t> pool;
    //    //Generate pool
    //    for(size_t i = 2 ; i < 12 ; i++) {
    //        pool.push_back(i);
    //    }
    //    //Testing nonreplacementsampler
    //    NonReplacementSampler<size_t> sampler = NonReplacementSampler<size_t>(&pool);
    //    for(int i = 0 ; i < 10 ; i++) {
    //        printf("Sample #%d = %zu\n",i,sampler.RequestSample());
    //    }
    
    //    if(dataset.IsSet()) {
    //        RIVTable *intersectTable = dataset.GetTable("intersections");
    ////        intersectTable->ClusterWithSize("intersection X","intersection Y","intersection Z",1000);
    //
    //    }
    
    //    std::vector<size_t> testValues;
    //    testValues.push_back(0);
    //    testValues.push_back(5);
    //    testValues.push_back(7);
    //    testValues.push_back(15);
    //
    //    DiscreteInterpolator<size_t> interpolator = DiscreteInterpolator<size_t>(testValues);
    //    for(size_t i = 0 ; i < 20 ; i++) {
    //        printf("%zu = %f\n",i,interpolator.Interpolate(i));
    //    }
    
    //    ::testing::InitGoogleTest(&argc, argv);
    //    return RUN_ALL_TESTS();
    
    std::vector<size_t> values;
    values.push_back(0);
    values.push_back(4);
    values.push_back(6);
    Evaluator<size_t, float> *multipleInterpolator = new LinearInterpolator<size_t>(values);
    printf("Interpolate(0) = %f\n",multipleInterpolator->Evaluate(0));
    printf("Interpolate(2) = %f\n",multipleInterpolator->Evaluate(4));
    printf("Interpolate(2.5) = %f\n",multipleInterpolator->Evaluate(5));
    printf("Interpolate(3) = %f\n",multipleInterpolator->Evaluate(6));
    
    
    RIVTable tableOne = RIVTable("table_1");
    RIVTable tableTwo = RIVTable("table_2");
    RIVTable tableThree = RIVTable("table_3");
    
    RIVFloatRecord recordOne = RIVFloatRecord("record_1");
    RIVFloatRecord recordTwo = RIVFloatRecord("record_2");
    RIVFloatRecord recordThree = RIVFloatRecord("record_3");
    
    std::vector<float> valuesOne;
    valuesOne.push_back(1.F); // ---> referes to row 1 of table two
    valuesOne.push_back(2.F);
    valuesOne.push_back(3.F);
    
    std::vector<float> valuesTwo;
    valuesTwo.push_back(11.F);
    valuesTwo.push_back(12.F); // ---> refers to row 0 and 2 or table three
    
    std::vector<float> valuesThree;
    valuesThree.push_back(111.F);
    valuesThree.push_back(112.F);
    valuesThree.push_back(113.F);
    valuesThree.push_back(114.F);
    valuesThree.push_back(115.F);
    
    recordOne.SetValues(valuesOne);
    recordTwo.SetValues(valuesTwo);
    recordThree.SetValues(valuesThree);
    
    tableOne.AddRecord(&recordOne);
    tableTwo.AddRecord(&recordTwo);
    tableThree.AddRecord(&recordThree);
    
    // Reference from table one to table two
    RIVReference reference = RIVReference(&tableOne, &tableTwo);
    std::map<size_t,std::vector<size_t>> indexReferences;
    std::vector<size_t> targetIndices;
    targetIndices.push_back(1);
    indexReferences[0] = targetIndices;
    reference.SetReferences(indexReferences);
    tableOne.AddReference(reference);
    tableTwo.AddReference(reference.ReverseReference());
    
    //Reference from table two to table three
    RIVReference referenceTwo = RIVReference(&tableTwo, &tableThree);
    std::map<size_t,std::vector<size_t>> indexTwoReferences;
    std::vector<size_t> targetTwoIndices;
    targetTwoIndices.push_back(3);
    targetTwoIndices.push_back(4);
    indexTwoReferences[1] = targetTwoIndices;
    referenceTwo.SetReferences(indexTwoReferences);
    tableTwo.AddReference(referenceTwo);
    tableThree.AddReference(referenceTwo.ReverseReference());
    
    RIVReferenceChain chain;
    tableTwo.GetReferenceChainToTable(tableThree.GetName(), chain);
    
    std::cout << "table_one row 0 is linked to table_three rows : [";
    std::vector<size_t> resolvedRows = chain.ResolveRow(0);
    for(size_t i : chain.ResolveRow(0)) {
        std::cout << i;
    }
}


#endif
