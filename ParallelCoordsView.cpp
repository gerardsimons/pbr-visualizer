#include "ParallelCoordsView.h"
#include "DataView.h"
#include <stdio.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

ParallelCoordsView::ParallelCoordsView(int x, int y, int width, int height) : RIVDataView(x,y,width,height) {

}


ParallelCoordsView::~ParallelCoordsView(void)
{
	//Additional deleting unique to parallel coords view
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

	float delta = 1.F / (columns + 1) * width;

	glBegin(GL_LINES);
		for(size_t i = 0 ; i < columns ; i++) {
			float x = delta * (i + 1) + startX;
			//printf("x=%f\n",x);
			glVertex3f(x, startY + marginHeight, 0.0);
			glVertex3f(x, startY + height - marginHeight, 0);


			//Draw text for axis
		}
	glEnd();

	//Draw text for axis
	for(size_t i = 0 ; i < columns ; i++) {
		float x = delta * (i + 1) + startX;
		//printf("x=%f\n",x);
		RIVRecord *record = dataset.GetRecord(i);	
		string text = record->name;
		DrawText(text.c_str(),text.size() + 1,x,startY + marginHeight,textColor,.15F);
	}
	
	//Find the min-max for each axis
	vector<pair<float,float>> minmax_values;
	for(size_t i = 0 ; i < columns ; i++) {
		RIVRecord *record = dataset.GetRecord(i);	
		pair<float,float> min_max = record->MinMax();
		minmax_values.push_back(min_max);
	}

	size_t records_per_column = dataset.NumberOfValuesPerRecord();
	printf("record_per_column = %d\n",records_per_column);

	//Draw the lines
	
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(3.F);

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

			printf("heightratio = %f\n",heightRatio);
			printf("value point (x,y) = (%f,%f)\n",x,y);
			
			glVertex3f(x, y, 0);
		}
		glEnd();
	}

	
	//glTranslatef(width / 2, 3.6, 0);
	//glScalef(0.15, 0.15, 0.15);
	//glColor4f( 0.0, 0.0, 0.0, 1.0 );
	char p[5] = "test";

	DrawText(p,4,width/2,height/2,textColor,.2F);

	//Draw the texts
}

void ParallelCoordsView::DrawText(const char *text, int size, int x, int y, float *color, float sizeModifier) {
	glMatrixMode( GL_MODELVIEW );
	glColor3f(*color,*(color+1),*(color+2));
	glTranslatef(x,y, 0);
	glScalef(sizeModifier,sizeModifier,1);
	for(size_t i = 0 ; i < size ; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *(text+i));
	}
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}
