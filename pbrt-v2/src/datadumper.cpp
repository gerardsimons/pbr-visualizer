#include "core/stdafx.h"
#include "datadumper.h"

#include <stdio.h>
#include <map>

std::string DataDumper::ascii_file_name;
std::string DataDumper::binary_file_name;

ushort DataDumper::objectIdCounter = 1;

FILE* DataDumper::pathASCII;
FILE* DataDumper::pathBinary;
//File to write data about geometry (object IDs, shape IDs, vertices)
FILE* DataDumper::geometryFile;

size_t DataDumper::pathsWritten = 0;
size_t DataDumper::intersectionsWritten = 0;

const size_t DataDumper::bufferSize = 1000000;

std::vector<DataDumper> DataDumper::instances;

std::map<ushort,ushort> objectPrimitives;

bool writeASCII = true;

template<typename T,typename U>
void printMap(std::map<T,U> map) {
    for(typename std::map<T, U>::const_iterator it = map.begin();
        it != map.end(); ++it)
    {
        std::cout << it->first << " : " << it->second << "\n";
    }
}

DataDumper::DataDumper(const std::string& imagename, int ID) {
    //Each datadumper has its own
//    ascii_file_name = (imagename + std::to_string(ID) + ".txt").c_str();
//    binary_file_name = (imagename + std::to_string(ID) + ".bin").c_str();
				
    ascii_file_name = (imagename + ".txt").c_str();
    binary_file_name = (imagename + ".bin").c_str();
	geometryFile = fopen((imagename + "_geometry.txt").c_str(),"w");
    
    if(writeASCII)
        pathASCII = fopen(ascii_file_name.c_str(),"w");
    pathBinary = fopen(binary_file_name.c_str(),"wb");
    
    if(pathASCII == NULL && writeASCII) {
        throw "unable to open file " + ascii_file_name;
    }																												
    if(pathBinary == NULL) {
        throw "unable to open file " + binary_file_name;
    }
	if(geometryFile == NULL) {
		throw "unable to open geometry file";
	}
    
    //printf("Opening file.\n");
    //Init header of file
    
    //fprintf(pathASCII,"******************************************************************************************\n");
    //fprintf(pathASCII,"BSDF_REFLECTION=%d\n",BSDF_REFLECTION);
    //fprintf(pathASCII,"BSDF_TRANSMISSION=%d\n",BSDF_TRANSMISSION);
    //fprintf(pathASCII,"BSDF_DIFFUSE=%d\n",BSDF_DIFFUSE);
    //fprintf(pathASCII,"BSDF_GLOSSY=%d\n",BSDF_GLOSSY);
    //fprintf(pathASCII,"BSDF_SPECULAR=%d\n",BSDF_SPECULAR);
    //fprintf(pathASCII,"BSDF_ALL_TYPES=%d\n",BSDF_ALL_TYPES);
    //fprintf(pathASCII,"BSDF_ALL_REFLECTION=%d\n",BSDF_ALL_REFLECTION);
    //fprintf(pathASCII,"BSDF_ALL_TRANSMISSION=%d\n",BSDF_ALL_TRANSMISSION);
    //fprintf(pathASCII,"BSDF_ALL=%d\n",BSDF_ALL);
    //fprintf(pathASCII,"******************************************************************************************\n\n");
    if(writeASCII)
        fprintf(pathASCII,"%%imageX,imageY,lensU,lensV,timestamp,throughput,#isects,isect_positions,primitive_ids,shape_ids,object_ids,spectra,interaction_types,light_ids\n");
//		fprintf(pathASCII,"%%imageX,imageY,lensU,lensV,timestamp,throughput,#isects\n");
}
    
void DataDumper::Init(const std::string& imagename, int numDumpers) {
    //printf("DATADUMPER INIT\n");
    if(numDumpers < 1) {
        throw "Number of dumpers must be a positive number";
    }
    if(numDumpers != 1) {
        throw "Multiple dumpers not yet supported.";
    }
    
    //Create instances, give each their own unique ID
    for(int i = 0 ; i < numDumpers ; ++i) {
        DataDumper newInstance = DataDumper(imagename, i);
        instances.push_back(newInstance);
    }
}

