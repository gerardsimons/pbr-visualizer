//
//  Grid.cpp
//  embree
//
//  Created by Gerard Simons on 16/02/15.
//
//

#include "Grid.h"
#include <algorithm>

Grid::Grid(unsigned int width,unsigned int height) : width(width), height(height) {
    initCells();
}
Grid::Grid(unsigned int size) : width(size), height(size){
    initCells();
}
void Grid::initCells() {
    cells.resize(width);
    for(unsigned int x = 0 ; x < width ; ++x) {
        cells[x].resize(height);
    }
}
RIVRectangle Grid::fillBounds() {
    
    int lastFilledRow = -1;
    int lastFilledColumn = -1;
    
    int firstFilledRow = -1;
    int firstFilledColumn = -1;
    
    for(int x = 0 ; x < width ; ++x) {
        //        bool emptyColumn = true;
        for(int y = 0 ; y < height ; ++y) {
            
            if(cells[x][y]) {
                if(firstFilledRow == -1) {
                    firstFilledRow = x;
                    firstFilledColumn = y;
                }
                lastFilledRow = x;
                lastFilledColumn = std::max(lastFilledColumn,y);
            }
            
        }
    }
    
    return RIVRectangle(firstFilledRow,firstFilledColumn,lastFilledRow,lastFilledColumn);
}
unsigned int Grid::GetWidth() {
    return width;
}
unsigned int Grid::GetHeight() {
    return height;
}
void Grid::Clear() {
    for(unsigned int x = 0 ; x < width ; ++x) {
        for(unsigned int y = 0 ; y < height ; ++y) {
            cells[x][y] = false;
        }
    }
}
bool Grid::IsFilled(int cellX, int cellY) {
    return cells[cellX][cellY];
}
void Grid::fillNeighbors(unsigned int x, unsigned int y) {
    if(x > 0) {
        cells[x-1][y] = true;
    }
    if(y > 0) {
        cells[x][y-1] = true;
    }
    if(x < width - 1) {
        cells[x+1][y] = true;
    }
    if(y < height - 1) {
        cells[x][y+1] = true;
    }
}
void Grid::FillCell(unsigned int x, unsigned int y) {
//    printf("Fill cell %ud,%ud\n",x,y);
    if(x <= width && y <= height) {
        cells[x][y] = true;
        fillNeighbors(x, y);
    }
    else {
        throw std::runtime_error("out of grid bounds");
    }
}
void Grid::InvertFill() {
    for(unsigned int x = 0 ; x < width ; ++x) {
        for(unsigned int y = 0 ; y < height ; ++y) {
            cells[x][y] = !cells[x][y];
        }
    }
}
//Helper function to fill 4-connected neighbors
void Grid::floodFillNeighbors(int x, int y) {
    if(cells[x][y]) {
        return;
    }
    else {
        cells[x][y] = true;
    }
    if(x > 0) {
        floodFillNeighbors(x - 1, y);
    }
    if(y > 0) {
        floodFillNeighbors(x, y-1);
    }
    if(x < width - 1) {
        floodFillNeighbors(x + 1, y);
    }
    if(y < height - 1) {
        floodFillNeighbors(x, y + 1);
    }
}
void Grid::FloodFill(const RIVPoint &seed) {
    floodFillNeighbors(seed.x, seed.y);
}
