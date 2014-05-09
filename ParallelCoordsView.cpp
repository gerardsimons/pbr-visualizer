#include "ParallelCoordsView.h"
#include "DataView.h"

#include <stdio.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

ParallelCoordsView::ParallelCoordsView(int x, int y, int width, int height, int paddingX, int paddingY) : RIVDataView(x,y,width,height, paddingX, paddingY) {
    //Nothing else to do
}


ParallelCoordsView::~ParallelCoordsView(void) {
	//Additional deleting unique to parallel coords view ?
}

void ParallelCoordsView::Draw() {

	if(needsRedraw) {
		//printf("ParallCoordsView::Draw()\n");

		float textColor[3] = {0,0,0};

		//Draw the axes
		glLineWidth(1);
		glColor3f(0.0, 0.0, 0.0);

		size_t columns = dataset->NumberOfRecords();
		int marginHeight = 20;

		
		//Draw the vertical lines of the axis
		glBegin(GL_LINES);
		for(size_t i = 0 ; i < axes.size() ; i++) {
			ParallelCoordsAxis *axis = &axes[i];
        
			glVertex3f(axis->x, axis->y, 0);
			glVertex3f(axis->x, axis->y + axis->height, 0);

            std::vector<float> scale = axis->scale;

			//Draw the scales indicator
			for(size_t j = 0 ; j < scale.size() ; j++) {
				int height = axis->PositionOnScale(scale[j]);
				glVertex3f(axis->x,height,0);
				glVertex3f(axis->x + 20,height,0);
			}
		}
		glEnd();
    
		//Draw texts
		for(size_t i = 0 ; i < axes.size() ; i++) {
			ParallelCoordsAxis *axis = &axes[i];
            std::string text = axis->record->name;
        
			DrawText(text.c_str(),text.size() + 1,axis->x,axis->y - 15,textColor,.1F);

            std::vector<float> scale = axis->scale;

			//Draw the scales indicator
			for(size_t j = 0 ; j < scale.size() ; j++) {
				float value = axis->ValueOnScale(scale[j]);				
				int height = axis->PositionOnScale(scale[j]);
				char text[150];
				sprintf(text,"%.2f",value);
				DrawText(text,10,axis->x - 6,height,textColor,.1F);
			}
		}
	
		size_t records_per_column = dataset->NumberOfValuesPerRecord();
		//printf("record_per_column = %d\n",records_per_column);

		//Draw the lines
		glColor3f(1.0, 0.0, 0.0);
		glLineWidth(1.F);
    
		float delta = 1.F / (columns + 1) * width;

		size_t lineIndex = 0;
		size_t totalNumberOfLines = dataset->NumberOfValuesPerRecord();

		//for each record
		for(size_t record_i = 0 ; record_i < records_per_column ; record_i++) {
			//For each axis/column
			//printf("record_i = %d\n",record_i);
			glBegin(GL_LINE_STRIP);
			for(size_t axis_index = 0 ; axis_index < axes.size() ; axis_index++) {
				//printf("axis_index = %d\n",axis_index);	
				ParallelCoordsAxis *axis = &axes[axis_index];

                std::pair<float,float> *min_max = dataset->GetRecord(axis_index)->MinMax();
				float* value = dataset->GetRecordValue(axis_index,record_i);

				float* color = computeColor(lineIndex,totalNumberOfLines);
				glColor3f(color[0],color[1],color[2]);

				if(value != 0) {
					float x = axis->x;

					//indicating the ratio of height, where max = 1 and min = 0
					float heightRatio;
					if(min_max->second == min_max->first) { //Special case to avoid divide-by-zero
						heightRatio = .5F;
					}	
					else heightRatio = (*value - min_max->first) / (min_max->second - min_max->first); 

					float y = heightRatio * axis->height + startY + marginHeight;

					//printf("heightratio = %f\n",heightRatio);
					//printf("value point (x,y) = (%f,%f)\n",x,y);
					glVertex3f(x, y, 0);
				}
			}
			++lineIndex;
			glEnd();
		}
		//needsRedraw = false; //TODO: This does not work when losing and regaining focus!
	}
}

float* ParallelCoordsView::computeColor(int lineIndex, int totalNrOfLines) {
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
    axes.clear(); //Remove the old axes
    
    size_t nr_of_axes = dataset->NumberOfRecords();
	
    
    //Fixed y for all axes
    int y = startY + paddingY;
    int axisHeight = height - 2 * paddingY;
    float delta = 1.F / (nr_of_axes - 1) * (width - 2 * paddingX);
    
    for(size_t i = 0 ; i < nr_of_axes ; i++) {
        int x = delta * i + startX + paddingX;
        
        RIVRecord<float>* record = dataset->GetRecord(i);
        ParallelCoordsAxis axis(x,y,axisHeight,record);

		axis.ComputeScale(4);

        axes.push_back(axis);
    }
}

bool ParallelCoordsView::HandleMouse(int button, int state, int x, int y) {
	if(RIVDataView::containsPoint(x,y)) {
		//What axis was selected
		//What range was selected
		//How to define this range
		return true;
	}
    return false;
}

bool ParallelCoordsView::HandleMouseMotion(int x, int y) {
	return false;
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
