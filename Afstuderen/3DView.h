//
//  3DView.h
//  Afstuderen
//
//  Created by Gerard Simons on 19/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen___DView__
#define __Afstuderen___DView__

#include "DataView.h"

class RIV3DView : public RIVDataView {
public:
    RIV3DView(int,int,int,int,int,int);
    virtual void ComputeLayout();
	virtual void Draw();
	virtual bool HandleMouse(int button, int state, int x, int y);
	virtual bool HandleMouseMotion(int x, int y);
    
    //The models to draw

    //The camera
    
    //Anything else?
};

#endif /* defined(__Afstuderen___DView__) */
