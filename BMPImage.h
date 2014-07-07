/*
 * BMPImage.h
 *
 *  Created on: Jun 6, 2013
 *      Author: gerard
 */

#ifndef BMPIMAGE_H_
#define BMPIMAGE_H_

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class BMPImage {
private:
    bool loadTexture(const char* imagePath, int alpha);
//    bool loaded = false;
public:
    int ID;
    bool dataAllocated;
	BMPImage(const char* imagePath,bool hasAlpha);
    BMPImage();
//    BMPImage(const BMPImage& copyImage);;
    bool hasAlpha;
	int sizeX,sizeY;
	GLubyte *data;
    unsigned short* RGB(int x, int y) const;
	virtual ~BMPImage();
    friend std::ostream& operator<<(std::ostream& os, const BMPImage& c)
    {
        os << c.sizeX << "x" << c.sizeY << " BMPImage with ";
        if(!c.hasAlpha) {
            std::cout << "NO ";
        }
        os << " alpha channel.\n RGB data = ";
//        std::cout.setf(std::ios::hex, std::ios::basefield);
        //    cout << "Hex: " << num << endl;
        
        

        for(int y = 0 ; y < c.sizeY ; y++) {
            std::cout << "[";
            for(int x = 0 ; x < c.sizeX ; x++) {
                unsigned short *rgb = c.RGB(x,y);
                std::cout << "(" << std::to_string(rgb[0]) << "," << std::to_string(rgb[1]) << "," << std::to_string(rgb[2]) << ")" << " ";
            }
            std::cout << "]\n";
        }
                                  //        for(size_t i = 0 ; i < c.sizeX * c.sizeY ; i++) {
//            std::cout << (int)c.data[i] << " ";
//        }
//        std::cout << "]\n";
        
        std::cout.unsetf(std::ios::hex);
        
        return os;
    }
};

#endif /* BMPIMAGE_H_ */
