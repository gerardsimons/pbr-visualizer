//
//  helper.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 16/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "helper.h"

float* linearInterpolateColor(float ratio, const float colorOne[3],const float colorTwo[3]) {
    float color[3];
    for(int i = 0 ; i < 3 ; i++) {
        color[i] = ratio * colorOne[i] + (1-ratio) * colorTwo[i];
    }
    return color;
}

void copy_buffer()
{
    static GLint viewport[4];
    static GLfloat raster_pos[4];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    /* set source buffer */
    glReadBuffer(GL_FRONT); //red
//    glDrawBuffer(GL_BACK); //green
    
    /* set projection matrix */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity() ;
    gluOrtho2D(0, viewport[2], 0, viewport[3]);
    
    /* set modelview matrix */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* save old raster position */
    glGetFloatv(GL_CURRENT_RASTER_POSITION, raster_pos);
    
    /* set raster position */
    glRasterPos4f(0.0, 0.0, 0.0, 1.0);
    
    /* copy buffer */
    glCopyPixels(0, 0, viewport[2], viewport[3], GL_COLOR);
    
    /* restore old raster position */
    glRasterPos4fv(raster_pos);
    
    /* restore old matrices */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* restore source buffer */
//    glReadBuffer(GL_BACK);
    glReadBuffer(GL_FRONT); //red
}

void copy_buffer_back_to_front()
{
    static GLint viewport[4];
    static GLfloat raster_pos[4];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    /* set source buffer */
    glReadBuffer(GL_BACK);
    
    /* set projection matrix */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity() ;
    gluOrtho2D(0, viewport[2], 0, viewport[3]);
    
    /* set modelview matrix */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* save old raster position */
    glGetFloatv(GL_CURRENT_RASTER_POSITION, raster_pos);
    
    /* set raster position */
    glRasterPos4f(0.0, 0.0, 0.0, 1.0);
    
    /* copy buffer */
    glCopyPixels(0, 0, viewport[2], viewport[3], GL_COLOR);
    
    /* restore old raster position */
    glRasterPos4fv(raster_pos);
    
    /* restore old matrices */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* restore source buffer */
    glReadBuffer(GL_FRONT);
}