void DataDumper::Finish() {
    
    //Write what remains in buffer to files
    for(DataDumper dumper : instances) {
        dumper.WriteBufferToFile();
    }

//    unsigned long long pathASCIISize = ftell(pathASCII);
    unsigned long long pathBinarySize = ftell(pathBinary);
    
    //printf("Closing file.\n");
//    fclose(pathASCII);
//    fclose(pathBinary);
//
//    //Open ASCII file in binary
//    pathASCII = fopen(ascii_file_name.c_str(),"rb");
//    pathBinary = fopen(binary_file_name.c_str(),"rb");
    
    printf("binary file %s size = %llu\n",binary_file_name.c_str(),pathBinarySize);
//    printf("ascii file %s size = %llu\n",ascii_file_name.c_str(),pathASCIISize);

//    float compression = (float)(pathASCIISize - pathBinarySize) / pathASCIISize;
//    printf("\ncompression = %f\n",compression);

    fclose(pathBinary);
    if(writeASCII)
        fclose(pathASCII);
    
    //Write the summary of the files to a new file
    //The name is the ASCII and binary files root name (e.g. without the extensions) appended with .summary
    std::string summaryFileName = ascii_file_name.substr(0, ascii_file_name.size() - 4) + ".summary";
    FILE* summaryFile = fopen(summaryFileName.c_str() , "w");
    
    //Write the number of paths and the number of intersections written
    fprintf(summaryFile,"%zu,%zu",pathsWritten,intersectionsWritten);
    
    //Close, we are done
    fclose(summaryFile);
}

//file should be a file opened in binary!
//likely not the most efficient way, but at least it is platform independent
unsigned long long DataDumper::FileSize(FILE* file) {
    int ch;
    /* error checking ommited for brevity */
    unsigned long long filesize = 0; /* or unsigned long for C89 compatability*/
    while ((ch = fgetc(file)) != EOF) filesize++;

    return filesize;
}

bool DataDumper::FileIsOpen() {
    return pathASCII != 0;
}

bool writing = false;

//According to an ID find the right datadumper instance and add the path data to its buffer
void DataDumper::AddToBuffer(size_t id, const PathData& pathData) {
    
    if(id >= instances.size())
        throw "No instance with ID ";
    
    DataDumper& dumper = instances[id];
    dumper.AddToBuffer(pathData);
}

void DataDumper::AddToBuffer(const PathData& pathData) {
    bufferData.push_back(pathData);
    if(bufferData.size() >= bufferSize) {
        //        printf("Writing buffer to file...\n");
        while(writing) {
            //Wait
            
        }
        //        printf("Buffer size = %zu \n",bufferData.size());
        WriteBufferToFile();
    }
}

