#include "ParallelCoordsView.h"
#include "DataView.h"
#include "Geometry.h"

#include "DataSet.h"
#include "Table.h"

#include <math.h>
#include <stdio.h>

ParallelCoordsView::ParallelCoordsView(int x, int y, int width, int height, int paddingX, int paddingY) : RIVDataView(x,y,width,height, paddingX, paddingY) {
    linesAreDirty = true;
    axesAreDirty = true;
    selectedAxis = 0;
    //Nothing else to do
}


ParallelCoordsView::~ParallelCoordsView(void) {
	//Additional deleting unique to parallel coords view ?
}

template <typename T>
size_t find(std::vector<T> haystack, T needle) {
    int i = 0 ;
    for(T value : haystack) {
        if(value == needle) {
            return i;
        }
        i++;
    }
    return -1;
}

void ParallelCoordsView::createAxes() {
    axisGroups.clear();
    size_t total_nr_of_records = dataset->TotalNumberOfRecords();
    int y = startY + paddingY;
    int axisHeight = height - 2 * paddingY;
    
    float delta = 1.F / (total_nr_of_records - 1) * (width - 2 * paddingX);

    std::vector<RIVTable*>* tablePointers = dataset->GetTables();
    
    int axisIndex = 0;
    
    for(size_t j = 0 ; j < tablePointers->size() ; j++) {
        RIVTable *table = tablePointers->at(j);
        ParallelCoordsAxisGroup axisGroup;
        size_t numberOfRecords = table->NumberOfColumns();
        for(size_t i = 0 ; i < numberOfRecords ; ++i) {
            RIVRecord* record = table->GetRecord(i);
            RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(record);
            if(floatRecord) {
                int x = delta * (axisIndex) + startX + paddingX;
                
                std::pair<float,float> minMax = floatRecord->MinMax();
                
                ParallelCoordsAxis axis(x,y,axisHeight,minMax.first,minMax.second,record->name);
                
                axis.ComputeScale(4);
                axis.RecordPointer = record;
                
                axisGroup.axes.push_back(axis);
                axisIndex++;
                continue;
            }
            RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(record);
            if(shortRecord) {
                int x = delta * (axisIndex) + startX + paddingX;
                
                const std::pair<ushort,ushort> &minMax = shortRecord->MinMax();
                ParallelCoordsAxis axis(x,y,axisHeight,minMax.first,minMax.second,record->name);
                
                axis.ComputeScale(4);
                axis.RecordPointer = record;
                axisIndex++;
                
                axisGroup.axes.push_back(axis);
            }

        }
        axisGroup.table = table;
        axisGroups.push_back(axisGroup);
    }
}

void ParallelCoordsView::drawAxes() {
    if(axesAreDirty) {
        glColor3f(1,1,1);
        glLineWidth(1.F);

        glBegin(GL_LINES);
        for(ParallelCoordsAxisGroup &axisGroup : axisGroups) {
                for(ParallelCoordsAxis &axis : axisGroup.axes) {
                    glColor3f(0,0,0);
                    glVertex3f(axis.x,axis.y,0);
                    glVertex3f(axis.x,axis.y+axis.height,0);
                }
            }
        glEnd();
        
        glColor3f(1,0,0);
        glLineWidth(1.F);
        
        //Draw selection box
        glBegin(GL_LINE_STRIP);
        if(selectedAxis) {
            Area &selectionBox  = selectedAxis->selection;
            
            glVertex3f(selectionBox.start.x,selectionBox.start.y,0);
            glVertex3f(selectionBox.end.x,selectionBox.start.y,0);
            
            glVertex3f(selectionBox.end.x,selectionBox.end.y,0);
            glVertex3f(selectionBox.start.x,selectionBox.end.y,0);
            
            glVertex3f(selectionBox.start.x,selectionBox.start.y,0);
        }
        
        glEnd();
        
        float textColor[3] = {0,0,0};
        
        //Draw texts
        for(ParallelCoordsAxisGroup &axisGroup : axisGroups) {
            for(ParallelCoordsAxis &axis : axisGroup.axes) {
                std::string text = axis.name;
                
                drawText(text,axis.x,axis.y - 15,textColor,.1F);
            
                std::vector<float> &scale = axis.scale;
                
                //Draw the scales indicators
                for(size_t j = 0 ; j < scale.size() ; j++) {
                    float value = axis.ValueOnScale(scale[j]);
                    int height = axis.PositionOnScale(scale[j]);
                    
                    std::string text = std::to_string(value);
                    
                    char buffer[4];
                    sprintf(buffer,"%.2f",value);
                    
                    drawText(buffer,4,axis.x - 6,height,textColor,.1F);
                }
            }
        }
    }
}

