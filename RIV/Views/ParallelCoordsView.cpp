
#include "ParallelCoordsView.h"
#include "DataView.h"
#include "../Geometry/Geometry.h"
#include "../helper.h"
#include "../Data/DataSet.h"
#include "../Data/Table.h"

#include <math.h>
#include <stdio.h>

//Static declarations
ParallelCoordsView* ParallelCoordsView::instance = NULL;
int ParallelCoordsView::windowHandle = -1;

ParallelCoordsView::ParallelCoordsView(RIVDataSet* dataset) : RIVDataView(dataset) {
    instance = this;
	Color black;
	black.R = 0;
	black.G = 0;
	black.B = 0;
    colorProperty = new RIVFixedColorProperty(black);
    sizeProperty = new RIVFixedSizeProperty(1);
}

ParallelCoordsView::ParallelCoordsView(RIVDataSet* dataset, int x, int y, int width, int height, int paddingX, int paddingY, RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(dataset,x,y,width,height, paddingX, paddingY,colorProperty,sizeProperty) {
    if(instance != NULL) {
        throw "Only 1 instance allowed.";
    }
    linesAreDirty = true;
    axesAreDirty = true;
    selectedAxis = 0;
    identifier = "ParallelCoordsView";
    instance = this;
    //Nothing else to do
}

ParallelCoordsView::ParallelCoordsView(RIVDataSet* dataset, RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(dataset,colorProperty,sizeProperty) {
    if(instance != NULL) {
        throw "Only 1 instance allowed.";
    }
    linesAreDirty = true;
    axesAreDirty = true;
    selectedAxis = 0;
    identifier = "ParallelCoordsView";
	instance = this;
}

ParallelCoordsView::~ParallelCoordsView(void) {
	
}

void ParallelCoordsView::createAxes() {
    axisGroups.clear();
    if(dataset != NULL) {
        size_t total_nr_of_records = dataset->TotalNumberOfRecords();
		//    int y = startY + paddingY;
        int y = paddingY; //View port takes care of startY
        int axisHeight = height - 2 * paddingY;
        
        float delta = 1.F / (total_nr_of_records - 1) * (width - 2 * paddingX);
        std::vector<RIVTable*>* tablePointers = dataset->GetTables();
        
        int axisIndex = 0;
        
        ParallelCoordsAxisGroup *fromConnection = NULL;
        ParallelCoordsAxis *lastAxis = NULL;
        
        for(size_t j = 0 ; j < tablePointers->size() ; j++) {
            RIVTable *table = tablePointers->at(j);
            ParallelCoordsAxisGroup axisGroup;
            size_t numberOfRecords = table->NumberOfColumns();
            for(size_t i = 0 ; i < numberOfRecords ; ++i) {
                RIVRecord* record = table->GetRecord(i);
                RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(record);
                int x = delta * (axisIndex) + paddingX;
                if(floatRecord) {
                    
                    std::pair<float,float> minMax = floatRecord->MinMax();
                    
                    ParallelCoordsAxis axis(x,y,axisHeight,minMax.first,minMax.second,record->name);
                    
                    axis.ComputeScale(4);
                    axis.RecordPointer = record;
                    
                    axisGroup.axes.push_back(axis);
                    lastAxis = &axis;
                    axisIndex++;
                    continue;
                }
                RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(record);
                if(shortRecord) {
                    
                    const std::pair<ushort,ushort> &minMax = shortRecord->MinMax();
                    ParallelCoordsAxis axis(x,y,axisHeight,minMax.first,minMax.second,record->name);
                    
                    axis.ComputeScale(4);
                    axis.RecordPointer = record;
                    axisIndex++;
                    lastAxis = &axis;
                    axisGroup.axes.push_back(axis);
                }
            }
            axisGroup.table = table;
            if(fromConnection) { //Any outstanding connections to be made?
                axisGroup.connectorAxis = lastAxis;
                axisGroup.connectedGroup = fromConnection;
				//            printf("Axis groups connected\n");
                //Erase connection pointer
            }
            if(j < tablePointers->size() - 1) { //Only connect if not last
                //What is the next table group?
                RIVTable* nextTable = (*tablePointers)[j + 1];
                const RIVTable* result = table->FindTable(nextTable->GetName());
                if(result) { //If a reference exists, connect the groups
                    //But the next axisgroup is not yet made, so cache it until it is made
                    fromConnection = &axisGroup;
                    axisGroup.connectorAxis = lastAxis;
                }
            }
            axisGroups.push_back(axisGroup);
        }
    }
}

void ParallelCoordsView::drawSelectionBoxes() {
	for(ParallelCoordsAxisGroup& axisGroup : axisGroups) {
		for(ParallelCoordsAxis& axis : axisGroup.axes) {
			if(axis.HasSelectionBox) {
				glColor3f(1, 0, 0);
				glBegin(GL_LINE_STRIP);
				
				Area &selectionBox  = axis.selection;
				
				glVertex3f(selectionBox.start.x,selectionBox.start.y,0);
				glVertex3f(selectionBox.end.x,selectionBox.start.y,0);
				
				glVertex3f(selectionBox.end.x,selectionBox.end.y,0);
				glVertex3f(selectionBox.start.x,selectionBox.end.y,0);
				
				glVertex3f(selectionBox.start.x,selectionBox.start.y,0);
				
				glEnd();
			}
		}
	}
}


void ParallelCoordsView::drawAxes() {
	
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
                int height = axis.PositionOnScaleForScalar(scale[j]);
                
                std::string text = std::to_string(value);
                
                char buffer[4];
                sprintf(buffer,"%.2f",value);
                
                drawText(buffer,4,axis.x - 6,height,textColor,.1F);
            }
        }
    }
    axesAreDirty = false;
}

