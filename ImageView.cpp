//
//  ImageView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ImageView.h"
#include "BMPImage.h"

RIVImageView::RIVImageView(char* filename, int x, int y, int width, int height) : RIVDataView(x,y,width,height) {
    BMPImage image(filename,true);
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.width, image.height,GL_RGBA, GL_UNSIGNED_BYTE, image.data);
}

void RIVImageView::Draw() {
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    
    //remember all states of the GPU
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3f(1,1,1);
    glNormal3d(0, 0, 1);
    glEnable(GL_TEXTURE_2D);
    
    // texture addition
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f);
        glVertex3f(startX,startY+height,1);
        glTexCoord2f(0.0f,0.0f);
        glVertex3f(startX,startY,1);
        glTexCoord2f(1.0f,0.0f);
        glVertex3f(startX+width,startY,1);
        glTexCoord2f(1.0f,1.0f);
        glVertex3f(startX+width,startY+height,1);
    glEnd();
    //reset to previous state
    glPopAttrib();
    
    
	glDisable(GL_TEXTURE_2D);
}

void RIVImageView::ComputeLayout() { /* Nothing to do */ }
