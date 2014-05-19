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

std::vector<std::string> explode(std::string line, char delimiter, std::string ignoreChars) {
	std::vector<std::string> exploded;
	std::string buffer;
	for(char c : line) {
		if(c == ',') {
			exploded.push_back(buffer);
			buffer.clear();
		}
		else {
            bool ignore = false;
            for(char ignoreChar : ignoreChars) {
                if(ignoreChar == c) {
                    ignore = true;
                    break;
                }
            }
			if(!ignore) buffer.push_back(c);
		}
	}
    exploded.push_back(buffer);
    
//    printf("%s became = ",line.c_str());
//    for(std::string str : exploded) {
//        printf("%s ",str.c_str());
//    }
//    printf("\n");
    
	return exploded;
}

RIVDataSet DataFileReader::ReadAsciiData(std::string fileName) {
    std::ifstream is;
    is.open (fileName, std::ios::in );
    
    is.seekg (0, std::ios::beg);
    
    
    std::vector<unsigned short> xPixelData;
    std::vector<unsigned short> yPixelData;
    std::vector<float> throughPutOne;  //We only use the first one for now, as they all seem the same every time
    std::vector<float> throughPutTwo;
    std::vector<float> throughPutThree;
    
    std::vector<unsigned short> nrOfIntersections;
    
    std::vector<float> intersectionPosX;
    std::vector<float> intersectionPosY;
    std::vector<float> intersectionPosZ;
    std::vector<unsigned short> primitveIds;
    std::vector<unsigned short> shapeIds;
    std::vector<float> spectraOne;
    std::vector<float> spectraTwo;
    std::vector<float> spectraThree;
    
    
    unsigned short x;
    unsigned short y;
    float throughput[3] = {-1.F,-1.F,-1.F};
    unsigned short intersections_size;
    
    float intersectPosX;
    float intersectPosY;
    float intersectPosZ;
    
    unsigned short primitiveId;
    unsigned short shapeId;
    float spectrumOne;
    float spectrumTwo;
    float spectrumThree;
    
    std::string ignoreList = "[]{}";
    
    std::string line;

    int lineNumber = 0;
    
    if (is.is_open())
    {

        std::string buffer;
        while (getline(is,line)) {
            ++lineNumber;
            if(!line.empty() && line[0] != '%') {
                std::vector<std::string> exploded = explode(line,',',ignoreList);
                
                x = std::stoul(exploded[0],nullptr,0);
                y = std::stoul(exploded[1],nullptr,0);
                throughput[0] = std::stof(exploded[2],0);
                throughput[1] = std::stof(exploded[3],0);
                throughput[2] = std::stof(exploded[4],0);
                intersections_size = std::stoul(exploded[5],nullptr,0);
                
                //Do this at least once, even if there are no intersections!

                xPixelData.push_back(x);
                yPixelData.push_back(y);
                throughPutOne.push_back(throughput[0]);
                throughPutTwo.push_back(throughput[1]);
                throughPutThree.push_back(throughput[2]);
                nrOfIntersections.push_back(intersections_size);
                
                for(size_t i = 0 ; i < intersections_size ; i++) {
//                    xPixelData.push_back(x);
//                    yPixelData.push_back(y);
//                    throughPutOne.push_back(throughput[0]);
//                    throughPutTwo.push_back(throughput[1]);
//                    throughPutThree.push_back(throughput[2]);
//                    nrOfIntersections.push_back(intersections_size);
                    
                    intersectPosX = std::stof(exploded[6 + i * 3],0);
                    intersectPosY = std::stof(exploded[7 + i * 3],0);
                    intersectPosZ = std::stof(exploded[8 + i * 3],0);
                    primitiveId = std::stoul(exploded[9+ i]);
                    shapeId = std::stoul(exploded[9+ i]);
                    spectrumOne = std::stof(exploded[10 + i * 3],0);
                    spectrumTwo = std::stof(exploded[11 + i * 3],0);
                    spectrumThree = std::stof(exploded[12 + i * 3],0);
                    
                    intersectionPosX.push_back(intersectPosX);
                    intersectionPosY.push_back(intersectPosY);
                    intersectionPosZ.push_back(intersectPosZ);
                }
            }
        }
        is.close();
    }

    RIVDataSet dataset;
    RIVTable pathTable("path");
    RIVTable intersections("intersections");
    
    //Record definitions
    RIVUnsignedShortRecord *xRecord = new RIVUnsignedShortRecord("x");
    xRecord->SetValues(xPixelData);
    RIVUnsignedShortRecord *yRecord = new RIVUnsignedShortRecord("y");
    yRecord->SetValues(yPixelData);
    RIVFloatRecord *tpOne = new RIVFloatRecord("throughput 1");
    tpOne->SetValues(throughPutOne);
    RIVFloatRecord *tpTwo = new RIVFloatRecord("throughput 2");
    tpTwo->SetValues(throughPutTwo);
    RIVFloatRecord *tpThree = new RIVFloatRecord("throughput 3");
    tpThree->SetValues(throughPutThree);
    RIVUnsignedShortRecord *isects = new RIVUnsignedShortRecord("#intersections");
    isects->SetValues(nrOfIntersections);
    RIVFloatRecord *isectPosX = new RIVFloatRecord("intersection X");
    isectPosX->SetValues(intersectionPosX);
    RIVFloatRecord *isectPosY = new RIVFloatRecord("intersection Y");
    isectPosY->SetValues(intersectionPosY);
    RIVFloatRecord *isectPosZ = new RIVFloatRecord("intersection Z");
    isectPosZ->SetValues(intersectionPosZ);
    
    pathTable.AddRecord(xRecord);
    pathTable.AddRecord(yRecord);
    pathTable.AddRecord(tpOne);
//    dataset.AddRecord(tpTwo);
//    dataset.AddRecord(tpThree);
    pathTable.AddRecord(isects);
//    intersections.AddRecord(isectPosX);
//    intersections.AddRecord(isectPosY);
//    intersections.AddRecord(isectPosZ);
//    
    dataset.AddTable(pathTable);
//    dataset.AddTable(intersections);
    

//    Filter *yFilter = new RangeFilter("x",8,9);
//    dataset.AddFilter(yFilter);
    
    
    
    printf("*******************   DATASET READ   *******************\n");
    dataset.Print();
    printf("****************    END DATASET READ    ****************\n");
    
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

        fread(throughput,sizeof(float),3,inputFile);
        sprintf(buffer, "%f,%f,%f,",throughput[0],throughput[1],throughput[2]);
        fprintf(outputFile, buffer);
        
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
    RIVTable pathTable("path");
    RIVTable intersectionsTable("intersections");
    
    RIVUnsignedShortRecord *xRecord = new RIVUnsignedShortRecord("x");
    xRecord->SetValues(xPixelData);
    RIVUnsignedShortRecord *yRecord = new RIVUnsignedShortRecord("y");
    yRecord->SetValues(yPixelData);
    RIVFloatRecord *tpOne = new RIVFloatRecord("throughput 1");
    tpOne->SetValues(throughPutOne);
    RIVFloatRecord *tpTwo = new RIVFloatRecord("throughput 2");
    tpTwo->SetValues(throughPutTwo);
    RIVFloatRecord *tpThree = new RIVFloatRecord("throughput 3");
    tpThree->SetValues(throughPutThree);
    RIVUnsignedShortRecord *isects = new RIVUnsignedShortRecord("#intersections");
//    isects->SetValues(nrOfIntersections);
//    RIVFloatRecord *isectPosX = new RIVFloatRecord("intersection X");
//    isectPosX->SetValues(intersectionPosX);
//    RIVFloatRecord *isectPosY = new RIVFloatRecord("intersection Y");
//    isectPosY->SetValues(intersectionPosY);
//    RIVFloatRecord *isectPosZ = new RIVFloatRecord("intersection Z");
//    isectPosZ->SetValues(intersectionPosZ);
    
    pathTable.AddRecord(xRecord);
    pathTable.AddRecord(yRecord);
    pathTable.AddRecord(tpOne);
    //    dataset.AddRecord(tpTwo);
    //    dataset.AddRecord(tpThree);
    pathTable.AddRecord(isects);
    
    dataset.AddTable(pathTable);

//    dataset.AddRecord(isectPosX);
//    dataset.AddRecord(isectPosY);
//    dataset.AddRecord(isectPosZ);
    
    fclose(outputFile);
    fclose(inputFile);
    
    return dataset;
};










