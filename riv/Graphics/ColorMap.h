//
//  ColorMap.h
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 08/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_DO_NOT_DELETE__ColorMap__
#define __afstuderen_DO_NOT_DELETE__ColorMap__

//#include <stdio.h>
#include <vector>

typedef unsigned short ushort;

namespace riv {
    
    class Color {
    public:
        float R;
        float G;
        float B;
        float A;
        
        Color() {};
        Color(float R, float G, float B) : R(R), G(G), B(B), A(1) {
        }
        Color(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {
        }
        //0 to 256
        Color(unsigned short R, unsigned short G, unsigned short B) : R(R / 255.F),G(G / 255.F),B(B / 255.F),A(1){
            
        }
        bool operator==(const Color& rightHand) {
            return R == rightHand.R && G == rightHand.G && B == rightHand.B && A == rightHand.A;
        }
        const float operator[] (const int nIndex) {
            if(nIndex < 3) {
                switch (nIndex) {
                    case 0:
                        return R;
                        break;
                    case 1:
                        return G;
                        break;
                    case 2:
                        return B;
                        break;
                }
            }
            else throw std::runtime_error("Out of bounds");
        }
    };
    
    //A linearly interpolated color map
    class ColorMap {
    private:
        std::vector<Color> colorTable;
        
        float min = 0;
        float max = 1;
    public:
        ColorMap() {
            
        }
        ColorMap(std::vector<Color>& colors);
        ColorMap(std::vector<Color>& colors, float minRange, float maxRange);
        ColorMap(float minRange, float maxRange);
        void Invert();
        
        //Methods
        void SetRange(float minRange,float maxRange);
        Color ComputeColor(float ratio);
        void AddColor(const Color& newColor);
    };
    
    class DiscreteColorMap {
    private:
//        std::map<ushort,
        std::vector<Color> colorTable;
    public:
        Color ComputeColor(ushort value);
        void AddColor(const Color& newColor);
    };
    
}

#endif /* defined(__afstuderen_DO_NOT_DELETE__ColorMap__) */
