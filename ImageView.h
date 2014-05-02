//
//  ImageView.h
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__ImageView__
#define __Afstuderen__ImageView__

#include <GLUT/GLUT.h>
#include "DataView.h"

class RIVImageView : public RIVDataView {
public:
    RIVImageView(char* filename, int x, int y, int width, int height);
    ~RIVImageView();
    void Draw(); //Override
    void ComputeLayout();
private:
    GLuint imageTexture;
    
};

#endif /* defined(__Afstuderen__ImageView__) */
