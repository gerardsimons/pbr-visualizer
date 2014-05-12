#include "ParallelCoordsView.h"
#include "DataView.h"
#include "Geometry.h"

#include <stdio.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

ParallelCoordsView::ParallelCoordsView(int x, int y, int width, int height, int paddingX, int paddingY) : RIVDataView(x,y,width,height, paddingX, paddingY) {
    selectedAxisIndex = -1;
    //Nothing else to do
}


ParallelCoordsView::~ParallelCoordsView(void) {
	//Additional deleting unique to parallel coords view ?
}

void ParallelCoordsView::createAxes() {
    axes.clear();
    size_t total_nr_of_records = dataset->TotalNumberOfRecords();
    size_t nr_of_float_records = dataset->NumberOfFloatRecords();
    size_t nr_of_short_records = dataset->NumberOfShortRecords();
    int y = startY + paddingY;
    int axisHeight = height - 2 * paddingY;
    float delta = 1.F / (total_nr_of_records - 1) * (width - 2 * paddingX);
    for(size_t i = 0 ; i < nr_of_float_records ; i++) {
        int x = delta * i + startX + paddingX;
        
        RIVRecord<float> &record = dataset->GetFloatRecord(i);
        
        std::pair<float,float> &minMax = record.MinMax();
        
        ParallelCoordsAxis axis(x,y,axisHeight,minMax.first,minMax.second,record.name);
        
		axis.ComputeScale(4);
        
        axes.push_back(axis);
    }
    
    
    /**                 FOR ALL SHORT RECORDS               **/
    for(size_t j = 0 ; j < nr_of_short_records ; j++) {
        int x = delta * (j + nr_of_float_records) + startX + paddingX;
        
        RIVRecord<unsigned short> &record = dataset->GetUnsignedShortRecord(j);
        
        std::pair<unsigned short,unsigned short> &minMax = record.MinMax();
        ParallelCoordsAxis axis(x,y,axisHeight,minMax.first,minMax.second,record.name);
        
		axis.ComputeScale(4);
        
        axes.push_back(axis);
    }
}

void ParallelCoordsView::drawAxes() {
    

//    printf("%p\n",selectedAxis);
    printf("Drawing axes.");
    
    glLineWidth(1.F);

    glBegin(GL_LINES);
        for(size_t i = 0 ; i < axes.size() ; ++i) {
            ParallelCoordsAxis &axis = axes[i];
            glColor3f(0,0,0);
            glVertex3f(axis.x,axis.y,0);
            glVertex3f(axis.x,axis.y+axis.height,0);
        }
    glEnd();
    
    glColor3f(1,0,0);
    glLineWidth(5.F);
    
    //Draw selection box
    glBegin(GL_LINE_STRIP);
    if(selectedAxisIndex != -1) {
//        
//        printf("Drawing selection box\n");
        
        Area &selectionBox = axes[selectedAxisIndex].selection;
        
//        printf("selection (x,y) = (%d,%d)\n",selectionBox.start.x,selectionBox.start.y);
//        printf("selection (endX,endY) = (%d,%d)\n",selectionBox.end.x,selectionBox.end.y);
        
        glVertex3f(selectionBox.start.x,selectionBox.start.y,0);
        glVertex3f(selectionBox.end.x,selectionBox.start.y,0);
        
        glVertex3f(selectionBox.end.x,selectionBox.end.y,0);
        glVertex3f(selectionBox.start.x,selectionBox.end.y,0);
        
        glVertex3f(selectionBox.start.x,selectionBox.start.y,0);
    }
    
    glEnd();
    
    float textColor[3] = {0,0,0};
    
    //Draw texts
    for(ParallelCoordsAxis &axis : axes) {
        std::string text = axis.name;
        
        drawText(text.c_str(),text.size() + 1,axis.x,axis.y - 15,textColor,.1F);
    
        std::vector<float> scale = axis.scale;
        
        //Draw the scales indicators
        for(size_t j = 0 ; j < scale.size() ; j++) {
            float value = axis.ValueOnScale(scale[j]);
            int height = axis.PositionOnScale(scale[j]);
            char text[150]; 
            sprintf(text,"%.2f",value);
            drawText(text,10,axis.x - 6,height,textColor,.1F);
        }
    }
}

