#include "DataFileReader.h"

#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <regex>


DataFileReader::DataFileReader(void)
{
    
}


DataFileReader::~DataFileReader(void)
{
    
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

std::vector<float> DataFileReader::ReadModelData(std::string fileName) {
    std::ifstream is;
    is.open (fileName, std::ios::in );
    
    is.seekg (0, std::ios::beg);
    
    std::vector<float> vertices;
    
    std::string ignoreChars = "\"[]";
    
    std::regex rx("Point");
    
    if (is.is_open())
    {
        bool shapeFound = false;
        bool pointFound = false;
        
        std::string token;
        std::string line;
        while (getline(is,line)) {
            for(char c : line) {
                if(c == ' ') {
                    //Process token
                    if(!shapeFound) {
                        if(token == "Shape") {
                            shapeFound = true;
                        }
                    }
                    else if(!pointFound){ //Look for point declaration
                        if(std::regex_search(token.begin(), token.end(),rx)) {
                            pointFound = true;
                        }
                    }
                    else if(is_number(token)) {
                        //Add to vertex
                        float vertex = std::stof(token);
                        vertices.push_back(vertex);
                    }
                    else {
                        //This line is done
                        break;
                    }
                }
                else {
                    for(char ignoreC : ignoreChars) {
                        if(ignoreC == c) {
                            continue;
                        }
                    }
                    token.push_back(c);
                }
            }
        }
    }
    return vertices;
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
    unsigned short interactionType;
    unsigned short lightId;
    
    std::string ignoreList = "[]{}";
    
    std::string line;

    size_t lineNumber = 0;
    
    size_t pathIndex = 0;
    size_t intersectionIndex = 0;
    
    std::map<size_t,size_t> *references = new std::map<size_t,size_t>();
    
    if (is.is_open())
    {

        std::string buffer;
        while (getline(is,line)) {
            ++lineNumber;
            if(!line.empty() && line[0] != '%') { //Is a valid path declaration
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
                

                    size_t endIntersections = 5 + intersections_size * 3;
                    size_t endPrimitiveIds = endIntersections + intersections_size;
                    size_t endShapeIds = endPrimitiveIds + intersections_size;
                    size_t endSpectrum = endShapeIds + intersections_size * 3;
                    size_t endInteractions = endSpectrum + intersections_size;
                
//                    printf("line %d : \n",lineNumber);
//                    printf("endIntersections = %d endPrimitiveIds = %d endShapeIds = %d\n",endIntersections,endPrimitiveIds,endShapeIds);
                
                if(intersections_size > 0) {
                    for(size_t i = 0 ; i < intersections_size ; i++) {
                        (*references)[pathIndex] = intersectionIndex;
                        
                        intersectPosX = std::stof(exploded[6 + i * 3],0);
                        intersectPosY = std::stof(exploded[7 + i * 3],0);
                        intersectPosZ = std::stof(exploded[8 + i * 3],0);
                        primitiveId = std::stoul(exploded[endIntersections + 1 + i]);
                        shapeId = std::stoul(exploded[endPrimitiveIds + i + 1]);
                        spectrumOne = std::stof(exploded[endShapeIds + i * 3 + 1],0);
                        spectrumTwo = std::stof(exploded[endShapeIds + i * 3 + 2],0);
                        spectrumThree = std::stof(exploded[endShapeIds + i * 3 + 3],0);
                        interactionType = std::stoul(exploded[endSpectrum + 1 + i]);
                        lightId = std::stoul(exploded[endInteractions + 1 + i]);
                        
                        intersectionPosX.push_back(intersectPosX);
                        intersectionPosY.push_back(intersectPosY);
                        intersectionPosZ.push_back(intersectPosZ);
                        primitveIds.push_back(primitiveId);
                        shapeIds.push_back(shapeId);
                        spectraOne.push_back(spectrumOne);
                        spectraTwo.push_back(spectrumTwo);
                        spectraThree.push_back(spectrumThree);
                        
                        ++intersectionIndex;
                    }
                }
                ++pathIndex;
            }
        }
        is.close();
    }

    RIVDataSet dataset;
    RIVTable *pathTable = new RIVTable("path");
    RIVTable *intersectionsTable = new RIVTable("intersections");
    
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
    RIVUnsignedShortRecord *primitiveIdRecord = new RIVUnsignedShortRecord("primitive ID");
    primitiveIdRecord->SetValues(primitveIds);
    RIVUnsignedShortRecord *shapeIdRecord = new RIVUnsignedShortRecord("shape ID");
    shapeIdRecord->SetValues(shapeIds);
    RIVFloatRecord *spectrumOneRecord = new RIVFloatRecord("spectrum 1");
    spectrumOneRecord->SetValues(spectraOne);
    RIVFloatRecord *spectrumTwoRecord = new RIVFloatRecord("spectrum 2");
    spectrumTwoRecord->SetValues(spectraTwo);
    RIVFloatRecord *spectrumThreeRecord = new RIVFloatRecord("spectrum 3");
    spectrumThreeRecord->SetValues(spectraThree);
    
    pathTable->AddRecord(xRecord);
    pathTable->AddRecord(yRecord);
    pathTable->AddRecord(tpOne);
//    dataset.AddRecord(tpTwo);
//    dataset.AddRecord(tpThree);
    pathTable->AddRecord(isects);
    
    //Intersection table
    intersectionsTable->AddRecord(isectPosX);
    intersectionsTable->AddRecord(isectPosY);
    intersectionsTable->AddRecord(isectPosZ);
    intersectionsTable->AddRecord(primitiveIdRecord);
    intersectionsTable->AddRecord(shapeIdRecord);
    intersectionsTable->AddRecord(spectrumOneRecord);
    intersectionsTable->AddRecord(spectrumTwoRecord);
    intersectionsTable->AddRecord(spectrumThreeRecord);
    
    RIVReference *reference = new RIVReference(pathTable, intersectionsTable);
    RIVReference *reverseReference = new RIVReference(intersectionsTable, pathTable);
    
    reference->SetReferences(references);
    reverseReference->SetReferences(references);
    
    pathTable->AddReference(reference);
    intersectionsTable->AddReference(reverseReference);
    
    dataset.AddTable(pathTable);
    dataset.AddTable(intersectionsTable);
    
//    Filter* filter = new RangeFilter("x",-2,-1); //Filter all
//    dataset.AddFilter(filter);
//
//    printf("references mapped : \n");
//    for(auto it  = references->begin() ; it != references->end() ; it++) {
//        printf("%lu : %lu\n",it->first,it->second);
//    }

    /*
    printf("*******************   DATASET READ   *******************\n");
    dataset.Print();
    printf("****************    END DATASET READ    ****************\n");
    */
    
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
    RIVTable *pathTable = new RIVTable("path");
    RIVTable *intersectionsTable = new RIVTable("intersections");
    
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
    
    pathTable->AddRecord(xRecord);
    pathTable->AddRecord(yRecord);
    pathTable->AddRecord(tpOne);
    //    dataset.AddRecord(tpTwo);
    //    dataset.AddRecord(tpThree);
    pathTable->AddRecord(isects);
    
    dataset.AddTable(pathTable);

    
    fclose(outputFile);
    fclose(inputFile);
    
    return dataset;
};