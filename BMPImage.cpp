/*
 * BMPImage.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: gerard
 */

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>

#include "BMPImage.h"


BMPImage::BMPImage(const char* imagePath, bool hasAlpha) {
    this->hasAlpha = hasAlpha;
	loadTexture(imagePath, hasAlpha);
	// TODO Auto-generated constructor stub
}

BMPImage::~BMPImage() {
	// TODO Auto-generated destructor stub
    delete data;
}

bool BMPImage::loadTexture(const char* imagepath, int alpha)
{   
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3

	FILE * file = fopen(imagepath,"rb");
	if (!file)                              {
        printf("Image %s could not be opened\n",imagepath);
        throw "BMP file could not be opened.\n";
        return 0;
    }

	if ( fread(header, 1, 54, file)!=54 )
	{ // If not 54 bytes read : problem
	    printf("Not a correct BMP file\n");
        throw "Not a correct BMP file\n";
	    return false;
	}

    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        throw "Not a correct BMP file\n";
        return 0;
    }

    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);

    sizeX      = *(int*)&(header[0x12]);
    sizeY     = *(int*)&(header[0x16]);
    if(sizeY < 0) {
        sizeY = -sizeY;
    }
    
    if(sizeX < 0) {
        throw "image has invalid width value";
    }
    if(sizeY < 0) {
        throw "image has invalid height value";
    }

    // Some BMP files are misformatted, guess missing information
    int channels = 3 + alpha;
    printf("Channels found: %i\n",channels);
    imageSize=sizeX*sizeY*(3+alpha); // 3 : one byte for each Red, Green and Blue (and Alpha) component
    if (dataPos==0)      dataPos=500; // The BMP header is done that way

    // Create a buffer
    data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    printf("bytes read = %zu\n",sizeof(data));
    printf("Succesfully read BMPImage\n");
    return true;
}

short* BMPImage::RGB(int x, int y) const {
    int channels = 3 + hasAlpha;
    short rgb[3];
    int pixelPosition = (x * sizeX + y) * channels;
    rgb[2] = data[pixelPosition];
    rgb[1] = data[pixelPosition + 1];
    rgb[0] = data[pixelPosition + 2];
    
    return rgb;
}