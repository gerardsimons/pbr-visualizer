#include "core/stdafx.h"
#include "datadumper.h"
#include <stdio.h>


std::string DataDumper::ascii_file_name;
std::string DataDumper::binary_file_name;

FILE* DataDumper::asciiFile;
FILE* DataDumper::binaryFile;

size_t DataDumper::pathsWritten = 0;
size_t DataDumper::intersectionsWritten = 0;

const size_t DataDumper::bufferSize = 1000000;

std::vector<DataDumper> DataDumper::instances;

bool writeASCII = true;

DataDumper::DataDumper(const std::string& imagename, int ID) {
    //Each datadumper has its own
//    ascii_file_name = (imagename + std::to_string(ID) + ".txt").c_str();
//    binary_file_name = (imagename + std::to_string(ID) + ".bin").c_str();
				
    ascii_file_name = (imagename + ".txt").c_str();
    binary_file_name = (imagename + ".bin").c_str();
    
    if(writeASCII)
        asciiFile = fopen(ascii_file_name.c_str(),"w");
    binaryFile = fopen(binary_file_name.c_str(),"wb");
    
    if(asciiFile == NULL && writeASCII) {
        throw "unable to open file " + ascii_file_name;
    }																												
    if(binaryFile == NULL) {
        throw "unable to open file " + binary_file_name;
    }
    
    //printf("Opening file.\n");
    //Init header of file
    
    //fprintf(asciiFile,"******************************************************************************************\n");
    //fprintf(asciiFile,"BSDF_REFLECTION=%d\n",BSDF_REFLECTION);
    //fprintf(asciiFile,"BSDF_TRANSMISSION=%d\n",BSDF_TRANSMISSION);
    //fprintf(asciiFile,"BSDF_DIFFUSE=%d\n",BSDF_DIFFUSE);
    //fprintf(asciiFile,"BSDF_GLOSSY=%d\n",BSDF_GLOSSY);
    //fprintf(asciiFile,"BSDF_SPECULAR=%d\n",BSDF_SPECULAR);
    //fprintf(asciiFile,"BSDF_ALL_TYPES=%d\n",BSDF_ALL_TYPES);
    //fprintf(asciiFile,"BSDF_ALL_REFLECTION=%d\n",BSDF_ALL_REFLECTION);
    //fprintf(asciiFile,"BSDF_ALL_TRANSMISSION=%d\n",BSDF_ALL_TRANSMISSION);
    //fprintf(asciiFile,"BSDF_ALL=%d\n",BSDF_ALL);
    //fprintf(asciiFile,"******************************************************************************************\n\n");
    if(writeASCII)
        fprintf(asciiFile,"%%imageX,imageY,lensU,lensV,timestamp,throughput,#isects,isect_positions,primitive_ids,shape_ids,spectra,interaction_types,light_ids\n");
//		fprintf(asciiFile,"%%imageX,imageY,lensU,lensV,timestamp,throughput,#isects\n");
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

//    unsigned long long asciiFileSize = ftell(asciiFile);
    unsigned long long binaryFileSize = ftell(binaryFile);
    
    //printf("Closing file.\n");
//    fclose(asciiFile);
//    fclose(binaryFile);
//
//    //Open ASCII file in binary
//    asciiFile = fopen(ascii_file_name.c_str(),"rb");
//    binaryFile = fopen(binary_file_name.c_str(),"rb");
    
    printf("binary file %s size = %llu\n",binary_file_name.c_str(),binaryFileSize);
//    printf("ascii file %s size = %llu\n",ascii_file_name.c_str(),asciiFileSize);

//    float compression = (float)(asciiFileSize - binaryFileSize) / asciiFileSize;
//    printf("\ncompression = %f\n",compression);

    fclose(binaryFile);
    if(writeASCII)
        fclose(asciiFile);
    
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
    return asciiFile != 0;
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
    
//    if(asciiFile == NULL || binaryFile == NULL) {
//        throw "1 or more files are not open for writing.";
//    }
	bool writeIntersections = true;
    if(binaryFile == NULL) {
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
            fprintf(asciiFile,"%f,%f,%f,%f,%f,[%f,%f,%f],%hu",data.imageX,data.imageY,data.lensU,data.lensV,data.timestamp, throughputRGB[0],throughputRGB[1],throughputRGB[2],nrIsects);
        }
        
        fwrite(&data.imageX, sizeof(float), 1, binaryFile);
        fwrite(&data.imageY, sizeof(float), 1, binaryFile);
        fwrite(&data.lensU, sizeof(float), 1, binaryFile);
        fwrite(&data.lensV, sizeof(float), 1, binaryFile);
        fwrite(&data.timestamp, sizeof(float), 1, binaryFile);
        fwrite(&throughputRGB, sizeof(float), 3, binaryFile);
        fwrite(&nrIsects, sizeof(unsigned short), 1, binaryFile);
        
        //Update the counters
        ++pathsWritten;
        if(writeIntersections) {
			if(writeASCII) {
				if(nrIsects == 0) {
					fputs("\n", asciiFile);
					continue;
				}
				else fputs(",{",asciiFile);
			}
			
			for(ushort i = 0 ; i < nrIsects ; ++i) {
				IntersectData isectData = data.intersectionData[i];
				
				float positionFloat[] = { isectData.position.x, isectData.position.y, isectData.position.z };
				
				fprintf(asciiFile,"[%f,%f,%f]",positionFloat[0],positionFloat[1],positionFloat[2]);
				if(i != nrIsects - 1) {
					fputs(",",asciiFile);
				}
				fwrite(&positionFloat,sizeof(float),3,binaryFile);
			}
			fputs("},",asciiFile);
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
	//            isectData->primitiveId = 4;

	//            char buffer[100];
	//            sprintf(buffer,"%hu,",isectData->primitiveId);
	//            fprintf(asciiFile,buffer);
				if(writeASCII)
					fprintf(asciiFile, "%hu,",isectData.primitiveId);
				fwrite(&isectData.primitiveId,sizeof(unsigned short),1,binaryFile); //2 bytes
			}
			//fprintf(asciiFile,"},");

			//write shape ids, size * integers
			//fprintf(asciiFile,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
	//            isectData->shapeId = 5;

	//            char buffer[100];
	//            sprintf(buffer,"%hu,",isectData->shapeId);
				if(writeASCII)
					fprintf(asciiFile, "%hu,",isectData.shapeId);
				fwrite(&isectData.shapeId,sizeof(unsigned short),1,binaryFile); //2 bytes
			}
			//fprintf(asciiFile,"},");

			//write color spectra size * float[nSamples]
			fprintf(asciiFile,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
				const RGBSpectrum &s = isectData.spectrum.ToRGBSpectrum();
				float color[3];
				s.ToRGB(color);
	//				const float* samples = color->GetSamples();

	//            char buffer[100];
	//            if(i != data->size() - 1) {
	//                sprintf(buffer,"[%f,%f,%f],",color[0],color[1],color[2]);
	//            }
	//            else
	//                sprintf(buffer,"[%f,%f,%f]},",color[0],color[1],color[2]);
	//            fprintf(asciiFile,buffer);
				if(writeASCII) {
					if(i != nrIsects - 1) {
						fprintf(asciiFile,"[%f,%f,%f],",color[0],color[1],color[2]);
					}
					else
						fprintf(asciiFile,"[%f,%f,%f]},",color[0],color[1],color[2]);
				}

				fwrite(&color,sizeof(float),3,binaryFile); //12 bytes
			}
			//fprintf(asciiFile,"},");

			//write interaction types, size * unsigned short
			//fprintf(asciiFile,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];
				if(writeASCII)
					fprintf(asciiFile,"%hu,",isectData.interactionType);
				fwrite(&isectData.interactionType,sizeof(unsigned short),1,binaryFile); //2 bytes
			}
			//fprintf(asciiFile,"},");

			//Write light ids size * unsigned short (2N)
			//fprintf(asciiFile,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData isectData = data.intersectionData[i];

				if(writeASCII)
					fprintf(asciiFile,"%hu",isectData.lightId);
				fwrite(&isectData.lightId,sizeof(unsigned short),1,binaryFile); //2 bytes
			}
		}
        if(writeASCII)
            fprintf(asciiFile,"\n");
        //Flush the buffers
        fflush(asciiFile);
        fflush(binaryFile);
        
        intersectionsWritten += nrIsects;
    }
    //Clear buffer
    writing = false;
    bufferData.clear();
//    printf("Buffer data size after clearing = %zu\n",bufferData.size());
//    fprintf(asciiFile,"\n");
}