void ParallelCoordsView::drawLines() {
//	linesAreDirty = true;

		size_t lineIndex = 0;
        glColor3f(1.0, 0.0, 0.0);
        size_t linesDrawn = 0;
		
        for(ParallelCoordsAxisGroup &axisGroup : axisGroups) {
            RIVTable *table = axisGroup.table;
            
            size_t row = 0;

            TableIterator* iterator = table->GetIterator();
            Color lineColor;
            while(iterator->GetNext(row)) {
                if(colorProperty->ComputeColor(table, row, lineColor)) {
					glBegin(GL_LINE_STRIP); //Unconnected groups, draw connections later as they are not 1-to-1
					
					float size = sizeProperty->ComputeSize(table,row);
					
					glLineWidth(size);
                    glColor3f(lineColor.R, lineColor.G, lineColor.B);
                    for(ParallelCoordsAxis &axis : axisGroup.axes) {
                        RIVRecord *ptr = axis.RecordPointer;
                        RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(ptr);
		
                        if(floatRecord) {
							//                            //Code here
                            float value = floatRecord->Value(row);
                            float x = axis.x;
                            float y = axis.PositionOnScaleForValue(value);

                            ++linesDrawn;
                            glVertex3f(x, y, 0);
//                            if(y > axis.y + axis.height || y < axis.y) {
//                                throw new std::string("A line was being drawn outside ot the parallel coordinates view");
//                            }
                            continue;
                        }
                        RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(ptr);
                        if(shortRecord) {
                            //Code here
                            
                            ushort value = shortRecord->Value(row);
                            
                            float x = axis.x;
                            float y = axis.PositionOnScaleForValue(value);
                            glVertex3f(x, y, 0);
							
//                            if(y > axis.y + axis.height || y < axis.y) {
//                                throw new std::string("A line was being drawn outside ot the parallel coordinates view");
//                            }
                            ++linesDrawn;
                        }
                    }
                }
                glEnd();
                lineIndex++;
            }
        
        linesAreDirty = false;
        printf("Drawn %zu lines.\n",linesDrawn);
    }
}

void ParallelCoordsView::DrawInstance() {
    if(instance != NULL) {
        instance->Draw();
    }
}

void ParallelCoordsView::ReshapeInstance(int width, int height) {
    if(instance != NULL) {
        instance->Reshape(width,height);
    }
}

void ParallelCoordsView::Mouse(int button, int state, int x, int y) {
    if(instance != NULL) {
        instance->HandleMouse(button,state,x,y);
    }
}

void ParallelCoordsView::Motion(int x, int y) {
    if(instance != NULL) {
        instance->HandleMouseMotion(x, y);
    }
}