/*
++lineNumber;
if(lineNumber > skipLines) {
    int elementCount = 0;
    buffer.clear();
    for(size_t i = 0 ; i < line.size() ; i++) {
        char c = line[i];
        if(elementCount < 7) {
            if(c == ',') {
                ++elementCount;
                //                        printf("buffer = %s\n",buffer.c_str());
                switch(elementCount) { //What element are we currently parsing?
                    case 1:
                        x = (unsigned short) std::stoul(buffer,nullptr,0);
                    case 2:
                        y = (unsigned short) std::stoul(buffer,nullptr,0);
                    case 3:
                        throughput[0] = std::stof(buffer,nullptr);
                    case 4:
                        throughput[2] = std::stof(buffer,nullptr);
                    case 5:
                        throughput[1] = std::stof(buffer,nullptr);
                    case 6:
                        intersections_size = (unsigned short) std::stoul(buffer,nullptr,0);
                        
                }
                buffer.clear();
            }
            else if(c == '[' || c == ']') { //Ignore, they exist just for readability
                //                        printf("char %c ignored.\n",c);
                continue;
            }
            else {
                //                        printf("char %c pushed to buffer.\n",c);
                buffer.push_back(c);
            }
        }
        else { //The intersection data
            if(c == '}') {
                ++elementCount;
                switch(elementCount) {
                    case 7: //intersection positions
                        std::string secondBuffer;
                        for(char isectPos_c : buffer) {
                            if(isectPos_c == ',') {
                                
                            }
                            else if(isectPos_c != ']' && isectPos_c != '[') {
                                secondBuffer.push_back(isectPos_c);
                            }
                        }
                    case 8: //primitive ids
                        
                    case 9:
                        
                }
            }
            else if(c == '{') {
                
            }
            else {
                buffer.push_back(c);
            }
            
        }
*/