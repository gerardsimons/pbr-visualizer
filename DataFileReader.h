
#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <string>
#include "DataSet.h"

class DataFileReader
{
private:
	DataFileReader();
public:
	~DataFileReader(void);
    

	static RIVDataSet LoadData(std::string fileName) {
        printf("Trying to load %s\n",fileName.c_str());
        FILE *inputFile = fopen(fileName.c_str(),"rb");
        if(inputFile == 0) {
            throw "Error opening file.";
        }
        
        std::vector<unsigned short> xPixelData;
        std::vector<unsigned short> yPixelData;
        std::vector<float> throughPutOne;
        std::vector<float> throughPutTwo;
        std::vector<float> throughPutThree;
        std::vector<unsigned short> intersections;
        
        int lineNumber = 1;
        
        while(!feof(inputFile)) {
            
            //Read x
            unsigned short x;
            fread(&x,sizeof(unsigned short),1,inputFile);

            xPixelData.push_back(x);
            
            //Read y
            unsigned short y;
            fread(&y,sizeof(unsigned short),1,inputFile);

            yPixelData.push_back(y);
            
            float throughput[3];
            fread(throughput,sizeof(float),3,inputFile);


            throughPutOne.push_back(throughput[0]);

            throughPutTwo.push_back(throughput[1]);

            throughPutThree.push_back(throughput[2]);
            
            //Read the number of intersections
            unsigned short size;
            fread(&size,sizeof(unsigned short),1,inputFile);

            intersections.push_back(size);
            
            //printf("%d = %hu,%hu,[%f,%f,%f],%hu\n",lineNumber,x,y,throughput[0],throughput[1],throughput[2],size);
            ++lineNumber;
        }
        RIVDataSet dataset;
        RIVRecord<unsigned short> xRecord("x");
        xRecord.SetValues(xPixelData);
        RIVRecord<unsigned short> yRecord("y");
        yRecord.SetValues(yPixelData);
        RIVRecord<float> tpOne("throughput 1");
        tpOne.SetValues(throughPutOne);
        RIVRecord<float> tpTwo("throughput 2");
        tpTwo.SetValues(throughPutTwo);
        RIVRecord<float> tpThree("throughput 3");
        tpThree.SetValues(throughPutThree);
        RIVRecord<unsigned short> isects("intersections");
        isects.SetValues(intersections);
        dataset.AddRecord(xRecord);
        dataset.AddRecord(yRecord);
        dataset.AddRecord(tpOne);
        dataset.AddRecord(tpTwo);
        dataset.AddRecord(tpThree);
        dataset.AddRecord(isects);
        
        return dataset;
    }
};

#endif

