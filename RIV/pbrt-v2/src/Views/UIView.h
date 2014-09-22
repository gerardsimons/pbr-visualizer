//
//  UIView.h
//  Afstuderen
//
//  Created by Gerard Simons on 23/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__UIView__
#define __Afstuderen__UIView__

#include "DataView.h"
#include "../AntTweakBar.h"

namespace riv {

enum COLOR_SCHEME {
    LINEAR,
    FIXED,
    RGB
};

class UIView : public RIVDataView {
private:
    TwBar* tweakBar = NULL;
    float tBarX = 0;
    float tBarY = 0;
    const std::string barName = "TweakBar";
    
    bool hasFocus = false;
    
    //tweak bar variables
    COLOR_SCHEME currentScheme = LINEAR;
    float someVar;
    
    static UIView* instance;
public:
    void Reshape(int newWidth,int newHeight);
	void Draw();
    void HandleEntry(int state);
	bool HandleMouse(int button, int state, int x, int y);
	bool HandleMouseMotion(int x, int y);
    void MoveMenu(float,float);
    
    void initTweakBar();
    
    static void DrawInstance(); //Override
    static void ReshapeInstance(int,int);
    static void Mouse(int button, int state, int x, int y);
    static void Entry(int state);
    static void Motion(int x, int y);
    
    UIView(RIVColorProperty* controlledColorProp, RIVSizeProperty* controlledSizeProp);
};
}
#endif /* defined(__Afstuderen__UIView__) */
