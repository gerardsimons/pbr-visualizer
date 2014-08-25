//
//  UIView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 23/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "UIView.h"
#include "AntTweakBar.h"

#define NUM_COLOR_TYPES 3

UIView* UIView::instance = NULL;

UIView::UIView(RIVColorProperty *controlledColorProp, RIVSizeProperty* controlledSizeProp) {
    if(instance != NULL) {
        throw "Only 1 instance of UIView allowed.";
    }
    instance = this;
    initTweakBar();
}

//UIView::UIView(int x, int y, int width, int height, int paddingX, int paddingY) : RIVDataView(x,y,width,height,paddingX,paddingY){
//    if(instance != NULL) {
//        throw "Only 1 instance of UIView allowed.";
//    }
//    instance = this;
//    initTweakBar();
//}

void TW_CALL BreakOnError(const char *errorMessage)
{
    printf("\nTW ERROR: %s\n\n",errorMessage);
}

void UIView::initTweakBar() {
//    printf("Init tweak bar\n");
    TwInit(TW_OPENGL, NULL);
    // Initialize AntTweakBar
    tweakBar = TwNewBar("TweakBar");
    TwHandleErrors(BreakOnError);
    
    TwGLUTModifiersFunc(glutGetModifiers);
    TwDefine(" TweakBar size='200 200' color='200 200 200' label='Color Properties'"); // change default tweak bar size and color
    
    // Add the enum variable 'g_CurrentShape' to 'bar'
    // (before adding an enum variable, its enum type must be declared to AntTweakBar as follow)
    {
        // ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
        TwEnumVal colorEV[NUM_COLOR_TYPES] = { {LINEAR, "Linear"}, {FIXED, "Fixed"}, {RGB, "RGB"} };
        // Create a type for the enum shapeEV
        TwType colorScheme = TwDefineEnum("ColorScheme", colorEV, NUM_COLOR_TYPES);
        // add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
        TwAddVarRW(tweakBar, "ColorScheme", colorScheme, &currentScheme, " keyIncr='<' keyDecr='>' help='Change object shape.' ");
    }
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); // Message added to the help bar.
    // Add 'g_Zoom' to 'bar': this is a modifable (RW) variable of type TW_TYPE_FLOAT. Its key shortcuts are [z] and [Z].
    TwAddVarRW(tweakBar, "Zoom", TW_TYPE_FLOAT, &someVar,
               " min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");
    
//    // Add 'g_Rotation' to 'bar': this is a variable of type TW_TYPE_QUAT4F which defines the object's orientation
//    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation,
//               " label='Object rotation' opened=true help='Change the object orientation.' ");
//    
//    // Add callback to toggle auto-rotate mode (callback functions are defined above).
//    TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL,
//               " label='Auto-rotate' key=space help='Toggle auto-rotate mode.' ");
//    
//    // Add 'g_LightMultiplier' to 'bar': this is a variable of type TW_TYPE_FLOAT. Its key shortcuts are [+] and [-].
//    TwAddVarRW(bar, "Multiplier", TW_TYPE_FLOAT, &g_LightMultiplier,
//               " label='Light booster' min=0.1 max=4 step=0.02 keyIncr='+' keyDecr='-' help='Increase/decrease the light power.' ");
//    
//    // Add 'g_LightDirection' to 'bar': this is a variable of type TW_TYPE_DIR3F which defines the light direction
//    TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &g_LightDirection,
//               " label='Light direction' opened=true help='Change the light direction.' ");
}

void positionTweakBar(int x, int y) {
    char buffer[100];
    sprintf(buffer,"TweakBar position='%d %d'",x,y);
    TwDefine(buffer);
    printf("TwDefine(%s)\n",buffer);
}

void resizeTweakBar(int width, int height) {
    char buffer[100];
    sprintf(buffer,"TweakBar size='%d %d'",width,height);
    TwDefine(buffer);
    printf("TwDefine(%s)\n",buffer);
}

void UIView::MoveMenu(float deltaX,float deltaY) {
    tBarX+=deltaX;
    tBarY+=deltaY;
    positionTweakBar(tBarX,tBarY);
}

void UIView::Reshape(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    
    startX = 0;
    startY = 0;
    
    paddingX = 10;
    paddingY = 10;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    
    TwWindowSize(width,height);
//    positionTweakBar(paddingX, paddingY);
//    resizeTweakBar(width-2*paddingX,height-2*paddingY);
}

void UIView::Draw() {
    glDisable(GL_DEPTH_TEST);
    
//    printf("UIView Draw!\n");
//
//    printVar("startX",startX);
//    printVar("startY",startY);
//    printVar("width",width);
//    printVar("height",height);
    
    glClearColor(1.0, 1.0, 1.0, 0);
    glClear( GL_COLOR_BUFFER_BIT );
    
    TwDraw();
    
    glutSwapBuffers();
    
    if(hasFocus) {
        glutPostRedisplay();
    }
};

void UIView::DrawInstance() {
    if(instance != NULL) {
        instance->Draw();
    }
    else {
        printf("No instance to draw.\n");
    }
}

void UIView::HandleEntry(int state) {
    if(state == GLUT_LEFT) {
        hasFocus = false;
    }
    else {
        hasFocus = true;
        glutPostRedisplay();
    }
}

void UIView::Entry(int state) {
    if(instance != NULL) {
        instance->HandleEntry(state);
    }
    printf("No UIView instance exists!\n");
}

void UIView::ReshapeInstance(int newWidth,int newHeight) {
    if(instance != NULL) {
        instance->Reshape(newWidth,newHeight);
    }
    else printf("No instance to reshape.");
}

//static
void UIView::Mouse(int button, int state, int x, int y) {
    if(instance) {
        instance->HandleMouse(button, state, x, y);
    }
    else printf("No instance for mouse.");
}

void UIView::Motion(int x, int y) {
    if(instance) {
        instance->HandleMouseMotion(x, y);
    }
    else printf("No instance for mouse motion.");
}

bool UIView::HandleMouse(int button, int state, int x, int y) {
//    printf("UIView HandleMouse.\n");
    if(!TwEventMouseButtonGLUT(button, state, x,y)) {
        y = height - y;
        if(containsPoint(x, y)) {
            //Tweak bar interaction
            return true;
        }
    }
    return true;
}

bool UIView::HandleMouseMotion(int x, int y) {
//    printf("UIView HandleMouseMotion.\n");
    TwEventMouseMotionGLUT(x, y);
    ToViewSpaceCoordinates(&x, &y);
    if(containsPoint(x, y)) {
        return true;
    }
    return false;
}