void ParallelCoordsView::drawLines() {
    
    std::clock_t start;
    double duration;
    
    start = std::clock();
    
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(1.F);
    
    size_t lineIndex = 0;
    size_t totalNumberOfLines = dataset->NumberOfValuesPerRecord();
    size_t records_per_column = dataset->NumberOfValuesPerRecord();
    
    /**             FOR EACH FLOAT RECORD           **/
    for(size_t record_i = 0 ; record_i < records_per_column ; record_i++) {
        float* color = computeColor(lineIndex,totalNumberOfLines);
        glColor3f(color[0],color[1],color[2]);
        
        glBegin(GL_LINE_STRIP);
        
        for(size_t axis_index = 0 ; axis_index < dataset->NumberOfFloatRecords() ; axis_index++) {
            
            ParallelCoordsAxis &axis = axes[axis_index];
            
            std::pair<float,float> &min_max = dataset->GetFloatRecord(axis_index).MinMax();
            float* value = dataset->GetFloatRecordValue(axis_index,record_i);
            
            if(value != 0) {
                float x = axis.x;
                
                float heightRatio;
                if(min_max.second == min_max.first) { //Special case to avoid divide-by-zero
                    heightRatio = .5F;
                }
                else heightRatio = (*value - min_max.first) / (min_max.second - min_max.first);
                
                float y = heightRatio * axis.height + startY + paddingY;
//                printf("x=%f\n",x);
//                printf("y=%f\n",y);
                glVertex3f(x, y, 0);
            }
        }
        for(size_t axis_index = 0 ; axis_index < dataset->NumberOfShortRecords() ; axis_index++) {
            
            ParallelCoordsAxis *axis = &axes[axis_index + dataset->NumberOfFloatRecords()];
            std::pair<unsigned short,unsigned short> &min_max = dataset->GetUnsignedShortRecord(axis_index).MinMax();
            
            unsigned short* value = dataset->GetShortRecordValue(axis_index,record_i);
            
            if(value != 0) {
                
//                printf("value=%hu\n",*value);
                float x = axis->x;
                
                float heightRatio;
                if(min_max.second == min_max.first) { //Special case to avoid divide-by-zero
                    heightRatio = .5F;
                }
                else heightRatio = (*value - min_max.first) / (float)(min_max.second - min_max.first);
                
                //                    printf("heighratio = %f\n",heightRatio);
                float y = heightRatio * axis->height + startY + paddingY;
//                printf("x=%f\n",x);
//                printf("y=%f\n",y);
                glVertex3f(x, y, 0);
            }
        }
        
        ++lineIndex;
        glEnd();
    }
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
//    
//    printf("CLOCKS_PER_SEC=%d\n",CLOCKS_PER_SEC);
//    printf("draw lines took %f\n",duration);
}

void ParallelCoordsView::Draw() {

	if(needsRedraw) {
        
        //Draw the axes, including text and scales, should be created a priori!
        drawAxes();
        
        //Draw the lines from each axis
        drawLines();
        
//		needsRedraw = false; //TODO: This does not work when losing and regaining focus!
	}
}

float* ParallelCoordsView::computeColor(size_t lineIndex, size_t totalNrOfLines) {
	float minColor[] = {1.F,1.F,0.F}; //yellow
	float maxColor[] = {0.F,0.F,1.F}; //blue

	float ratio = (lineIndex + 1) / (float) (totalNrOfLines + 1);

	float color[3];
	for(size_t i = 0 ; i < 3 ; i++) {
		color[i] = ratio * minColor[i] + (1 - ratio) * maxColor[i];
	}
	return color;
}


