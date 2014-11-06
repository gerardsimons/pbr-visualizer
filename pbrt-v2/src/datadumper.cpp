#include "core/stdafx.h"
#include "datadumper.h"

#include <stdio.h>
#include <map>

ushort DataDumper::objectIdCounter = 1;

FILE* DataDumper::pathASCII;
FILE* DataDumper::pathBinary;
//File to write data about geometry (object IDs, shape IDs, vertices)
//FILE* DataDumper::geometryFile;

size_t DataDumper::pathsWritten = 0;
size_t DataDumper::intersectionsWritten = 0;

const size_t DataDumper::bufferSize = 100000;
bool DataDumper::writeASCII;
bool DataDumper::writeBinary;
bool DataDumper::writeToDatabase;

std::vector<PathData> DataDumper::bufferData;

std::string DataDumper::ascii_file_name;
std::string DataDumper::binary_file_name;
std::string DataDumper::db_file_name;
sqlite3* DataDumper::db = NULL;

std::vector<DataDumper> DataDumper::instances;

std::map<ushort,ushort> objectPrimitives;

std::mutex DataDumper::bufferLock;
std::mutex DataDumper::fileLock;
bool fileIsLocked = false;
//
template<typename T,typename U>
void printMap(std::map<T,U> map) {
    for(typename std::map<T, U>::const_iterator it = map.begin();
        it != map.end(); ++it)
    {
        std::cout << it->first << " : " << it->second << "\n";
    }
}

void DataDumper::executeSQL(const char* sql) {
	char *zErrMsg = 0;
	int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		printf("SQL = %s : ",sql);
		sqlite3_free(zErrMsg);
		throw "SQL Error";
	}else{
		//		fprintf(stdout, "SQL executed successfully\n");
	}
}

