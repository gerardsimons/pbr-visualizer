#include "DataFileReader.h"

#include <stdio.h>

DataFileReader::DataFileReader(void)
{
}


DataFileReader::~DataFileReader(void)
{
}

void DataFileReader::LoadData(std::string fileName) {
	FILE *inputFile = fopen(fileName.c_str(),"rb");
	if(inputFile == 0) {
		throw "Error opening file.";
	}
	while(!feof(inputFile)) {

		//Read x
		unsigned short x;
		fread(&x,sizeof(unsigned short),1,inputFile);

		//Read y
		unsigned short y;
		fread(&y,sizeof(unsigned short),1,inputFile);

		float throughput[3];
		fread(throughput,sizeof(float),3,inputFile);

		//Write the number of intersections 
		unsigned short size;
		fwrite(&size,sizeof(unsigned short),1,inputFile);

		printf("%hu,%hu,[%f,%f,%f],%hu\n",x,y,throughput[0],throughput[1],throughput[2],size);
	}
}
