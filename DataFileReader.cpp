#include "DataFileReader.h"

#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>


DataFileReader::DataFileReader(void)
{
    
}


DataFileReader::~DataFileReader(void)
{
    
}

RIVDataSet DataFileReader::ReadAsciiData(std::string fileName) {
    std::ifstream is;
    is.open (fileName, std::ios::binary | std::ios::in );
    
    is.seekg (0, std::ios::beg);
    
    std::vector<unsigned short> xPixelData;
    std::vector<unsigned short> yPixelData;
    std::vector<float> throughPutOne;
    std::vector<float> throughPutTwo;
    std::vector<float> throughPutThree;
    std::vector<unsigned short> intersections;
    
    unsigned short x;
    unsigned short y;
    float throughput[3] = {-1.F,-1.F,-1.F};
    unsigned short size;
    
    std::string line;
    int skipLines = 13;
    int lineNumber = 0;
    
    if (is.is_open())
    {
        while ( getline (is,line) )
        {
            if(lineNumber > skipLines) {
                int commaCount = 0;
                
                std::string buffer;
                for(size_t i = 0 ; i < line.size() ; i++) {
                    char c = line[i];
                    if(c == ',') {
                        ++commaCount;
//                        printf("buffer = %s\n",buffer.c_str());
                        switch(commaCount) {
                            case 1:
                                x = (unsigned short) std::stoi(buffer,nullptr,0);
                            case 2:
                                y = (unsigned short) std::stoi(buffer,nullptr,0);
                            case 3:
                                throughput[0] = std::stof(buffer,nullptr);
                            case 4:
                                throughput[2] = std::stof(buffer,nullptr);
                            case 5:
                                throughput[1] = std::stof(buffer,nullptr);
                            case 6:
                                y = (unsigned short) std::stoi(buffer,nullptr,0);
                        }
                        buffer.clear();
                    }
                    else if(c == '[' || c == ']') { //Ignore, they exist just for readability
                        continue;
                    }
                    else {
                        buffer.push_back(c);
                    }
                }
                xPixelData.push_back(x);
                yPixelData.push_back(y);
                
                throughPutOne.push_back(throughput[0]);
                throughPutTwo.push_back(throughput[1]);
                throughPutThree.push_back(throughput[2]);
                intersections.push_back(size);
                
                
            }
            ++lineNumber;
        }
        is.close();
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
    is.close();
    
    return dataset;
}

RIVDataSet DataFileReader::ReadBinaryData(std::string fileName) {
    printf("Trying to load %s\n",fileName.c_str());
    
    FILE *inputFile = fopen(fileName.c_str(),"rb");
    //write what was read as ASCII
    FILE *outputFile = fopen("output.txt","w");
    
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
    
    fprintf(outputFile, "x,y,throughput_1,throughput_2,throughput_3,size\n");
    
    while(!feof(inputFile)) {
        
        printf("reading line %d\n",lineNumber);
        
        //Read x
        unsigned short x;
        unsigned short y;
        float throughput[3] = {-1.F,-1.F,-1.F};
        unsigned short size;
        
        
        fread(&x,sizeof(unsigned short),1,inputFile);
        
        char buffer[30];
        sprintf(buffer, "%hu,",x);

        fprintf(outputFile, buffer);
        
        //Read y
        
        fread(&y,sizeof(unsigned short),1,inputFile);
        sprintf(buffer, "%hu,",y);
        fprintf(outputFile, buffer);
        

        

        //fread(throughput,sizeof(float),3,inputFile);
        //sprintf(buffer, "%f,%f,%f,",throughput[0],throughput[1],throughput[2]);
        //fprintf(outputFile, buffer);
        

        
        //Read the number of intersections

        fread(&size,sizeof(unsigned short),1,inputFile);
        sprintf(buffer, "%hu\n",size);
        fprintf(outputFile, buffer);

        
        xPixelData.push_back(x);
        yPixelData.push_back(y);
        
        throughPutOne.push_back(throughput[0]);
        throughPutTwo.push_back(throughput[1]);
        throughPutThree.push_back(throughput[2]);
        intersections.push_back(size);
        
        //printf("%d = %hu,%hu,[%f,%f,%f],%hu\n",lineNumber,x,y,throughput[0],throughput[1],throughput[2],size);
        ++lineNumber;
        if(lineNumber > 1000000) {
            break;
        }
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
    
    fclose(outputFile);
    fclose(inputFile);
    
    return dataset;
};

