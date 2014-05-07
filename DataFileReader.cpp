#include "DataFileReader.h"

#include <stdio.h>
#include <vector>

DataFileReader::DataFileReader(void)
{
}


DataFileReader::~DataFileReader(void)
{
}

RIVDataSet DataFileReader::LoadData(std::string fileName) {
	printf("Trying to load %s\n",fileName.c_str());
	FILE *inputFile = fopen(fileName.c_str(),"rb");
	if(inputFile == 0) {
		throw "Error opening file.";
	}

	std::vector<float> xPixelData;
	std::vector<float> yPixelData;
	std::vector<float> throughPutOne;
	std::vector<float> throughPutTwo;
	std::vector<float> throughPutThree;
	std::vector<float> intersections;

	int lineNumber = 1;

	while(!feof(inputFile)) {

		//Read x
		unsigned short x;
		fread(&x,sizeof(unsigned short),1,inputFile);
		xPixelData.push_back((float)x);

		//Read y
		unsigned short y;
		fread(&y,sizeof(unsigned short),1,inputFile);
		yPixelData.push_back((float)y);

		float throughput[3];
		fread(throughput,sizeof(float),3,inputFile);
		throughPutOne.push_back(throughput[0]);
		throughPutTwo.push_back(throughput[1]);
		throughPutThree.push_back(throughput[2]);

		//Read the number of intersections
		unsigned short size;
		fread(&size,sizeof(unsigned short),1,inputFile);
		intersections.push_back((float)size);

		//printf("%d = %hu,%hu,[%f,%f,%f],%hu\n",lineNumber,x,y,throughput[0],throughput[1],throughput[2],size);
		++lineNumber;
	}
	RIVDataSet dataset;
	RIVRecord xRecord("x",xPixelData);
	RIVRecord yRecord("y",yPixelData);
	RIVRecord tpOne("throughput 1",throughPutOne);
	RIVRecord tpTwo("throughput 2",throughPutTwo);
	RIVRecord tpThree("throughput 3",throughPutThree);
	RIVRecord isects("intersections",intersections);

	dataset.AddRecord(xRecord);
	dataset.AddRecord(yRecord);
	dataset.AddRecord(tpOne);
	dataset.AddRecord(tpTwo);
	dataset.AddRecord(tpThree);
	dataset.AddRecord(isects);

	return dataset;
}