void ParallelCoordsView::drawLines() {
    
    if(linesAreDirty) {
        std::clock_t start;
        double duration;
        
        start = std::clock();
        
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(1.F);
        
//        printf("Redrawing lines\n");

        for(ParallelCoordsAxisGroup &axisGroup : axisGroups) {
            RIVTable *table = axisGroup.table;
            size_t lineIndex = 0;
            size_t numberOfRows = table->NumberOfRows();
            size_t recordIndex = 0;
            
            if(table->GetName() == "intersections") {
                
            }
            
            TableIterator *iterator = table->GetIterator();
            printf("iterator for %s : ", table->GetName().c_str());
            iterator->Print();
            
            
//            printf("%zu rows for table %s\n",numberOfRows,table->GetName().c_str());
//            for(size_t recordIndex = 0 ; recordIndex < numberOfRows ; recordIndex++) {
            while(iterator->HasNext()) {
                recordIndex = iterator->GetNext();
                
//                printf("Record index %zu\n",recordIndex);
                
                glBegin(GL_LINE_STRIP); //Unconnected groups, draw connections later as they are not 1-to-1
                
                float* color = computeColor(lineIndex, numberOfRows);
                glColor3f(color[1], color[1], color[2]);
                for(ParallelCoordsAxis axis : axisGroup.axes) {
                    RIVRecord *ptr = axis.RecordPointer;
                    RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(ptr);
                    if(floatRecord) {
                        //Code here
                        const std::pair<float,float> &min_max = floatRecord->MinMax();
                        
                        float value = floatRecord->Value(recordIndex);
                        float x = axis.x;
                        
                        float heightRatio;
                        if(min_max.second == min_max.first) { //Special case to avoid divide-by-zero
                            heightRatio = .5F;
                        }
                        else heightRatio = (value - min_max.first) / (float)(min_max.second - min_max.first);
                        
                        float y = heightRatio * axis.height + startY + paddingY;
//                        printf("glVertex3f(%f,%f)\n",x,y);
//                        printf("Drawing line %zu\n",lineIndex);
                        glVertex3f(x, y, 0);
                        if(heightRatio < 0 || heightRatio > 1) {
                            
                            //throw new std::string("A line was being drawn outside ot the parallel coordinates view");
                            printf("This should not happen!!!!!\n");
                            printf("Weird results for record %s (x,y) = (%f,%f)\n",floatRecord->name.c_str(),x,y);
                            std::cout << "value = " << value << "\n";
                            printf("END");
                        }
                        continue;
                        
                    }
                    RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(ptr);
                    if(shortRecord) {
                        //Code here
                        const std::pair<unsigned short,unsigned short> &min_max = shortRecord->MinMax();
                        
                        ushort value = shortRecord->Value(recordIndex);
                        
                            float x = axis.x;
                            
                            float heightRatio;
                            if(min_max.second == min_max.first) { //Special case to avoid divide-by-zero
                                heightRatio = .5F;
                            }
                            else heightRatio = (value - min_max.first) / (float)(min_max.second - min_max.first);
                            
                            //                    printf("heighratio = %f\n",heightRatio);
                            float y = heightRatio * axis.height + startY + paddingY;

//                            printf("glVertex3f(%f,%f)\n",x,y);
//                            printf("Drawing line %zu\n",lineIndex);
                            glVertex3f(x, y, 0);
                            if(heightRatio < 0 || heightRatio > 1) {
                                
                                //throw new std::string("A line was being drawn outside ot the parallel coordinates view");
                                printf("This should not happen!!!!!\n");
                                printf("Weird results for record %s (x,y) = (%f,%f)\n",shortRecord->name.c_str(),x,y);
                                std::cout << "value = " << value << "\n";
                                printf("END");
                            }
                    }
                }
                glEnd();
                lineIndex++;
            }

        }
//        linesAreDirty = false;
    }
//    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
//    printf("CLOCKS_PER_SEC=%d\n",CLOCKS_PER_SEC);
//    printf("draw lines took %f\n",duration);
}

