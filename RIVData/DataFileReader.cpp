#include "DataFileReader.h"


#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <regex>

#include "../reporter.h"

DataFileReader::DataFileReader(void)
{
    
}


DataFileReader::~DataFileReader(void)
{
    
}

bool is_number(const std::string& s)
{
    for(const char &c : s) {
        if(!std::isdigit(c) && c != '.' && c != '-') {
            return false;
        }
    }
    return !s.empty();
}

MeshModel DataFileReader::ReadModelData(const std::string& fileName) {
    std::ifstream is;
    is.open (fileName, std::ios::in );
    
    is.seekg (0, std::ios::beg);
    std::vector<float> vertices;
    
    std::string ignoreChars = "[]";
    
    std::regex rx("point");
    
    size_t lineNumber = 0;
    
    if (is.is_open())
    {
        char delimiter = ' ';
        std::string token;
        std::string line;
        while (getline(is,line)) {
//            std::cout << line;
            ++lineNumber;
            size_t tokenCount = 0;
            bool shapeFound = false;
            bool pointFound = false;
            //            printf("Reading line %zu\n",lineNumber);
            for(char c : line) {
                //                printf("Reading char %c \n",c);
                if(c == delimiter) {
                    //Process token
                    ++tokenCount;
                    //                    printf("Token = '%s'\n",token.c_str());
                    if(!shapeFound) {
                        if(token == "Shape") {
                            shapeFound = true;
                            //                            printf("Shape declaration found at line %zu\n",lineNumber);
                        }
                    }
                    else if(!pointFound){ //Look for point declaration
                        
                        if(std::regex_search(token.begin(), token.end(),rx)) {
                            //                            printf("Point declaration found\n");
                            pointFound = true;
                        }
                    }
                    else if(is_number(token)) {
                        //Add to vertex
                        float vertex = std::stof(token);
                        //                        printf("adding vertex %f\n",vertex);
                        vertices.push_back(vertex);
                    }
                    else {
                        //This line is done
                        //                        printf("This line finished at token #%zu = %s\n",tokenCount,token.c_str());
                        token.clear();
                        break;
                    }
                    token.clear();
                }
                else {
                    if(token.empty() && (c == '"' || c == ' ')) {
                        delimiter = c;
                        //                        printf("Changed delimiter to '%c'\n",c);
                    }
                    else {
                        bool ignore = false;
                        for(char ignoreC : ignoreChars) {
                            if(ignoreC == c) {
                                ignore = true;
                            }
                        }
                        if(!ignore) token.push_back(c);
                    }
                }
            }
        }
    }
    else {
        throw "Unable to open PBRT file " + fileName;
    }
    printf("Found %zu vertices in PBRT file.\n",vertices.size());
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

RIVDataSet DataFileReader::ReadAsciiData(const std::string& fileName, const BMPImage& image, const size_t pathsLimit) {
    std::ifstream is;
    is.open (fileName, std::ios::in );
    
    RIVDataSet dataset;
    
    //First pass, count the number of path records
    size_t pathsFound = 0;
    std::string line;
    if(is.is_open()) {
        is.seekg (0, std::ios::beg);
        while (getline(is,line)) {
            if(line[0] != '%') {
                ++pathsFound;
            }
        }
        is.seekg (0, std::ios::beg);
    }
    is.close();;
    is.open(fileName,std::ios::in);
    
    double probability = 1;
    if(pathsLimit > 0) { //Probabilistic sampling
        probability = pathsFound / (double) pathsLimit;
    }
    

    if (is.is_open())
    {
        std::vector<ushort> xPixelData;
        std::vector<ushort> yPixelData;
        
        std::vector<float> throughPutOne;  //We only use the first one for now, as they all seem the same every time
        std::vector<ushort> nrOfIntersections;
        
        std::vector<float> intersectionPosX;
        std::vector<float> intersectionPosY;
        std::vector<float> intersectionPosZ;
        std::vector<ushort> primitveIds;
        std::vector<ushort> shapeIds;
        std::vector<float> spectraOne;
        std::vector<float> spectraTwo;
        std::vector<float> spectraThree;
        std::vector<ushort> interactionTypes;
        std::vector<ushort> lightIds;
        
        ushort x;
        ushort y;
        float throughput[3] = {-1.F,-1.F,-1.F};
        ushort intersections_size;
        
        float intersectPosX;
        float intersectPosY;
        float intersectPosZ;
        
        ushort primitiveId;
        ushort shapeId;
        float spectrumOne;
        float spectrumTwo;
        float spectrumThree;
        ushort interactionType;
        ushort lightId;
        
        std::string ignoreList = "[]{}";
        
        size_t lineNumber = 0;
        
        size_t pathIndex = 0;
        size_t intersectionIndex = 0;
        
        RIVTable* imageTable = new RIVTable("image");
        
        RIVUnsignedShortRecord *imageRedRecord = new RIVUnsignedShortRecord("R",0,255);
        RIVUnsignedShortRecord *imageGreenRecord = new RIVUnsignedShortRecord("G",0,255);
        RIVUnsignedShortRecord *imageBlueRecord = new RIVUnsignedShortRecord("B",0,255);
      // Do not usually include these, only include x,y with actual path values (from pbrt out files)
//        RIVUnsignedShortRecord *xPixelRecord = new RIVUnsignedShortRecord("x");
//        RIVUnsignedShortRecord *yPixelRecord = new RIVUnsignedShortRecord("y");
        
        size_t iteration = 0;
        
        std::vector<ushort> redChannel;
        std::vector<ushort> greenChannel;
        std::vector<ushort> blueChannel;
        std::vector<ushort> xPixels;
        std::vector<ushort> yPixels;
        
        //Fill the image table, sorted on x,y
        for(int x = 0 ; x < image.sizeX ; x++) {
            for(int y = 0 ; y < image.sizeY ; y++) {
                
                unsigned short* rgb = image.RGB(x,y);
                int R = rgb[0];
                int G = rgb[1];
                int B = rgb[2];
                
//                printf("image.data pixel %lu (%d,%d) (R,G,B) = (%d,%d,%d)\n",pixelPosition / channels, (x+1), (y+1), R,G,B);
                
                ++iteration;
                
                xPixels.push_back(x+1);
                yPixels.push_back(y+1);
                redChannel.push_back(R);
                greenChannel.push_back(G);
                blueChannel.push_back(B);
                
            }
        }
        
        std::map<size_t,std::vector<size_t>> imagePathReferences = std::map<size_t,std::vector<size_t>>();
        std::map<size_t,std::vector<size_t>> pathIsectReferences = std::map<size_t,std::vector<size_t>>();
        std::string buffer;
        while (getline(is,line)) {
            ++lineNumber;
            if(!line.empty() && line[0] != '%') { //Is a valid path declaration
                
                float randomFloat = rand() / RAND_MAX;
                if(pathsLimit > 0 && randomFloat >= probability)
                    continue;
                
                
                std::vector<std::string> exploded = explode(line,',',ignoreList);
                
                x = std::stoul(exploded[0],nullptr,0);
                y = std::stoul(exploded[1],nullptr,0);
                throughput[0] = std::stof(exploded[2],0);
                throughput[1] = std::stof(exploded[3],0);
                throughput[2] = std::stof(exploded[4],0);
                intersections_size = std::stoul(exploded[5],nullptr,0);
                
                //Hacky way because it is ordered on x,y
                size_t imageTableIndex = (x - 1) * image.sizeX + (y - 1);
//                printf("imagetableindex for x = %d, y = %d = %zu\n",x,y,imageTableIndex);
//                std::vector<RIVRecord*> imageRecords = imageTable->GetRecords();

                    std::vector<size_t> *indices = &(imagePathReferences)[imageTableIndex];
                    indices->push_back(pathIndex);

                
                
//                while(iterator->GetNext(imageRow) && (!xFound || !yFound)) {
////                    printf("imagerow = %zu\n",imageRow);
//                    size_t numberOfRecords = imageTable->NumberOfColumns();
//                    bool found = false;
//                    for(size_t recordI = 0 ; recordI < numberOfRecords ; ++recordI) {
//                        RIVRecord* record = imageTable->GetRecord(recordI);
//                        RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(record);
//                        if(shortRecord) {
//                            if(shortRecord->name == "x" && shortRecord->Value(imageRow) == x) {
//                                xFound = true;
//                            }
//                            if(shortRecord->name == "y" && shortRecord->Value(imageRow) == y) {
//                                yFound = true;
//                            }
//                            if(xFound && yFound) {
//                                (*imagePathReferences)[pathIndex] = std::pair<size_t,size_t>(pathIndex,pathIndex);
////                                printf("image to path connection created.\n");
////                                test_count++;
//                                found = true;
//                                break;
//                            }
//                        }
//                    }
//                    if(found) break;
//                }
                
                //Do this at least once, even if there are no intersections!
                
                xPixelData.push_back(x);
                yPixelData.push_back(y);
                throughPutOne.push_back(throughput[0]);
//                throughPutTwo.push_back(throughput[1]);
//                throughPutThree.push_back(throughput[2]);
                nrOfIntersections.push_back(intersections_size);
                
                size_t endIntersections = 5 + intersections_size * 3;
                size_t endPrimitiveIds = endIntersections + intersections_size;
                size_t endShapeIds = endPrimitiveIds + intersections_size;
                size_t endSpectrum = endShapeIds + intersections_size * 3;
                size_t endInteractions = endSpectrum + intersections_size;
                
                //sanity checks
//                if(throughput[0] > 1.F) printf("lineNumber %zu has insane throughput[0]\n",lineNumber);
//                if(throughput[1] > 1.F) printf("lineNumber %zu has insane throughput[1]\n",lineNumber);
//                if(throughput[2] > 1.F) printf("lineNumber %zu has insane throughput[2]\n",lineNumber);
                
                //                    printf("line %d : \n",lineNumber);
                //                    printf("endIntersections = %d endPrimitiveIds = %d endShapeIds = %d\n",endIntersections,endPrimitiveIds,endShapeIds);
                
                if(intersections_size > 0) {
                    //                    (*references)[pathIndex].first = intersectionIndex;
                    std::vector<size_t> range;
                    for(size_t i = 0 ; i < intersections_size ; i++) {
                        range.push_back(intersectionIndex);
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
                        interactionTypes.push_back(interactionType);
                        lightIds.push_back(lightId);
                        
//                        if(spectrumOne > 1.F) printf("lineNumber %zu has insane spectrum[0]\n",lineNumber);
//                        if(spectrumTwo > 1.F) printf("lineNumber %zu has insane spectrum[1]\n",lineNumber);
//                        if(spectrumThree > 1.F) printf("lineNumber %zu has insane spectrum[2]\n",lineNumber);
                        
                        ++intersectionIndex;
                    }
                    (pathIsectReferences)[pathIndex] = range;
                }
                ++pathIndex;
            }
        }
        RIVTable *pathTable = new RIVTable("path");
        RIVTable *intersectionsTable = new RIVTable("intersections");
        
        //Record definitions
        RIVUnsignedShortRecord *xRecord = new RIVUnsignedShortRecord("x");
        xRecord->SetValues(xPixelData);
        RIVUnsignedShortRecord *yRecord = new RIVUnsignedShortRecord("y");
        yRecord->SetValues(yPixelData);
        RIVFloatRecord *tpOne = new RIVFloatRecord("throughput 1");
        tpOne->SetValues(throughPutOne);
//        RIVFloatRecord *tpTwo = new RIVFloatRecord("throughput 2");
//        tpTwo->SetValues(throughPutTwo);
//        RIVFloatRecord *tpThree = new RIVFloatRecord("throughput 3");
//        tpThree->SetValues(throughPutThree);
        RIVUnsignedShortRecord *isects = new RIVUnsignedShortRecord("#intersections");
        isects->SetValues(nrOfIntersections);
        
        RIVFloatRecord *isectPosX = new RIVFloatRecord("intersection X",intersectionPosX);
        RIVFloatRecord *isectPosY = new RIVFloatRecord("intersection Y",intersectionPosY);
        RIVFloatRecord *isectPosZ = new RIVFloatRecord("intersection Z",intersectionPosZ);
        RIVUnsignedShortRecord *primitiveIdRecord = new RIVUnsignedShortRecord("primitive ID",primitveIds);
        RIVUnsignedShortRecord *shapeIdRecord = new RIVUnsignedShortRecord("shape ID",shapeIds);
        RIVFloatRecord *spectrumOneRecord = new RIVFloatRecord("spectrum 1",spectraOne);
        RIVFloatRecord *spectrumTwoRecord = new RIVFloatRecord("spectrum 2",spectraTwo);
        RIVFloatRecord *spectrumThreeRecord = new RIVFloatRecord("spectrum 3",spectraThree);
        RIVUnsignedShortRecord *interactionTypesRecord = new RIVUnsignedShortRecord("interaction types",interactionTypes);
        RIVUnsignedShortRecord *lightIdsRecord = new RIVUnsignedShortRecord("light ids",lightIds);
        
        imageRedRecord->SetValues(redChannel);
        imageGreenRecord->SetValues(greenChannel);
        imageBlueRecord->SetValues(blueChannel);
//        xPixelRecord->SetValues(xPixels);
//        yPixelRecord->SetValues(yPixels);
        
        imageTable->AddRecord(imageRedRecord);
        imageTable->AddRecord(imageGreenRecord);
        imageTable->AddRecord(imageBlueRecord);
//        imageTable->AddRecord(xPixelRecord);
//        imageTable->AddRecord(yPixelRecord);
        
        
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
        intersectionsTable->AddRecord(interactionTypesRecord);
        intersectionsTable->AddRecord(lightIdsRecord);
        
        RIVReference imagePathReference = RIVReference(imageTable,pathTable);
        imagePathReference.SetReferences(imagePathReferences);
        RIVReference pathImageReference = imagePathReference.ReverseReference();
        
        imageTable->AddReference(imagePathReference);
        pathTable->AddReference(pathImageReference);
        
        RIVReference referenceToIntersections = RIVReference(pathTable, intersectionsTable);
        referenceToIntersections.SetReferences(pathIsectReferences);
        RIVReference reverseReference = referenceToIntersections.ReverseReference();
        
        pathTable->AddReference(referenceToIntersections);
        intersectionsTable->AddReference(reverseReference);
        
        dataset.AddTable(imageTable);
        dataset.AddTable(pathTable);
        dataset.AddTable(intersectionsTable);
        
        //    Filter* filter = new RangeFilter("x",-2,-1); //Filter all
        //    dataset.AddFilter(filter);
        //
        //    printf("references mapped : \n");
        //    for(auto it  = references->begin() ; it != references->end() ; it++) {
        //        printf("%lu : (%lu,%lu)\n",it->first,it->second.first, it->second.second);
        //    }
        
         printf("*******************   DATASET READ   *******************\n");
         dataset.Print(1000);
         printf("****************    END DATASET READ    ****************\n");
//
        printf("%zu path data records read.\n",pathTable->GetNumRows());
        printf("%zu intersection data records read.\n",intersectionsTable->GetNumRows());
        
        is.close();
    }
    else {
        throw "Could not open file " + fileName;
    }
    return dataset;
}

//Read data from the image, return as table
RIVTable* DataFileReader::ReadImageData(const BMPImage& image) {
    RIVTable* imageTable = new RIVTable("image");
    
    RIVUnsignedShortRecord *imageRedRecord = new RIVUnsignedShortRecord("R");
    RIVUnsignedShortRecord *imageGreenRecord = new RIVUnsignedShortRecord("G");
    RIVUnsignedShortRecord *imageBlueRecord = new RIVUnsignedShortRecord("B");
    
    std::vector<ushort> redChannel;
    std::vector<ushort> greenChannel;
    std::vector<ushort> blueChannel;
    
    int channels = 3 + image.hasAlpha;
    
    int iteration = 0;
    
    for(size_t x = 0 ; x < image.sizeX ; x++) {
        for(size_t y = 0 ; y < image.sizeY ; y++) {
            size_t pixelPosition = iteration * channels;
            
            int B = image.data[pixelPosition];
            int G = image.data[pixelPosition + 1];
            int R = image.data[pixelPosition + 2];
//            int A = image.data[pixelPosition + 3];
            
//            printf("image.data pixel %lu (%d,%d) (R,G,B) = (%d,%d,%d)\n",pixelPosition / channels, (x+1), (y+1), R,G,B);
            
            ++iteration;
            
            redChannel.push_back(R);
            greenChannel.push_back(G);
            blueChannel.push_back(B);
//                int A = image.data[pixelPosition]; ignore alpha channels
            
        }
    }
    
    printf("total number of iterations = %d\n",iteration);
    
    imageRedRecord->SetValues(redChannel);
    imageGreenRecord->SetValues(greenChannel);
    imageBlueRecord->SetValues(blueChannel);

    imageTable->AddRecord(imageRedRecord);
    imageTable->AddRecord(imageGreenRecord);
    imageTable->AddRecord(imageBlueRecord);

    return imageTable;
}

RIVDataSet DataFileReader::ReadBinaryData(const std::string& fileName, const BMPImage& image, const size_t pathsLimit) {
    printf("Loading binary dataset from file %s\n",fileName.c_str());
    
    std::string taskName = "Binary data reading";
    reporter::startTask(taskName);
    
    FILE *inputFile = fopen(fileName.c_str(),"rb");
    
    //write what was read as ASCII, strictly used only for debugging
//    FILE *outputFile = fopen("output.txt","w");
    
    if(inputFile == 0) {
        throw "Error opening file.";
    }
    
    //Path data
    std::vector<ushort> xPixelData;
    std::vector<ushort> yPixelData;
    std::vector<float> lensUs;
    std::vector<float> lensVs;
    std::vector<float> timestamps;
    std::vector<float> throughPutOne;
    std::vector<float> throughPutTwo;
    std::vector<float> throughPutThree;
    std::vector<ushort> intersections;
    
    std::vector<ushort> bounceNumbers;
    std::vector<float> intersectionPosX;
    std::vector<float> intersectionPosY;
    std::vector<float> intersectionPosZ;
    std::vector<ushort> primitveIds;
    std::vector<ushort> shapeIds;
    std::vector<float> spectraOne;
    std::vector<float> spectraTwo;
    std::vector<float> spectraThree;
    std::vector<ushort> interactionTypes;
    std::vector<ushort> lightIds;
    
//    fprintf(outputFile, "x,y,lensU,lensV,timestamp,throughput_1,throughput_2,throughput_3,size\n");
    
    std::map<size_t,std::vector<size_t>> pathIsectReferences = std::map<size_t,std::vector<size_t>>();
    
    size_t isect_index = 0;
    size_t path_index = 0;
    
    while(!feof(inputFile) && (path_index < pathsLimit || pathsLimit == 0)) {
        
//        printf("reading line %d\n",lineNumber);
        
        //Unrounded continuous x pixel data
        float x;
        float y;
        float lensU;
        float lensV;
        float timestamp;
        float throughput[3] = {-1.F,-1.F,-1.F};
        ushort size;
        
        fread(&x,sizeof(float),1,inputFile);
//        char buffer[30];
//        sprintf(buffer, "%f,",x);
//        fprintf(outputFile, buffer);
        
        //Read y
        fread(&y,sizeof(float),1,inputFile);
//        sprintf(buffer, "%f,",y);
//        fprintf(outputFile, buffer);
        
        //read lens U
        fread(&lensU,sizeof(float),1,inputFile);
//        sprintf(buffer, "%f,",lensU);
//        fprintf(outputFile, buffer);
        
        //read lens V
        fread(&lensV,sizeof(float),1,inputFile);
//        sprintf(buffer, "%f,",lensV);
//        fprintf(outputFile, buffer);
        
        //read timestamp
        fread(&timestamp,sizeof(float),1,inputFile);
//        sprintf(buffer, "%f,",timestamp);
//        fprintf(outputFile, buffer);
        
        // throughputs
        fread(throughput,sizeof(float),3,inputFile);
//        sprintf(buffer, "%f,%f,%f,",throughput[0],throughput[1],throughput[2]);
//        fprintf(outputFile, buffer);
        
        //Read the number of intersections
        fread(&size,sizeof(ushort),1,inputFile);
//        sprintf(buffer, "%hu\n",size);
//        fprintf(outputFile, buffer);
        
        xPixelData.push_back(x);
        yPixelData.push_back(y);
        lensUs.push_back(lensU);
        lensVs.push_back(lensV);
        timestamps.push_back(timestamp);
        throughPutOne.push_back(throughput[0]);
        throughPutTwo.push_back(throughput[1]);
        throughPutThree.push_back(throughput[2]);
        intersections.push_back(size);
        
        std::vector<size_t> isectIndices;

            
        //isect_x,isect_y,isect_z,primitive_ids,shape_ids,spectra,interaction_types,light_ids
        float isectX;
        float isectY;
        float isectZ;
        ushort primitiveId;
        ushort shapeId;
        float spectraR;
        float spectraG;
        float spectraB;
        ushort interactionType;
        ushort lightId;
        
        for(ushort i = 0 ; i < size ; ++i) {
            isectIndices.push_back(isect_index);
            bounceNumbers.push_back(i+1);
            
            fread(&isectX,sizeof(float),1,inputFile);
            fread(&isectY,sizeof(float),1,inputFile);
            fread(&isectZ,sizeof(float),1,inputFile);
            
            intersectionPosX.push_back(isectX);
            intersectionPosY.push_back(isectY);
            intersectionPosZ.push_back(isectZ);
        }
        
        for(ushort i = 0 ; i < size ; ++i) {
            fread(&primitiveId,sizeof(unsigned short),1,inputFile);
            
            primitveIds.push_back(primitiveId);
        }
        
        for(ushort i = 0 ; i < size ; ++i) {
            fread(&shapeId,sizeof(unsigned short),1,inputFile);
            
            shapeIds.push_back(shapeId);
        }
        
        for(ushort i = 0 ; i < size ; ++i) {
            fread(&spectraR,sizeof(float),1,inputFile);
            fread(&spectraG,sizeof(float),1,inputFile);
            fread(&spectraB,sizeof(float),1,inputFile);
            
            spectraOne.push_back(spectraR);
            spectraTwo.push_back(spectraG);
            spectraThree.push_back(spectraB);
        }
        
        for(ushort i = 0 ; i < size ; ++i) {
            fread(&interactionType,sizeof(unsigned short),1,inputFile);
            
            interactionTypes.push_back(interactionType);
        }
        
        for(ushort i = 0 ; i < size ; ++i) {
            fread(&lightId,sizeof(unsigned short),1,inputFile);
            
            lightIds.push_back(lightId);
        }
            
        isect_index += size;
        pathIsectReferences[path_index] = isectIndices;
        
        
        //printf("%d = %hu,%hu,[%f,%f,%f],%hu\n",lineNumber,x,y,throughput[0],throughput[1],throughput[2],size);
        ++path_index;
        
    }
    
    //Declare dataset and table objects
    RIVDataSet dataset;
    
    RIVTable *pathTable = new RIVTable("path");
    RIVTable *intersectionsTable = new RIVTable("intersections");
    
    RIVUnsignedShortRecord *xRecord = new RIVUnsignedShortRecord("x",xPixelData);
    RIVUnsignedShortRecord *yRecord = new RIVUnsignedShortRecord("y",yPixelData);
    RIVFloatRecord *lensURecord = new RIVFloatRecord("lens U",lensUs);
    RIVFloatRecord *lensVRecord = new RIVFloatRecord("lens V",lensVs);
    RIVFloatRecord *timestampRecord = new RIVFloatRecord("timestamp",timestamps);
    RIVFloatRecord *tpOne = new RIVFloatRecord("throughput 1",throughPutOne);
    RIVFloatRecord *tpTwo = new RIVFloatRecord("throughput 2",throughPutTwo);
    RIVFloatRecord *tpThree = new RIVFloatRecord("throughput 3",throughPutThree);
    RIVUnsignedShortRecord *isects = new RIVUnsignedShortRecord("#intersections",intersections);
    
    RIVUnsignedShortRecord *bounceNumberRecord = new RIVUnsignedShortRecord("bounce#",bounceNumbers);
    RIVFloatRecord *isectPosX = new RIVFloatRecord("intersection X",intersectionPosX);
    RIVFloatRecord *isectPosY = new RIVFloatRecord("intersection Y",intersectionPosY);
    RIVFloatRecord *isectPosZ = new RIVFloatRecord("intersection Z",intersectionPosZ);
    RIVUnsignedShortRecord *primitiveIdRecord = new RIVUnsignedShortRecord("primitive ID",primitveIds);
    RIVUnsignedShortRecord *shapeIdRecord = new RIVUnsignedShortRecord("shape ID",shapeIds);
    RIVFloatRecord *spectrumOneRecord = new RIVFloatRecord("spectrum 1",spectraOne);
    RIVFloatRecord *spectrumTwoRecord = new RIVFloatRecord("spectrum 2",spectraTwo);
    RIVFloatRecord *spectrumThreeRecord = new RIVFloatRecord("spectrum 3",spectraThree);
    RIVUnsignedShortRecord *interactionTypesRecord = new RIVUnsignedShortRecord("interaction types",interactionTypes);
    RIVUnsignedShortRecord *lightIdsRecord = new RIVUnsignedShortRecord("light ids",lightIds);
    
    pathTable->AddRecord(xRecord);
    pathTable->AddRecord(yRecord);
    pathTable->AddRecord(lensURecord);
    pathTable->AddRecord(lensVRecord);
    pathTable->AddRecord(timestampRecord);
    pathTable->AddRecord(tpOne);
    pathTable->AddRecord(tpTwo);
    pathTable->AddRecord(tpThree);
    pathTable->AddRecord(isects);
    
    intersectionsTable->AddRecord(bounceNumberRecord);
    intersectionsTable->AddRecord(isectPosX);
    intersectionsTable->AddRecord(isectPosY);
    intersectionsTable->AddRecord(isectPosZ);
    intersectionsTable->AddRecord(primitiveIdRecord);
    intersectionsTable->AddRecord(shapeIdRecord);
    intersectionsTable->AddRecord(spectrumOneRecord);
//    intersectionsTable->AddRecord(spectrumTwoRecord);
//    intersectionsTable->AddRecord(spectrumThreeRecord);
//    intersectionsTable->AddRecord(interactionTypesRecord);
//    intersectionsTable->AddRecord(lightIdsRecord);
    
    // Create and apply references to tables
    RIVReference referenceToIntersections = RIVReference(pathTable, intersectionsTable);
    referenceToIntersections.SetReferences(pathIsectReferences);
    RIVReference reverseReference = referenceToIntersections.ReverseReference();
    
    pathTable->AddReference(referenceToIntersections);
    intersectionsTable->AddReference(reverseReference);
    
    //Add tables to dataset
    dataset.AddTable(pathTable);
    dataset.AddTable(intersectionsTable);
    
//    fclose(outputFile);
    fclose(inputFile);
    
    printf("*******************   DATASET READ   *******************\n");
    dataset.Print(1000);
    printf("****************    END DATASET READ    ****************\n");
    //
    printf("%zu path data records read.\n",pathTable->GetNumRows());
    printf("%zu intersection data records read.\n",intersectionsTable->GetNumRows());
    
    reporter::stop(taskName);
    
    return dataset;
};