int writeMax = 100;
int written = 0;
//Write buffer to file
void DataDumper::WriteBufferToFile() {
    writing = true;
    
//    if(pathASCII == NULL || pathBinary == NULL) {
//        throw "1 or more files are not open for writing.";
//    }
	bool writeIntersections = true;
    if(pathBinary == NULL) {
        throw "1 or more files are not open for writing.";
    }
    printf("Writing %zu data objects to file\n",bufferData.size());
    for(size_t i = 0 ; i < bufferData.size() ; ++i) {
        PathData data = bufferData[i];
        unsigned short nrIsects = (unsigned short) data.intersectionData.size();
//        if(written >= writeMax || nrIsects != 1) {
//            continue;
//        }
        written++;
        
        //ASCII
        float throughputRGB[3];
        data.throughput.ToRGB(throughputRGB);
        
        if(writeASCII) {
            fprintf(pathASCII,"%f,%f,%f,%f,%f,[%f,%f,%f],%hu",data.imageX,data.imageY,data.lensU,data.lensV,data.timestamp, throughputRGB[0],throughputRGB[1],throughputRGB[2],nrIsects);
        }
        
        fwrite(&data.imageX, sizeof(float), 1, pathBinary);
        fwrite(&data.imageY, sizeof(float), 1, pathBinary);
        fwrite(&data.lensU, sizeof(float), 1, pathBinary);
        fwrite(&data.lensV, sizeof(float), 1, pathBinary);
        fwrite(&data.timestamp, sizeof(float), 1, pathBinary);
        fwrite(&throughputRGB, sizeof(float), 3, pathBinary);
        fwrite(&nrIsects, sizeof(unsigned short), 1, pathBinary);
        
        //Update the counters
        ++pathsWritten;
        if(writeIntersections) {
			if(writeASCII) {
				if(nrIsects == 0) {
					fputs("\n", pathASCII);
					continue;
				}
				else fputs(",{",pathASCII);
			}
			
			for(ushort i = 0 ; i < nrIsects ; ++i) {
				IntersectData isectData = data.intersectionData[i];
				
				float positionFloat[] = { isectData.position.x, isectData.position.y, isectData.position.z };
				
				fprintf(pathASCII,"[%f,%f,%f]",positionFloat[0],positionFloat[1],positionFloat[2]);
				if(i != nrIsects - 1) {
					fputs(",",pathASCII);
				}
				fwrite(&positionFloat,sizeof(float),3,pathBinary);
			}
			fputs("},",pathASCII);
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
	//            isectData->primitiveId = 4;

	//            char buffer[100];
	//            sprintf(buffer,"%hu,",isectData->primitiveId);
	//            fprintf(pathASCII,buffer);
				if(writeASCII)
					fprintf(pathASCII, "%hu,",isectData.primitiveId);
				fwrite(&isectData.primitiveId,sizeof(unsigned short),1,pathBinary); //2 bytes
			}
			//fprintf(pathASCII,"},");

			//write shape ids, size * integers
			//fprintf(pathASCII,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
	//            isectData->shapeId = 5;

	//            char buffer[100];
	//            sprintf(buffer,"%hu,",isectData->shapeId);
				if(writeASCII)
					fprintf(pathASCII, "%hu,",isectData.shapeId);
				fwrite(&isectData.shapeId,sizeof(unsigned short),1,pathBinary); //2 bytes
			}
			
			//Write the objectId created using ObjectInstance
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
				//Find the object ID for this primitive ID
				ushort objectID = objectIdForPrimitiveId(isectData.primitiveId);
				if(writeASCII)
					fprintf(pathASCII, "%hu,",objectID);
				fwrite(&objectID,sizeof(unsigned short),1,pathBinary); //2 bytes
			}

			//write color spectra size * float[nSamples]
			fprintf(pathASCII,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
				const RGBSpectrum &s = isectData.spectrum.ToRGBSpectrum();
				float color[3];
				s.ToRGB(color);
				if(writeASCII) {
					if(i != nrIsects - 1) {
						fprintf(pathASCII,"[%f,%f,%f],",color[0],color[1],color[2]);
					}
					else
						fprintf(pathASCII,"[%f,%f,%f]},",color[0],color[1],color[2]);
				}

				fwrite(&color,sizeof(float),3,pathBinary); //12 bytes
			}
			//fprintf(pathASCII,"},");

			//write interaction types, size * unsigned short
			//fprintf(pathASCII,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
				if(writeASCII)
					fprintf(pathASCII,"%hu,",isectData.interactionType);
				fwrite(&isectData.interactionType,sizeof(unsigned short),1,pathBinary); //2 bytes
			}
			//fprintf(pathASCII,"},");

			//Write light ids size * unsigned short (2N)
			//fprintf(pathASCII,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];

				if(writeASCII)
					fprintf(pathASCII,"%hu",isectData.lightId);
				fwrite(&isectData.lightId,sizeof(unsigned short),1,pathBinary); //2 bytes
			}
		}
        if(writeASCII)
            fprintf(pathASCII,"\n");
        //Flush the buffers
        fflush(pathASCII);
        fflush(pathBinary);
        
        intersectionsWritten += nrIsects;
    }
    //Clear buffer
    writing = false;
    bufferData.clear();
//    printf("Buffer data size after clearing = %zu\n",bufferData.size());
//    fprintf(pathASCII,"\n");
}

ushort DataDumper::objectIdForPrimitiveId(ushort primitiveId) {
	ushort objectId = objectPrimitives[primitiveId];
	if(objectId == 0) {
//		printf("*** WARNING : No object ID for primitive ID %hu ***\n",primitiveId);
	}
	return objectId;
}

void DataDumper::AddObjectPrimitiveMapping(ushort primitiveId) {
	printf("New object to primitve mapping: \n");
	objectPrimitives[primitiveId] = objectIdCounter;

	printf("primitives - object map: ");
	printMap(objectPrimitives);
	++objectIdCounter;
}