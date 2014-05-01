#include "ParallelCoordsView.h"
#include "DataView.h"

#include <stdio.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

ParallelCoordsView::ParallelCoordsView(int x, int y, int width, int height) : RIVDataView(x,y,width,height) {
    //Nothing else to do
}


ParallelCoordsView::~ParallelCoordsView(void) {
	//Additional deleting unique to parallel coords view ?
}

void ParallelCoordsView::Draw() {
	printf("ParallCoordsView::Draw()\n");

	float textColor[3] = {0,0,0};

    //Draw the axes
	glLineWidth(1);
	glColor3f(0.0, 0.0, 0.0);

	size_t columns = dataset.NumberOfRecords();
	int marginHeight = 20;

	printf("columns = %d\n",columns);

	glBegin(GL_LINES);
    for(size_t i = 0 ; i < axes.size() ; i++) {
        ParallelCoordsAxis *axis = &axes[i];
        
        glVertex3f(axis->x, axis->y, 0);
        glVertex3f(axis->x, axis->y + axis->height, 0);
    }
	glEnd();
    
    for(size_t i = 0 ; i < axes.size() ; i++) {
        ParallelCoordsAxis *axis = &axes[i];
        string text = axis->record->name;
        
        glVertex3f(axis->x, axis->y, 0);
        glVertex3f(axis->x, axis->y, 0);
        
        DrawText(text.c_str(),text.size() + 1,axis->x,axis->y - 15,textColor,.1F);
    }
	
	//Find the min-max for each axis
	vector<pair<float,float>> minmax_values;
	for(size_t i = 0 ; i < columns ; i++) {
		RIVRecord *record = dataset.GetRecord(i);	
		pair<float,float> min_max = record->MinMax();
		minmax_values.push_back(min_max);
	}

	size_t records_per_column = dataset.NumberOfValuesPerRecord();
	//printf("record_per_column = %d\n",records_per_column);

	//Draw the lines
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(3.F);
    
    float delta = 1.F / (columns + 1) * width;

	//for each record
	for(size_t record_i = 0 ; record_i < records_per_column ; record_i++) {
		//For each axis/column
		//printf("record_i = %d\n",record_i);
		glBegin(GL_LINE_STRIP);
		for(size_t axis_index = 0 ; axis_index < columns ; axis_index++) {
			//printf("axis_index = %d\n",axis_index);	
			pair<float,float> *min_max = &minmax_values[axis_index];
			float value = dataset.GetRecord(axis_index)->Value(record_i);	

			float x = startX + delta * (axis_index + 1);

			//indicating the ratio of height, where max = 1 and min = 0
			float heightRatio;
			if(min_max->second == min_max->first) { //Special case to avoid divide-by-zero
				heightRatio = .5F;
			}	
			else heightRatio = (value - min_max->first) / (min_max->second - min_max->first); 

			float y = startY + marginHeight + heightRatio * (height - 2 * marginHeight);

//			printf("heightratio = %f\n",heightRatio);
//			printf("value point (x,y) = (%f,%f)\n",x,y);
			
			glVertex3f(x, y, 0);
		}
		glEnd();
	}
}

void ParallelCoordsView::ComputeLayout() {
    axes.clear(); //Remove the old axes
    
    size_t nr_of_axes = dataset.NumberOfRecords();
    int marginHeight = 20;
    
    //Fixed y for all axes
    int y = startY + marginHeight;
    int axisHeight = height - 2 * marginHeight;
    float delta = 1.F / (nr_of_axes + 1) * width;
    
    for(size_t i = 0 ; i < nr_of_axes ; i++) {
        int x = delta * (i + 1) + startX;
        
        RIVRecord* record = dataset.GetRecord(i);
        
        ParallelCoordsAxis axis(x,y,axisHeight,record);
        axes.push_back(axis);
    }
}

void ParallelCoordsView::DrawText(const char *text, int size, int x, int y, float *color, float sizeModifier) {
    
    //Estimate center, seems to be the magic number for font pixel size
    float xCenter = 60 * sizeModifier * size / 2.F;
    
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
