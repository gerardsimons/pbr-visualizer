//
//  Grid.cpp
//  embree
//
//  Created by Gerard Simons on 16/02/15.
//
//

#include "Grid.h"
#include "helper.h"

#include <algorithm>
#include <stack>

Grid::Grid(const std::vector<std::vector<bool>> cells) : cells(cells) {
    if(cells.size() && cells[0].size()) {
        this->height = cells.size();
        this->width = cells[0].size();
    }
    else throw std::runtime_error("Invalid cells dimensions");
}
Grid::Grid(unsigned int width,unsigned int height, const std::vector<std::vector<bool>> cells) : width(width), height(height), cells(cells) {
    if(cells.size() != width) {
        throw std::runtime_error("cells matrix has wrong width.");
    }
    else if(width > 0 && cells[0].size() != height) {
        throw std::runtime_error("cells matrix has wrong height.");
    }
}
Grid::Grid(unsigned int width,unsigned int height) : width(width), height(height) {
    initCells();
}
Grid::Grid(unsigned int size) : width(size), height(size){
    initCells();
}
void Grid::initCells() {
    cells.resize(height);
    for(unsigned int x = 0 ; x < height ; ++x) {
        cells[x].resize(width);
    }
}
RIVRectangle Grid::FillBounds() {
    
    int lastFilledRow = -1;
    int lastFilledColumn = -1;
    
    int firstFilledRow = -1;
    int firstFilledColumn = -1;
    
    for(int x = 0 ; x < width ; ++x) {
        //        bool emptyColumn = true;
        for(int y = 0 ; y < height ; ++y) {
            
            if(cells[y][x]) {
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
Grid Grid::GetHoles() {
    
    Grid copy = *this;
    RIVRectangle boundingRectangle = copy.FillBounds();
    RIVPoint seedPoint;
    
    if(boundingRectangle.start.x > 0) {
        seedPoint.x = 0;
    }
    else if(boundingRectangle.end.x < GetWidth() - 1) {
        seedPoint.x = GetWidth() - 1;
    }
    if(boundingRectangle.start.y > 0) {
        seedPoint.y = 0;
    }
    else if(boundingRectangle.end.y < GetHeight() - 1) {
        seedPoint.y = GetHeight() - 1;
    }
    copy.FloodFill(seedPoint);
    copy.InvertFill();
    
    return copy;
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
    return cells[cellY][cellX];
}
void Grid::fillNeighbors(unsigned int x, unsigned int y) {
    if(x > 0) {
        cells[y-1][x] = true;
    }
    if(y > 0) {
        cells[y][x-1] = true;
    }
    if(x < width - 1) {
        cells[y+1][x] = true;
    }
    if(y < height - 1) {
        cells[y][x+1] = true;
    }
}
void Grid::FillCell(unsigned int x, unsigned int y) {
//    printf("Fill cell %ud,%ud\n",x,y);
//    if(x <= width && y <= height) {
        cells[y][x] = true;
//        fillNeighbors(x, y);
//    }
//    else {
//        throw std::runtime_error("out of grid bounds");
//    }
}
void Grid::InvertFill() {
    for(unsigned int x = 0 ; x < width ; ++x) {
        for(unsigned int y = 0 ; y < height ; ++y) {
            cells[y][x] = !cells[y][x];
        }
    }
}
//Helper function to fill 4-connected neighbors
void Grid::floodFillNeighbors(int seedX, int seedY) {
    printf("Floodfill neighbours (%d,%d)\n",seedX,seedY);
    
    //Out of bounds check
    std::stack<RIVPoint> stack;
    stack.push(RIVPoint(seedX,seedY));
    
    while (stack.size() > 0)
    {
        RIVPoint p = stack.top();
        stack.pop();
        int x = p.x;
        int y = p.y;
        if (y < 0 || y > height - 1 || x < 0 || x > width - 1)
            continue;
        bool val = cells[y][x];
        if (!val)
        {
            cells[y][x] = true;
            stack.push(RIVPoint(x + 1, y));
            stack.push(RIVPoint(x - 1, y));
            stack.push(RIVPoint(x, y + 1));
            stack.push(RIVPoint(x, y - 1));
        }
    }
}
void Grid::FillHoles() {
    Grid holes = GetHoles();
    *this = *this | holes;
}
void Grid::FloodFill(const RIVPoint &seed) {
    floodFillNeighbors(seed.x, seed.y);
}
void Grid::Print() {
    printf("%d x %d Grid object : \n",width,height);
    
    print2DVector(cells);
}
Grid Grid::operator | (Grid& other) {
    
    if(other.width != width || other.height != height) {
        throw std::runtime_error("Dimensions mismatch");
    }
    Grid result(width,height);
    for(int x = 0 ; x < width ; x++) {
        for(int y = 0 ; y < height ; y++) {
            if(cells[y][x] || other.IsFilled(x, y)) {
                result.FillCell(x, y);
            }
        }
    }
    
    return result;
}
Grid Grid::FromLayout(const std::vector<std::string>& layout) {
    
    if(layout.size()) {
        std::vector<std::vector<bool>> cells = std::vector<std::vector<bool>>(layout.size(),std::vector<bool>(layout[0].size()));
        int x = 0;
        for(const std::string layoutLine : layout) {
            int y = 0;
            for(char c : layoutLine) {
                if(c == 'X') {
                    cells[x][y] = true;
                }
                else if(c == '.') {
                    cells[x][y] = false;
                }
                else throw std::runtime_error("Unknown char");
                y++;
            }
            x++;
        }
        return Grid(cells);
    }
    else throw std::runtime_error("Empty layout");
}