void ParallelCoordsView::Reshape(int width, int height) {
	//    printf("Reshape Parallel Coords View!\n");
    
    this->width = width;
    this->height = height;
    
	//    printf("New width = %d\n",width);
	//    printf("New height = %d\n",height);
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    paddingX = 20;
    paddingY = 20;
    
    axesAreDirty = true;
    linesAreDirty = true;
	
	InitializeGraphics();
}

void ParallelCoordsView::InitializeGraphics() {
	//Recreate the axes according to the new layout
	createAxes();
}
size_t drawCount = 0;
void ParallelCoordsView::Draw() {
	//    printf("linesAreDirty = %d axesAreDirty = %d\n",linesAreDirty,axesAreDirty);
	printf("ParallelCoordsView Draw #%zu : \n",++drawCount);
	
	if(axesAreDirty && linesAreDirty && selectionIsDirty) {
		printf("Clear PCV window\n");
        glClearColor(0.9, 0.9, 0.9, 0.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
		
	//Draw the axes
	if(axesAreDirty)
        drawAxes();
        
        //Draw the lines from each axis
	if(linesAreDirty)
        drawLines();
	
	if(selectionIsDirty)
		drawSelectionBoxes();
        
	glutSwapBuffers();
}

bool ParallelCoordsView::HandleMouse(int button, int state, int x, int y) {
	//    printf("PCV HandleMouse\n");
//    ToViewSpaceCoordinates(&x, &y);
    y = height - y;
	if(containsPoint(x,y) || isDragging) {
		//What axis was selected
        if(state == GLUT_DOWN) {
            int proximityMax = 50;
            
            //            printf("(x,y)=(%d,%d)",x,y);
            for(ParallelCoordsAxisGroup &axisGroup : axisGroups) {
                for(ParallelCoordsAxis &axis : axisGroup.axes) {
                    int distanceX = abs(axis.x - x);
//                    printf("distance=%d\n",distanceX);
                    
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
                        linesAreDirty = true;
                        
//                        printf("Selected axis %s\n",axis.name.c_str());
                        
                        return true;
                    }
                }
            }
            selectedAxis = NULL;
        }
        else if(state == GLUT_UP) { //Finish selection
            if(isDragging && selectedAxis != 0) {
                
                Area &selection = selectedAxis->selection;
                int sizeBox = abs(selection.start.y - selection.end.y);
				dataset->StartFiltering();
                if(sizeBox > 3) {
                    float lowerBound = selectedAxis->ValueOnScale(selectedAxis->ScaleValueForY(selection.start.y));
                    float upperBound = selectedAxis->ValueOnScale(selectedAxis->ScaleValueForY(selection.end.y));
                    dataset->ClearFilter(selectedAxis->name);
                    riv::Filter* rangeFilter = new riv::RangeFilter(selectedAxis->name,lowerBound,upperBound);
                    dataset->AddFilter(rangeFilter);
                    printf("Selection finalized on axis %s\n",selectedAxis->name.c_str());
                }
                else {
                    dataset->ClearFilter(selectedAxis->name);
                    clearSelection();
                }
				
				//Close access
				dataset->StopFiltering();
            }
            return true;
        }
		return true;
	}
    return false;
}

void ParallelCoordsView::clearSelection() {
    if(selectedAxis != NULL) {
        selectedAxis->selection.start.y = -1;
        selectedAxis->selection.start.x = -1;
        selectedAxis->selection.end.x = -1;
        selectedAxis->selection.end.y = -1;
        
        selectedAxis->HasSelectionBox = false;
        selectedAxis = NULL;
        
        
        printf("Selection cleared.");
    }
}

bool ParallelCoordsView::HandleMouseMotion(int x, int y) {
    y = height - y;
    if(isDragging && selectedAxis != 0) {
        Area *selection = &selectedAxis->selection;
        
		selection->end.y = selectedAxis->PositionOnScaleForViewY(y);
		selectionIsDirty = true;
        
        return true;
    }
    return false;
}

void ParallelCoordsView::OnDataSetChanged() {
    printf("ParallelCoordsView received a on dataset changed callback.\n");
    linesAreDirty = true;
    axesAreDirty = true;
	
	int currentWindow = glutGetWindow();
	glutSetWindow(ParallelCoordsView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
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
