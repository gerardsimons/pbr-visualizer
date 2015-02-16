//
//  Grid.h
//  embree
//
//  Created by Gerard Simons on 16/02/15.
//
//

#ifndef __embree__Grid__
#define __embree__Grid__

#include <vector>
#include "Geometry/Geometry.h"

class Grid {
private:
    
    unsigned int width;
    unsigned int height;
    
    std::vector<std::vector<bool>> cells;
    
    void initCells();
    void floodFillNeighbors(int x, int y);
    void fillNeighbors(unsigned int x, unsigned int y);
public:
    Grid(unsigned int width,unsigned int height);
    Grid(unsigned int size);
    
    //Return bounding volume for the currently filled cells
    RIVRectangle fillBounds();
    void FloodFill(const RIVPoint& seed);
    void InvertFill();
    unsigned int GetWidth();
    unsigned int GetHeight();
    bool IsFilled(int cellX, int cellY);
    void Clear();
    void FillCell(unsigned int x, unsigned int y);
};

#endif /* defined(__embree__Grid__) */