DataDumper::DataDumper(const std::string& imagename, int ID) {

	
    ascii_file_name = (imagename + ".txt").c_str();
    binary_file_name = (imagename + ".bin").c_str();
	
//	geometryFile = fopen((imagename + "_geometry.txt").c_str(),"w");
    
	if(writeASCII) {
        pathASCII = fopen(ascii_file_name.c_str(),"w");
		if(pathASCII == NULL && writeASCII) {
			throw "unable to open file " + ascii_file_name;
		}
	}
	
	if(writeBinary) {
		pathBinary = fopen(binary_file_name.c_str(),"wb");
		if(pathBinary == NULL) {
			throw "unable to open file " + binary_file_name;
		}
	}

//	bufferData.reserve(bufferSize);
	
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
    
void DataDumper::Init(const std::string& imagename, int numDumpers, bool writeASCII_, bool writeBinary_, bool writeToDatabase_) {
	
	writeASCII = writeASCII_;
	writeBinary = writeBinary_;
	writeToDatabase = writeToDatabase_;
	
	if(writeASCII == false && writeBinary == false && writeToDatabase == false) {
		throw "Some writing mode must be picked";
	}
    //printf("DATADUMPER INIT\n");
    if(numDumpers < 1) {
		numDumpers = 4;
    }
	
	if(writeToDatabase){
		//Prepare database

		db_file_name = imagename + ".db";
		
		int rc = sqlite3_open(db_file_name.c_str(), &db);
		
		const char *dropTableIfExists = "DROP TABLE IF EXISTS PATHS";
		executeSQL(dropTableIfExists);
		
		const char *dropIsectTable = "DROP TABLE IF EXISTS INTERSECTIONS";
		executeSQL(dropIsectTable);
		
		//The table creation SQL statements
		const char* createPathsTable = "CREATE TABLE PATHS("  \
		"IMAGE_X           UNSIGNED INTEGER    NOT NULL," \
		"IMAGE_Y           UNSIGNED INTEGER     NOT NULL," \
		"LENS_U				REAL NOT NULL," \
		"LENS_V				REAL NOT NULL," \
		"TIMESTAMP        REAL NOT NULL," \
		"THROUGHPUT_ONE        REAL NOT NULL," \
		"THROUGHPUT_TWO        REAL NOT NULL," \
		"THROUGHPUT_THREE        REAL NOT NULL," \
		"RADIANCE_R        REAL NOT NULL," \
		"RADIANCE_G        REAL NOT NULL," \
		"RADIANCE_B        REAL NOT NULL);";
		
		const char* createIsectTable = "CREATE TABLE INTERSECTIONS("  \
		"PID UNSIGNED BIG INT NOT NULL," \
		"BOUNCE_NR		 UNSIGNED INTEGER NOT NULL," \
		"POS_X           REAL    NOT NULL," \
		"POS_Y           REAL     NOT NULL," \
		"POS_Z           REAL	NOT NULL," \
		"PRIMITIVE_ID    UNSIGNED INTEGER NOT NULL," \
		"SHAPE_ID		 UNSIGNED INTEGER NOT NULL," \
		"OBJECT_ID		 UNSIGNED INTEGER NOT NULL," \
		"SPECTRUM_R        REAL NOT NULL," \
		"SPECTRUM_G        REAL NOT NULL," \
		"SPECTRUM_B        REAL NOT NULL," \
		"INTERACTION_TYPE	UNSIGNED INTEGER NOT NULL," \
		"LIGHT_ID			UNSIGNED INTEGER NOT NULL," \
		"FOREIGN KEY(PID) REFERENCES PATHS(rowid));";
		
		//Create the tables
		executeSQL(createPathsTable);
		executeSQL(createIsectTable);
	}
	
    //Create instances, give each their own unique ID
    for(int i = 0 ; i < numDumpers ; ++i) {
        DataDumper newInstance = DataDumper(imagename, i);
        instances.push_back(newInstance);
    }
}

void DataDumper::Finish() {
    
    //Write what remains in buffer to files
    for(DataDumper& dumper : instances) {
		if(writeASCII || writeBinary) {
			dumper.WriteBufferToFile();
		}
		if(writeToDatabase) {
			dumper.WriteBufferToDB();
		}
    }

	
//    unsigned long long pathASCIISize = ftell(pathASCII);
//    unsigned long long pathBinarySize = ftell(pathBinary);
	
    //printf("Closing file.\n");
//    fclose(pathASCII);
//    fclose(pathBinary);
//
//    //Open ASCII file in binary
//    pathASCII = fopen(ascii_file_name.c_str(),"rb");
//    pathBinary = fopen(binary_file_name.c_str(),"rb");
    
//    printf("binary file %s size = %llu\n",binary_file_name.c_str(),pathBinarySize);
//    printf("ascii file %s size = %llu\n",ascii_file_name.c_str(),pathASCIISize);

//    float compression = (float)(pathASCIISize - pathBinarySize) / pathASCIISize;
//    printf("\ncompression = %f\n",compression);

//    fclose(pathBinary);
    if(pathASCII)
        fclose(pathASCII);
	
	if(writeToDatabase) {
		//Finish up
		sqlite3_close(db);
	}
    
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
//void DataDumper::AddToBuffer(size_t dumperId, const PathData& pathData) {
//
//    if(idTouse >= instances.size())
//        throw "No instance with ID ";
//	
////	printf("Using dumper #%d\n",idTouse);
//    DataDumper& dumper = instances[idTouse];
//	++idTouse;
//	if(idTouse == instances.size()) {
//		idTouse = 0;
//	}
//    dumper.AddToBuffer(pathData);
//}

void DataDumper::AddToBuffer(const PathData& pathData) {
	bufferLock.lock();
	bufferData.push_back(pathData);

	//The buffer is full and there is not some other thread already dumping it into the file
    if(bufferData.size() == bufferSize) {
        //        printf("Buffer size = %zu \n",bufferData.size());

		if(writeBinary || writeASCII)
			WriteBufferToFile();
		else if(writeToDatabase)
			WriteBufferToDB();
//		bufferData.clear();
//		fileLock.unlock();
//		fileIsLocked = false;
//		bufferIndex = 0;
//		bufferData.reserve(bufferSize);
    }
	bufferLock.unlock();
}

double diffclock( clock_t clock1, clock_t clock2 ) {
	
	double diffticks = clock1 - clock2;
	double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );
	
	return diffms;
}


void DataDumper::WriteBufferToDB() {
	if(!db) {
		throw "SQL Database not ready";
	}
	printf("Writing %zu data objects to database\n",bufferData.size());
	
	//The templates used for insertion
	char* insertPathTemplate = "INSERT INTO PATHS (IMAGE_X,IMAGE_Y,LENS_U,LENS_V,TIMESTAMP,THROUGHPUT_ONE,THROUGHPUT_TWO,THROUGHPUT_THREE,RADIANCE_R,RADIANCE_G,RADIANCE_B) " \
	"VALUES (%hu,%hu,%f,%f,%f,%f,%f,%f,%f,%f,%f);";
	
	char* insertIsectTemplate = "INSERT INTO INTERSECTIONS (PID,BOUNCE_NR,POS_X,POS_Y,POS_Z,PRIMITIVE_ID,SHAPE_ID,OBJECT_ID,SPECTRUM_R,SPECTRUM_G,SPECTRUM_B,INTERACTION_TYPE,LIGHT_ID) " \
	"VALUES (%zu,%hu,%f,%f,%f,%hu,%hu,%hu,%f,%f,%f,%hu,%hu);";
	
	size_t insertsDone = 0;
	float percentageIncrement = 1;
	float percentage = 0;
	size_t roundCheck = round(bufferData.size() / (100.F * percentageIncrement));
	
	clock_t start = clock();
	char *zErrMsg = 0;
	
	//VERY important, by starting a transaction we gain a VERY significant performance boost (~ 100x)
	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
	
	for(size_t i = 0 ; i < bufferData.size() ; ++i) {
		size_t path_index = pathsWritten++;
		PathData data = bufferData[i];
		unsigned short nrIsects = (unsigned short) data.intersectionData.size();
		
		//ASCII
		float throughputRGB[3];
		data.throughput.ToRGB(throughputRGB);
		float radianceRGB[3];
		data.radiance.ToRGB(radianceRGB);
		
		char buffer[200];
		sprintf(buffer,insertPathTemplate,(ushort)data.imageX,(ushort)data.imageX,data.lensU,data.lensV,data.timestamp,throughputRGB[0],throughputRGB[1],throughputRGB[2],radianceRGB[0],radianceRGB[1],radianceRGB[2]);
		executeSQL(buffer);
		++insertsDone;
		
		for(ushort i = 0 ; i < nrIsects ; ++i) {
			IntersectData isectData = data.intersectionData[i];
			char buffer[200];
			float spectrumRGB[3];
			isectData.spectrum.ToRGB(spectrumRGB);
			ushort objectID = objectIdForPrimitiveId(isectData.primitiveId);
			sprintf(buffer,insertIsectTemplate,path_index,i+1,isectData.position.x,isectData.position.y,isectData.position.z,isectData.primitiveId,isectData.shapeId,objectID,spectrumRGB[0],spectrumRGB[1],spectrumRGB[2]);
			executeSQL(buffer);
			++insertsDone;
		}
		if(path_index % roundCheck == 0) {
			percentage += percentageIncrement;
			clock_t end = clock();
			double deltaT = diffclock(end,start);
			printf("%.1f%% %.3fs %.1f INSERTS/s\n",percentage,deltaT/1000,insertsDone * (1000 / deltaT));
			start = clock();
			insertsDone = 0;
		}
		intersectionsWritten += nrIsects;
	}
	
	//Put it in the database
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
	
	printf("Done writing buffer to DB\n");
}

int writeMax = 100;
int written = 0;
//Write buffer to file
void DataDumper::WriteBufferToFile() {
    writing = true;
	
//    if(pathASCII == NULL || pathBinary == NULL) {
//        throw "1 or more files are not open for writing.";
//    }
	
//	std::lock_guard<std::mutex> guard(fileMutex);
	fileLock.lock();
	bool writeIntersections = true;
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
		float radianceRGB[3];
		data.radiance.ToRGB(radianceRGB);
        
        if(writeASCII) {
            fprintf(pathASCII,"%f,%f,%f,%f,%f,[%f,%f,%f],[%f,%f,%f],%hu",data.imageX,data.imageY,data.lensU,data.lensV,data.timestamp, throughputRGB[0],throughputRGB[1],throughputRGB[2],radianceRGB[0],radianceRGB[1],radianceRGB[2],nrIsects);
        }
        
        fwrite(&data.imageX, sizeof(float), 1, pathBinary);
        fwrite(&data.imageY, sizeof(float), 1, pathBinary);
        fwrite(&data.lensU, sizeof(float), 1, pathBinary);
        fwrite(&data.lensV, sizeof(float), 1, pathBinary);
        fwrite(&data.timestamp, sizeof(float), 1, pathBinary);
        fwrite(&throughputRGB, sizeof(float), 3, pathBinary);
		fwrite(&radianceRGB, sizeof(float), 3, pathBinary);
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
				
				if(writeASCII) {
					fprintf(pathASCII,"[%f,%f,%f]",positionFloat[0],positionFloat[1],positionFloat[2]);
					if(i != nrIsects - 1) {
						fputs(",",pathASCII);
					}
				}
				fwrite(&positionFloat,sizeof(float),3,pathBinary);
			}
			if(writeASCII) {
				fputs("},",pathASCII);
	
			}
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
				IntersectData& isectData = data.intersectionData[i];
	//            isectData->shapeId = 5;

	//            char buffer[100];
	//            sprintf(buffer,"%hu,",isectData->shapeId);
				if(writeASCII)
					fprintf(pathASCII, "%hu,",isectData.shapeId);
				fwrite(&isectData.shapeId,sizeof(unsigned short),1,pathBinary); //2 bytes
			}
			
			//Write the objectId created using ObjectInstance
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData& isectData = data.intersectionData[i];
				//Find the object ID for this primitive ID
				ushort objectID = objectIdForPrimitiveId(isectData.primitiveId);
				if(writeASCII)
					fprintf(pathASCII, "%hu,",objectID);
				fwrite(&objectID,sizeof(unsigned short),1,pathBinary); //2 bytes
			}

			//write color spectra size * float[nSamples]
			if(writeASCII)
				fprintf(pathASCII,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData& isectData = data.intersectionData[i];
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
			if(writeASCII)
				fprintf(pathASCII,"{");
			for(size_t i = 0 ; i < nrIsects ; i++) {
				IntersectData& isectData = data.intersectionData[i];
				const RGBSpectrum &s = isectData.throughput.ToRGBSpectrum();
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
	bufferData.clear();
	fileLock.unlock();
    writing = false;
//    printf("Buffer data size after clearing = %zu\n",bufferData.size());
//    fprintf(pathASCII,"\n");
}

ushort DataDumper::objectIdForPrimitiveId(ushort primitiveId) {
	ushort objectId = objectPrimitives[primitiveId];
	if(objectId == 0) {
		printf("*** WARNING : No object ID for primitive ID %hu ***\n",primitiveId);
		throw "No object ID for primitive ID\n";
	}
	return objectId;
}

void DataDumper::AddObjectPrimitiveMapping(ushort primitiveId) {
//	printf("New object to primitve mapping: \n");
	objectPrimitives[primitiveId] = objectIdCounter;

//	printf("primitives - object map: ");
	printMap(objectPrimitives);
	++objectIdCounter;
}