void ParallelCoordsView::ComputeLayout() {
    createAxes();
}

bool ParallelCoordsView::HandleMouse(int button, int state, int x, int y) {
	if(isDragging || containsPoint(x,y)) {
		//What axis was selected
        if(state == GLUT_DOWN) {
            int proximityMax = 50;
            
            printf("(x,y)=(%d,%d)",x,y);
            
            
            
            for(size_t i = 0 ; i < axes.size() ; ++i) {
                
                ParallelCoordsAxis *axis = &axes[i];
                
                int distanceX = abs(axis->x - x);
                
                printf("distance=%d\n",distanceX);
                
                if(distanceX < proximityMax) {
                    //Close enough..

                    axis->selection.start.x = axis->x - 10;
                    axis->selection.end.x = axis->x + 10;
                    axis->selection.start.y = y;
                    axis->selection.end.y = y;
                    
                    printf("selection (startX,startY) = (%d,%d)\n",axis->selection.start.x,axis->selection.start.y);
                    printf("selection (endX,endY) = (%d,%d)\n",axis->selection.end.x,axis->selection.end.y);
                    
                    axis->HasSelectionBox = true;
                    selectedAxisIndex = i;
                    
                    printf("Created axis selection!\n");
                    
                    isDragging = true;
                    
                    return true;
                }
            }
        }
        else if(state == GLUT_UP) { //Stop selection dragging
//            printf("GLUT UP \n");
            if(isDragging && selectedAxisIndex != -1) {
                
//                printf("Stop dragging!\n");
                
                ParallelCoordsAxis &selectedAxis = axes[selectedAxisIndex];
                Area &selection = selectedAxis.selection;
                
                float lowerBound = selectedAxis.ValueOnScale(selectedAxis.ScaleValueForY(selection.start.y));
                float upperBound = selectedAxis.ValueOnScale(selectedAxis.ScaleValueForY(selection.end.y));
                
                printf("lowerbound = %f\n",lowerBound);
                printf("upperbound = %f\n",upperBound);
                
                Filter* rangeFilter = new RangeFilter(selectedAxis.name,lowerBound,upperBound);
                
                dataset->ClearFilters();
                dataset->AddFilter(rangeFilter);
                
//                printf("selection (x,y) = (%d,%d)\n",selection.start.x,selection.start.y);
//                printf("selection (endX,endY) = (%d,%d)\n",selection.end.x,selection.end.y);
                
                
                
                
            }
            isDragging = false;
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
    
    if(isDragging && selectedAxisIndex != -1) {
        
        printf("Drag selection box on axis #%d\n",selectedAxisIndex);
        
        
        ParallelCoordsAxis *selectedAxis = &axes[selectedAxisIndex];
        Area *selection = &selectedAxis->selection;
        
        
        selection->end.y = selectedAxis->PositionOnScale(selectedAxis->ScaleValueForY(y));
        
        printf("selection (x,y) = (%d,%d)\n",selection->start.x,selection->start.y);
        printf("selection (endX,endY) = (%d,%d)\n",selection->end.x,selection->end.y);
        

        return true;
    }
    return false;
}

void ParallelCoordsView::drawText(const char *text, int size, int x, int y, float *color, float sizeModifier) {
    
    //Estimate center, seems to be the magic number for font pixel size
    float xCenter = 60 * sizeModifier * size / 2.F;
    
//    printf("drawing chars = %s\n",text);
    
    glLineWidth(1);
	glColor3f(*color,*(color+1),*(color+2));
    glPushMatrix();
	glTranslatef(x - xCenter,y, 0);
    glPushMatrix();
	glScalef(sizeModifier,sizeModifier,1);
	for(size_t i = 0 ; i < size ; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *(text+i));
	}
    glPopMatrix();
    glPopMatrix();
}