void ParallelCoordsView::Draw() {
    //Draw the axes, including text and scales, should be created beforehand
    drawAxes();
        
    //Draw the lines from each axis
    drawLines();
}

float* ParallelCoordsView::computeColor(size_t lineIndex, size_t totalNrOfLines) {
    
//    printf("lineIndex = %zu, totalNrOfLines = %zu\n",lineIndex,totalNrOfLines);
    
	float minColor[] = {1.F,1.F,0.F}; //yellow
	float maxColor[] = {0.F,0.F,1.F}; //blue

	float ratio = (lineIndex) / (float) (totalNrOfLines);
    
//    printf("Ratio color = %f\n",ratio);
    ratio = fmax(fmin(ratio,1.F),0.F);

	float color[3];
	for(size_t i = 0 ; i < 3 ; i++) {
		color[i] = ratio * minColor[i] + (1 - ratio) * maxColor[i];
	}
	return color;
}


void ParallelCoordsView::ComputeLayout() {
//    axesOrder = {"x","y","throughput 1","#intersections","intersection X","intersection Y","intersection Z"};
    createAxes();
}

bool ParallelCoordsView::HandleMouse(int button, int state, int x, int y) {
	if(isDragging || containsPoint(x,y)) {
		//What axis was selected
        if(state == GLUT_DOWN) {
            int proximityMax = 50;
            
            //            printf("(x,y)=(%d,%d)",x,y);
            for(ParallelCoordsAxisGroup &axisGroup : axisGroups) {
                for(ParallelCoordsAxis &axis : axisGroup.axes) {
                    int distanceX = abs(axis.x - x);
                    //                printf("distance=%d\n",distanceX);
                    
                    if(distanceX < proximityMax) {
                        //Close enough..
                        
                        axis.selection.start.x = axis.x - 10;
                        axis.selection.end.x = axis.x + 10;
                        axis.selection.start.y = y;
                        axis.selection.end.y = y;
                        
                        axis.HasSelectionBox = true;
                        selectedAxis = &axis;
                        
                        isDragging = true;
                        axesAreDirty = true;
                        
                        return true;
                    }
                }
            }
            selectedAxis = 0;
        }
        else if(state == GLUT_UP) { //Stop selection dragging
            if(isDragging && selectedAxis != 0) {
                
                Area &selection = selectedAxis->selection;
                
                float lowerBound = selectedAxis->ValueOnScale(selectedAxis->ScaleValueForY(selection.start.y));
                float upperBound = selectedAxis->ValueOnScale(selectedAxis->ScaleValueForY(selection.end.y));
                
                printf("PCView selection lowerbound = %f upperbound = %f\n",lowerBound,upperBound);
                
                Filter* rangeFilter = new RangeFilter(selectedAxis->name,lowerBound,upperBound);
                
                dataset->ClearFilters();
//                dataset->ClearFilter(rangeFilter->attributeName);
                dataset->AddFilter(rangeFilter);
                
                axesAreDirty = true;
                linesAreDirty = true;
                isDragging = false;
            }
            return true;
        }
		//What range was selected
		//How to define this range
		return true;
	}
    return false;
}

bool ParallelCoordsView::HandleMouseMotion(int x, int y) {
    
//    printf("(x,y)=(%d,%d)\n",x,y);
    
    if(isDragging && selectedAxis != 0) {
        
        Area *selection = &selectedAxis->selection;
        
        selection->end.y = selectedAxis->PositionOnScale(selectedAxis->ScaleValueForY(y));
        
        axesAreDirty = true;
        linesAreDirty = true;
        
        return true;
    }
    return false;
}

void ParallelCoordsView::drawText(char *text, int size, int x, int y, float *color, float sizeModifier) {
    //Estimate center, seems to be the magic number for font pixel size
    float xCenter = 60 * sizeModifier * size / 2.F;
    
    glLineWidth(1);
	glColor3f(*color,*(color+1),*(color+2));
    glPushMatrix();
	glTranslatef(x - xCenter,y, 0);
    glPushMatrix();
	glScalef(sizeModifier,sizeModifier,1);
	for(size_t i = 0 ; i < size ; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	}
    glPopMatrix();
    glPopMatrix();

}

void ParallelCoordsView::drawText(std::string text, int x, int y, float *color, float sizeModifier) {
    drawText((char*)text.c_str(),(int)text.size(),x,y,color,sizeModifier);
}
