#include "ParallelCoordsView.h"
#include "SliderView.h"
#include "DataView.h"
#include "../Graphics/graphics_helper.h"
#include "../helper.h"
#include "../Geometry/Geometry.h"
#include "../Data/DataSet.h"
#include "../Data/Table.h"
#include "../Configuration.h"

#include <math.h>
#include <stdio.h>

//Static declarations
ParallelCoordsView* ParallelCoordsView::instance = NULL;
int ParallelCoordsView::windowHandle = -1;

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColorOne, RIVColorProperty *rayColorOne,RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo,RIVSliderView* sliderView) : RIVDataView(datasetOne,datasetTwo,x,y,width,height,paddingX,paddingY,pathColorOne,rayColorOne,pathColorTwo,rayColorTwo), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo), sliderView(sliderView) {
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	identifier = "ParallelCoordsView";
	instance = this;
}
ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, RIVColorProperty *pathColorOne, RIVColorProperty *rayColorOne, RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo,RIVSliderView* sliderView) :
RIVDataView(datasetOne,datasetTwo,pathColorOne,rayColorOne,pathColorTwo,rayColorTwo), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo), sliderView(sliderView)
{
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	identifier = "ParallelCoordsView";
	instance = this;
}

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty* pathColor, RIVColorProperty *rayColor,RIVSliderView* sliderView) : RIVDataView(dataset,x,y,width,height, paddingX, paddingY), distributionsOne(distributionsOne),sliderView(sliderView) {
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	this->pathColorOne = pathColor;
	this->rayColorOne = rayColor;
	identifier = "ParallelCoordsView";
	instance = this;
	//Nothing else to do
}

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, RIVColorProperty *pathColor, RIVColorProperty* rayColor,RIVSliderView* sliderView) : RIVDataView(dataset), distributionsOne(distributionsOne), sliderView(sliderView) {
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	this->pathColorOne = pathColor;
	this->rayColorOne = rayColor;
	identifier = "ParallelCoordsView";
	instance = this;
}

void ParallelCoordsView::createAxes() {
	printf("Create axes\n");
	axisGroups.clear();
	size_t total_nr_of_records = 0;
	//    int y = startY + paddingY;
	int y = 1.5F * paddingY; //View port takes care of startY
	int axisHeight = height - 2.5F * paddingY;
    
	std::vector<std::string> tablesToDisplay = {PATHS_TABLE,INTERSECTIONS_TABLE};
	
	for(const std::string& tableName : tablesToDisplay) {
		RIVTable<float,ushort> *table = (*datasetOne)->GetTable(tableName);
		total_nr_of_records += table->NumberOfRecords();
	}
	
	float delta = 1.F / (total_nr_of_records - 1) * (width - 2 * paddingX);
	
	size_t axisIndex = 0;
	//		std::vector<std::string> pathTableRecordNames = {"pixel x","pixel y","R","G","B","throughput R","throughput G","throughput B","renderer","depth"};
	const int divisionCount = 4;
	if(datasetTwo) {
		for(const std::string& tableName : tablesToDisplay) {
			//			std::vector<RIVTable<float,ushort>*>* otherTablePointers = (*datasetTwo)->GetTables();
			RIVTable<float,ushort> *table = (*datasetOne)->GetTable(tableName);
			RIVTable<float,ushort> *otherTable = (*datasetTwo)->GetTable(tableName);
			
			ParallelCoordsAxisGroup<float,ushort> axisGroup(table->GetName());
			
			auto recordsTuple = table->GetAllRecords();
			tuple_for_each(recordsTuple, [&](auto tRecords) {
				for(size_t i = 0 ; i < tRecords.size() ; ++i) {
					auto& record = tRecords.at(i);
					
					typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
					auto otherRecord = otherTable->GetRecord<Type>(record->name);
					
					int x = delta * (axisIndex) + paddingX;
					
					//A tuple containing the min and max values of the record
					auto minMax = record->MinMax();
					auto otherMinMax = otherRecord->MinMax();
					const std::string& name = record->name;
					//				printf("Record %s has min-max : ",record->name.c_str());
					//				std::cout << " " << minMax.first << ", " << minMax.second << std::endl;
					
					auto histOne = distributionsOne->GetHistogram<Type>(name);
					auto histTwo = distributionsTwo->GetHistogram<Type>(name);
					
					//					axisGroup.CreateAxis(record, x, y, axisWidth, axisHeight, std::min(minMax.first,otherMinMax.first), std::max(minMax.second,otherMinMax.second), record->name, divisionCount,distributionsOne->GetHistogram<Type>(name),distributionsTwo->GetHistogram<Type>(name));
					
					auto dataHistOne = Histogram<Type>(name,histOne->LowerBound(),histOne->UpperBound(),histOne->NumberOfBins());
					auto dataHistTwo = Histogram<Type>(name,histTwo->LowerBound(),histTwo->UpperBound(),histTwo->NumberOfBins());
					
					axisGroup.CreateAxis(record, x, y, axisWidth, axisHeight, std::min(minMax.first,otherMinMax.first), std::max(minMax.second,otherMinMax.second), record->name, divisionCount,dataHistOne,dataHistTwo);
					axisIndex++;
				}
			});
			axisGroups.push_back(axisGroup);
		}
	}
	else {
		for(const std::string& tableName : tablesToDisplay) {
			RIVTable<float,ushort> *table = (*datasetOne)->GetTable(tableName);
			ParallelCoordsAxisGroup<float,ushort> axisGroup(table->GetName());
			auto recordsTuple = table->GetAllRecords();
			tuple_for_each(recordsTuple, [&](auto tRecords) {
				for(size_t i = 0 ; i < tRecords.size() ; ++i) {
					auto& record = tRecords.at(i);
					
					typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
					
					int x = delta * (axisIndex) + paddingX;
					std::string name = record->name;
					//A tuple containing the min and max values of the record
					auto minMax = record->MinMax();
					//				printf("Record %s has min-max : ",record->name.c_str());
					//				std::cout << " " << minMax.first << ", " << minMax.second << std::endl;
					
					auto histOne = distributionsOne->GetHistogram<Type>(name);
					auto dataHistOne = Histogram<Type>(name,histOne->LowerBound(),histOne->UpperBound(),histOne->NumberOfBins());
					
					axisGroup.CreateAxis(record, x, y, axisWidth, axisHeight, minMax.first, minMax.second, name, divisionCount, dataHistOne);
					axisIndex++;
				}
			});
			axisGroups.push_back(axisGroup);
		}
	}
	printf("Create axes finished\n");
}

void ParallelCoordsView::drawSelectionBoxes() {
	for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto& axis : tAxes) {
				if(axis->HasSelectionBox) {
					printf("Drawing selection box on axis %s\n",selectedAxis->name.c_str());
					
					glColor4f(1, 1, 1,.3F);
					glRectf(axis->selection.start.x - 1, axis->selection.start.y, axis->selection.end.x, axis->selection.end.y);
					
				}
			}
		});
	}
	
}
void ParallelCoordsView::drawAxes() {
	//Draw the densities
	for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto axis : tAxes) {
				
				//				printf("Draw axis %s\n",axis->name.c_str());
                if(axis->height < 100) {
                    
                }
				
				auto& histogramOne = axis->densityHistogramOne;
				auto& histogramTwo = axis->densityHistogramTwo;
				
				int numBins = histogramOne.NumberOfBins();
				int startX = axis->x - axis->width / 2.F;
				int endX = startX + axis->width;
				int height = axis->height / (float)numBins;
				
				float maxValueOne = histogramOne.MaximumValue();
				float maxValueTwo = histogramTwo.MaximumValue();
				
				for(int i = 0 ; i < numBins ; ++i) {
					//					printf("bin = %d\n",i);
					
					int startY = i * height + axis->y;
					int endY = startY + height;
					
					float binValueOne = histogramOne.BinValue(i);
					float binValueTwo = histogramTwo.BinValue(i);
					
					//					printf("bin value one = %f\n",binValueOne);
					//					printf("bin value two = %f\n",binValueTwo);
					
					size_t nrElementsOne = histogramOne.NumberOfElements();
					size_t nrElementsTwo = histogramTwo.NumberOfElements();
					
					if(nrElementsOne || nrElementsTwo) {
						float normalizedValueOne = 0;
						float normalizedValueTwo = 0;
						if(nrElementsOne)
							normalizedValueOne = binValueOne / nrElementsOne;
						if(nrElementsTwo)
							normalizedValueTwo = binValueTwo / nrElementsTwo;
						
						//					float valueDelta = valueTwo - valueTwo;
						float blueColorValue;
						float redColorValue;
						float saturation;
						
						if(binValueOne == nrElementsOne && binValueTwo == nrElementsTwo) {
							if(binValueTwo > binValueOne) {
								blueColorValue = ((binValueTwo - binValueOne) / binValueTwo);
								redColorValue = 1-blueColorValue;
								saturation = binValueTwo / maxValueTwo;
							}
							else {
								redColorValue = ((binValueOne - binValueTwo) / binValueOne);
								blueColorValue = 1 - redColorValue;
								saturation = binValueOne / maxValueOne;
							}
						}
						else {
							if(normalizedValueTwo > normalizedValueOne) {
								blueColorValue = ((normalizedValueTwo - normalizedValueOne) / normalizedValueTwo + 1) / 2.F;
								redColorValue = 1-blueColorValue;
								saturation = binValueTwo / maxValueTwo;
							}
							else {
								redColorValue = ((normalizedValueOne - normalizedValueTwo) / normalizedValueOne + 1) / 2.F;
								blueColorValue = 1 - redColorValue;
								saturation = binValueOne / maxValueOne;
							}
						}
						if(useSaturation) {
							
//							saturation = std::pow(saturation, 0.25);
							
							//Convert the values to HSV
							float h,s,v;
							RGBtoHSV(redColorValue, 0, blueColorValue, &h, &s, &v);
							
							//And back again but use the computed saturation instead
							float r,g,b;
							HSVtoRGB(&r, &g, &b, h, saturation, v);
							
							glColor3f(r,g,b);
						}
						else {
							glColor3f(redColorValue, 0, blueColorValue);
						}
						
						//						printf("glColor3f(%f,%f,%f)\n",r,g,b);

						glRectf(startX, startY, endX, endY);
					}
				}
				
				glColor3f(0,0,0);
				glBegin(GL_LINES);
				for(int i = 0 ; i <= numBins ; ++i) {
					int y = i * height + axis->y;
					glVertex2f(startX, y);
					glVertex2f(startX + axis->width, y);
				}
				glVertex2f(axis->x - axis->width / 2.F, axis->y);
				glVertex2f(axis->x - axis->width / 2.F, axis->y + axis->height);
				glVertex2f(axis->x + axis->width / 2.F, axis->y);
				glVertex2f(axis->x + axis->width / 2.F, axis->y + axis->height);
				glEnd();
			}
		});
	}
}

void ParallelCoordsView::drawAxesExtras() {
	
	glColor3f(1,0,0);
	glLineWidth(1.F);
	
	float textColor[3] = {0,0,0};
	
	//Half of its size
	int selectionGlyphSize = 7;
	
	//Draw texts and extras
	for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto& axis : tAxes) {
				
				//Draw the name of the data dimension at the bottom of the axis
				std::string& text = axis->name;
				drawText(text,axis->x,axis->y - 12,textColor,.08F);
				
				//Draw a selection glyph if it is selected
				if(axis->isSelected) {
					int glyphY = axis->y - 20;
					
					glBegin(GL_POLYGON);
					glVertex2f(axis->x - selectionGlyphSize,glyphY - selectionGlyphSize);
					glVertex2f(axis->x + selectionGlyphSize,glyphY - selectionGlyphSize);
					glVertex2f(axis->x,glyphY);
					glEnd();
					//					glRectf(axis->x - selectionGlyphSize, glyphY - selectionGlyphSize, axis->x + selectionGlyphSize, glyphY + selectionGlyphSize);
				}
				
				auto &scale = axis->scale;
				
				//Draw the scales indicators
				if(scale.size() > 0) {
					for(size_t j = 0 ; j < scale.size() ; j++) {
						float value = axis->ValueOnScale(scale[j]);
						int height = axis->PositionOnScaleForScalar(scale[j]);
						
						// Draw small line to indicate
						// glBegin(GL_LINES);
						//	glVertex2f(axis.x, height);
						//	glVertex2f(axis.x + 10, height);
						// glEnd();
						
						std::string text = std::to_string(value);
						
						char buffer[4];
						sprintf(buffer,"%.2f",value);
						
						drawText(buffer,4,axis->x + axis->width + 2,height,textColor,.08F);
					}
				}
			}
		});
	}
	Invalidate();
}
void ParallelCoordsView::createAxisPoints() {
	//	printf("CREATE AXIS POINTS!!!\n");
	createAxisPoints(0, *datasetOne);
	if(datasetTwo) {
		createAxisPoints(1, *datasetTwo);
	}
}
void ParallelCoordsView::createAxisDensities() {
	createAxisDensities(0, *datasetOne);
	if(datasetTwo) {
		createAxisDensities(1, *datasetTwo);
	}
}
void ParallelCoordsView::createAxisDensities(int datasetId, RIVDataSet<float,ushort>* dataset) {
	for(auto &axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto axis : tAxes) {
				axis->GetHistogram(datasetId)->Clear();
			}
		});
	}
	
	for(auto &axisGroup : axisGroups) {
		size_t row = 0;
		
		auto table = dataset->GetTable(axisGroup.tableName);
		
		
		tuple_for_each(axisGroup.axes, [&](auto& tAxes) {
			for(auto& axis : tAxes) {
				
				TableIterator* iterator = table->GetIterator();
				while(iterator->GetNext(row)) {
					
					auto record = table->GetRecord<decltype(axis->minValue)>(axis->name);
					auto value = record->Value(row);
					
					axis->GetHistogram(datasetId)->Add(value);
				}
			}
		});
		
	}
}
void ParallelCoordsView::createAxisPoints(int datasetId, RIVDataSet<float,ushort>* dataset) {
    printf("CREATE AXIS POINTS FOR DATASET  #%d\n",datasetId);
	for(auto &axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto axis : tAxes) {
				axis->GetPoints(datasetId).clear();
			}
		});
	}
    size_t pointsCreated = 0;
	for(auto &axisGroup : axisGroups) {
		size_t row = 0;
		
		auto table = dataset->GetTable(axisGroup.tableName);
		TableIterator* iterator = table->GetIterator(true);
        pointsCreated = 0;
		while(iterator->GetNext(row)) {
            ++pointsCreated;
			tuple_for_each(axisGroup.axes, [&](auto& tAxes) {
				for(auto& axis : tAxes) {
					auto record = table->GetRecord<decltype(axis->minValue)>(axis->name);
					auto value = record->Value(row);
					
					axis->GetPoints(datasetId).push_back(axis->PositionOnScaleForValue(value));

				}
			});
		}
        
	}
    printf("%zu points per axis created.\n",pointsCreated);
    
}
void ParallelCoordsView::drawLines(int datasetId, RIVDataSet<float,ushort>* dataset, RIVColorProperty* pathColors, RIVColorProperty* rayColors) {
	//	linesAreDirty = true;
	
	//	dataset->Print();
	
	size_t lineIndex = 0;
	
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	/** AD HOC MEMBERSHIP COLORING PROPERTY CREATION **/
	
	bool pathMembershipDataPresent = false;
	bool isectMembershipDataPresent = false;
	
	RIVTable<float,ushort>* pathMembershipTable = dataset->GetTable(PATH_MEMBERSHIP_TABLE);
	RIVTable<float,ushort>* isectMembershipTable = dataset->GetTable(ISECT_MEMBERSHIP_TABLE);
	
	RIVFloatRecord* pathMembershipRecord = NULL;
	RIVFloatRecord* isectMembershipRecord = NULL;
	
	riv::ColorMap colorMap;
	colorMap.AddColor(colors::RED);
	colorMap.AddColor(colors::BLUE);
	
	if(pathMembershipTable) {
		pathMembershipRecord = pathMembershipTable->GetRecord<float>(MEMBERSHIP);
		
		if(pathMembershipRecord != NULL && pathMembershipRecord->Size()) {
			pathMembershipDataPresent = true;
		}
	}
	
	if(isectMembershipTable) {
		isectMembershipRecord = isectMembershipTable->GetRecord<float>(MEMBERSHIP);
		if(isectMembershipRecord) {
			isectMembershipDataPresent = isectMembershipRecord->Size();
		}
	}
	
	glLineWidth(1);
	for(auto &axisGroup : axisGroups) {
        auto table = dataset->GetTable(axisGroup.tableName);
		size_t numRows = table->NumberOfRows();
        size_t row = 0;
        if(numRows != axisGroup.axisInterfaces[0]->GetPoints(datasetId).size()) {
            printf("ERROR: Points not yet present...\n");
//            createAxes();
//            createAxisDensities();
//            createAxisPoints();
            return;
        }
        

		TableIterator* iterator = table->GetIterator();
		
		

		
		//Find what color property to use for this table
		RIVColorProperty* colorProperty = pathColors;
		if(table->name == PATHS_TABLE) {
			if(pathMembershipDataPresent) {
				colorProperty = new RIVEvaluatedColorProperty<float>(colorMap,table,pathMembershipRecord);
			}
			else colorProperty = pathColors;
			//			membershipRecord = pathMembershipRecord;
		}
		else if(table->name == INTERSECTIONS_TABLE) {
			if(isectMembershipDataPresent) {
				colorProperty = new RIVEvaluatedColorProperty<float>(colorMap,table,isectMembershipRecord);
			}
			else colorProperty = rayColors;
		}
		
		//You gotta love 'auto'!
		size_t lineIndex = 0;
		riv::Color lineColor;
		while(iterator->GetNext(row)) {
			
			
			float offset = axisWidth / 2.F;
			
			if(colorProperty->ComputeColor(table, row, lineColor)) {
				
				//					if(membershipRecord && membershipRecord->Size()) {
				//						float membershipValue = membershipRecord->Value(row);
				//						float blueColorValue = (membershipValue + 1) / 2.F;
				//						glColor4f(1-blueColorValue, lineColor.G, blueColorValue, lineOpacity);
				//					}
				//					else {
				glColor4f(lineColor.R, lineColor.G, lineColor.B, lineOpacity);
				//					}
				
				//TODO: Optimize this; unnecessary lines are being drawn 'through' the axis
				glBegin(GL_LINE_STRIP);
				//				int axisIndex = 0;
				
				
				for(auto axis : axisGroup.axisInterfaces) {
					
					float x = axis->x;
					float y = axis->GetPoints(datasetId)[row];
					
					glVertex2f(x - offset, y);
					glVertex2f(x + offset, y);
					//						offset = -offset * axisIndex++;
					
				}
				
				glEnd();
				++lineIndex;
			}
		}
	}
	printf("Parallel coordinates view drew %zu polylines\n",lineIndex);
	//	reporter::stop("drawLines");
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
	
	    printf("New width = %d\n",width);
	    printf("New height = %d\n",height);
	
 	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	paddingX = 30;
	paddingY = 20;
	
	Invalidate();
	
	InitializeGraphics();
}

void ParallelCoordsView::InitializeGraphics() {
	//Recreate the axes according to the new layout
	//	createAxes();
}

size_t drawCount = 0;
void ParallelCoordsView::Draw() {
	//    printf("linesAreDirty = %d axesAreDirty = %d\n",linesAreDirty,axesAreDirty);
	printHeader("PARALLEL COORDINATES PLOT VIEW DRAW");
	
	std::string taskName = "ParallelCoordsView Draw";
	reporter::startTask(taskName);
	printf("\n");
	
	if(needsRedraw) {
		printf("Clear PCV window\n");
		glClearColor(0.9, 0.9, 0.9, 0.0);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
	
	//Draw the lines from each axis
	if(needsRedraw) {
		if(drawDataSetOne) {
			printf("Draw lines for dataset one\n");
			drawLines(0,*datasetOne,pathColorOne,rayColorOne);
		}
		if(datasetTwo && drawDataSetTwo) {
            printf("Draw lines for dataset two\n");
            drawLines(1,*datasetTwo,pathColorTwo,rayColorTwo);
		}
		drawAxes();
		drawAxesExtras();
		drawSelectionBoxes();
	}
	
	glFlush();
	glutSwapBuffers();
	
	reporter::stop(taskName);
}

void ParallelCoordsView::ToggleDrawDataSetOne() {
	drawDataSetOne = !drawDataSetOne;
	
	Invalidate();
	
	redisplayWindow();
}

void ParallelCoordsView::ToggleDrawDataSetTwo() {
	drawDataSetTwo = !drawDataSetTwo;
	
	Invalidate();
	
	redisplayWindow();
}

bool ParallelCoordsView::HandleMouse(int button, int state, int x, int y) {
	//    printf("PCV HandleMouse\n");
	//    ToViewSpaceCoordinates(&x, &y);
	y = height - y;
	if(containsPoint(x,y)) {
		//What axis was selected
		if(state == GLUT_DOWN) {
			int proximityMax = 50;
			
			mouseDownX = x;
			mouseDownY = y;
			
			for(int i = 0 ; i < axisGroups.size() ; i++) {
				
				auto& axisGroup = axisGroups[i];
				bool axisFound = false;
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto& axis : tAxes) {
						
						int distanceX = abs(axis->x - x);
						
						if(distanceX < proximityMax) {
							selectedAxis = axis;
							//Select the
							if(axis->HasSelectionBox && ((y >= axis->selection.start.y && y <= axis->selection.end.y) || (y >= axis->selection.end.y && y <= axis->selection.start.y))) {
								//Is it inside the selection box?
								
								interactionState = MOUSE_DOWN_SELECTION;
								
								printf("NEW STATE IS MOUSE_DOWN_SELECTION\n");
								
								axisFound = true;
								
								return;
							}
							//Is it at least inside the axis
							else if(y > axis->y && y < axis->y + axis->height) {
								
								interactionState = MOUSE_DOWN_AXIS;
								
								printf("NEW STATE IS MOUSE_DOWN_AXIS\n");
								//Possibly create a selection box or swap axes
								
								//Close enough..
								axisFound = true;
								
								return;
								
							}
							else {
								
								interactionState = MOUSE_DOWN_AXIS_EXTRA;
								
								printf("NEW STATE IS MOUSE_DOWN_AXIS_EXTRA\n");
								
								selectedAxis = axis;
								
								if(axis->isSelected) {
									axis->isSelected = false;
									sliderView->RemoveSelectedRecord(axis->name);
								}
								else {
									axis->isSelected = true;
									sliderView->AddSelectedRecord(axisGroup.tableName, axis->name);
								}
								
								Invalidate();
								
								axisFound = true;
								
								glutPostRedisplay();
								
								return;
							}
						}
					}
				});
				if(axisFound) {
					return true;
				}
			}
		}
		else if(state == GLUT_UP) { //Finish selection
			
			switch (interactionState) {
				case DRAG_AXIS: {
					//					int distance = std::abs(x - mouseDownX);
					int minDistance = 25;
					
					ParallelCoordsAxisInterface* swapAxis = NULL;
					//				if(distance > 25) {
					//Should the axis be swapped with another axis?
					for(auto& axisGroup : axisGroups) {
						bool axisGroupFound = false;
						int shortestDistance = 10000;
						int axisIndex = 0;
						int swapAxisIndex = 0;
						for(size_t i = 0 ; i < axisGroup.axisInterfaces.size() ; ++i) {
							ParallelCoordsAxisInterface* axis = axisGroup.axisInterfaces[i];
							if(axis->name == selectedAxis->name) {
								axisGroupFound = true;
								axisIndex = i;
							}
							else {
								int distance = std::abs(axis->x - selectedAxis->x);
								if(distance < shortestDistance) {
									shortestDistance = distance;
									swapAxis = axis;
									swapAxisIndex = i;
								}
							}
							if(axisGroupFound && swapAxis != NULL && shortestDistance < minDistance) {
								selectedAxis->x = axisOriginX;
								axisGroup.SwapAxes(axisIndex,swapAxisIndex);
								
								Invalidate();
								
								glutPostRedisplay();
								
								printf("SWAP SELECTED AXIS %s WITH AXIS %s\n",selectedAxis->name.c_str(), swapAxis->name.c_str());
								
								return true;
							}
						}
					}
					//				}
					break;
				}
				case DRAG_SELECTION:
					filterData();
					break;
				case CREATE_SELECTION_BOX:
					filterData();
					break;
				case MOUSE_DOWN_SELECTION:
					for(auto& axisGroup : axisGroups) {
						tuple_for_each(axisGroup.axes, [&](auto tAxes) {
							for(auto& axis : tAxes) {
								if(axis->name == selectedAxis->name) {
									axis->HasSelectionBox = false;
									typedef typename get_template_type<typename std::decay<decltype(*axis)>::type>::type Type;
									(*datasetOne)->StartFiltering();
									if(datasetTwo) {
										(*datasetTwo)->StartFiltering();
									}
									(*datasetOne)->ClearFilter<Type>(axis->name);
									if(datasetTwo) {
										(*datasetTwo)->ClearFilter<Type>(axis->name);
									}
									(*datasetOne)->StopFiltering();
									if(datasetTwo) {
										(*datasetTwo)->StopFiltering();
									}
								}
							}
						});
					}
					break;
				default:
					break;
			}
			
			printf("NEW STATE IS IDLE\n");
			interactionState = IDLE;
			return true;
		}
		return true;
	}
	return false;
}
void ParallelCoordsView::ToggleSaturationMode() {
	useSaturation = !useSaturation;
	
	Invalidate();
	
	redisplayWindow();
}
bool ParallelCoordsView::HandleMouseMotion(int x, int y) {
	y = height - y;
	
	int yDistanceTravelled = std::abs(y - mouseDownY);
	int xDistanceTravelled = std::abs(x - mouseDownX);
	
	printf("Distance travelled = (%d,%d)\n",xDistanceTravelled, yDistanceTravelled);
	
	switch (interactionState) {
		case IDLE:
			return true;
		case MOUSE_DOWN_AXIS_EXTRA:
			return true;
		case MOUSE_DOWN_AXIS:
			if(yDistanceTravelled > dragStartSensitivity) {
				interactionState = CREATE_SELECTION_BOX;
				printf("NEW STATE IS CREATE_SELECTION_BOX\n");
				selectedAxis->HasSelectionBox = true;
				selectedAxis->selection.start.x = selectedAxis->x - 10;
				selectedAxis->selection.end.x = selectedAxis->x + 10;
				selectedAxis->selection.start.y = y;
				selectedAxis->selection.end.y = y;
				selection = &selectedAxis->selection;
				
				selectionBoxChanged = false;
				Invalidate();
				
				axisUpdateY = y;
				return true;
			}
			else if(xDistanceTravelled > dragStartSensitivity) {
				interactionState = DRAG_AXIS;
				printf("NEW STATE IS DRAG_AXIS\n");
				
				selectionBoxChanged = false;
				Invalidate();
				
				axisOriginX = x;
				axisUpdateX = x;
				
				return true;
			}
		case MOUSE_DOWN_SELECTION:
			if(yDistanceTravelled > 5) {
				printf("NEW STATE IS DRAG_SELECTION\n");
				axisUpdateY = y;
				interactionState = DRAG_SELECTION;
				dragBoxLastY = y;
			}
			return true;
		case CREATE_SELECTION_BOX:
			printf("CREATE SELECTION...\n");
			//TODO: Not very efficient to find the axis everytime, but pointer sucks due to templates
			for(auto& axisGroup : axisGroups) {
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto& axis : tAxes) {
						if(axis->name == selectedAxis->name) {
							selection->end.y = axis->PositionOnScaleForViewY(y);
						}
					}
				});
			}
			if(std::abs(y - axisUpdateY) > updateSensitivity) {
				axisUpdateY = y;
				filterData();
				Invalidate();
				selectionBoxChanged = true;
			}
			Invalidate();
			glutPostRedisplay();
			return true;
		case DRAG_SELECTION: {
			printf("DRAG SELECTION...\n");
			int deltaY = y - dragBoxLastY;
			for(auto& axisGroup : axisGroups) {
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto& axis : tAxes) {
						if(axis->name == selectedAxis->name) {
							selection->start.y += deltaY;
							selection->end.y += deltaY;
						}
					}
				});
			}
			if(std::abs(y - axisUpdateY) > updateSensitivity) {
				axisUpdateY = y;
				filterData();
				Invalidate();
				selectionBoxChanged = true;
			}
			dragBoxLastY = y;
			glutPostRedisplay();
			return true;
		}
		case DRAG_AXIS: {
			if(std::abs(x - axisUpdateX) > updateSensitivity) {
				axisUpdateX = x;
				selectedAxis->x = x;
				
				Invalidate();
				
				glutPostRedisplay();
			}
			return true;
		}
	}
	return false;
}


void ParallelCoordsView::filterData() {
	for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto& axis : tAxes) {
				if(selectedAxis && axis->name == selectedAxis->name) {
					int sizeBox = abs(axis->selection.start.y - axis->selection.end.y);
					(*datasetOne)->StartFiltering();
					if(datasetTwo) {
						(*datasetTwo)->StartFiltering();
					}
					//A super clever way of deducing the type of the template parameter of the axis that was selected so we can clear/create a filter of the same type
					typedef typename get_template_type<typename std::decay<decltype(*axis)>::type>::type Type;
					if(sizeBox > 3) {
						auto lowerBound = axis->ValueOnScale(axis->ScaleValueForY(axis->selection.start.y));
						auto upperBound = axis->ValueOnScale(axis->ScaleValueForY(axis->selection.end.y));
						
						(*datasetOne)->ClearFilter<Type>(axis->name);
						if(datasetTwo) {
							(*datasetTwo)->ClearFilter<Type>(axis->name);
						}
						
						riv::SingularFilter<Type>* rangeFilter = new riv::RangeFilter<Type>(axis->name,lowerBound,upperBound);
						printf("New filter on axis %s : ",axis->name.c_str());
						rangeFilter->Print();
						(*datasetOne)->AddFilter(rangeFilter);
						if(datasetTwo) {
							(*datasetTwo)->AddFilter(rangeFilter);
						}
						
						selection = &axis->selection;
					}
					else {
						(*datasetOne)->ClearFilter<Type>(axis->name);
						if(datasetTwo) {
							(*datasetTwo)->ClearFilter<Type>(axis->name);
						}
						axis->HasSelectionBox = false;
						selectedAxis = NULL;
					}
					//Close access
					(*datasetOne)->StopFiltering();
					if(datasetTwo) {
						(*datasetTwo)->StopFiltering();
					}
				}
			}
		});
	}
}

void ParallelCoordsView::OnDataChanged(RIVDataSet<float,ushort>* source) {
	
	//Recreate the axes
	printf("\n**   ParallelCoordsView onDataChanged notified.\n");
	
	Invalidate();
    createAxes();
    createAxisDensities();
	
	//Recreate the color property
	if(source == (*datasetOne)) {
		printf("Dataset One was changed.\n\n");
		auto rgbPath = dynamic_cast<RIVColorRGBProperty<float>*>(pathColorOne);
		auto rgbRay = dynamic_cast<RIVColorRGBProperty<float>*>(rayColorOne);
//        createAxisPoints(0, *datasetOne);
		if(rgbPath) {
			auto pathTable = source->GetTable(PATHS_TABLE);
			auto isectTable = source->GetTable(INTERSECTIONS_TABLE);
			rgbPath->SetColorRecords(pathTable->GetRecord<float>(PATH_R), pathTable->GetRecord<float>(PATH_G), pathTable->GetRecord<float>(PATH_B));
			rgbRay->SetColorRecords(isectTable->GetRecord<float>(INTERSECTION_R), isectTable->GetRecord<float>(INTERSECTION_G), isectTable->GetRecord<float>(INTERSECTION_B));
		}
		else {
			pathColorOne->Reset(source);
			rayColorOne->Reset(source);
		}
	}
	else if(source == (*datasetTwo)) {
		printf("Dataset Two was changed.\n\n");
		auto rgbPath = dynamic_cast<RIVColorRGBProperty<float>*>(pathColorTwo);
		auto rgbRay = dynamic_cast<RIVColorRGBProperty<float>*>(rayColorTwo);
//        createAxisPoints(1, *datasetTwo);
		if(rgbPath) {
			auto pathTable = source->GetTable(PATHS_TABLE);
			auto isectTable = source->GetTable(INTERSECTIONS_TABLE);
			rgbPath->SetColorRecords(pathTable->GetRecord<float>(PATH_R), pathTable->GetRecord<float>(PATH_G), pathTable->GetRecord<float>(PATH_B));
			rgbRay->SetColorRecords(isectTable->GetRecord<float>(INTERSECTION_R), isectTable->GetRecord<float>(INTERSECTION_G), isectTable->GetRecord<float>(INTERSECTION_B));
		}
		else {
			pathColorTwo->Reset(source);
			rayColorTwo->Reset(source);
		}
	}
	else {
		throw std::runtime_error("Unknown dataset " + source->GetName());
	}

    createAxisPoints();
	
	//Change the ordering to have the bounce_nr to be the first of the second axis group
	//	axisGroups[1].Reorder(6,0);
	
	redisplayWindow();
}

void ParallelCoordsView::redisplayWindow() {
	int currentWindow = glutGetWindow();
	glutSetWindow(ParallelCoordsView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
}

void ParallelCoordsView::OnFiltersChanged(RIVDataSet<float,ushort>* dataset) {
	printf("ParallelCoordsView received a on filters changed callback.\n");
	
	if(dataset == *datasetOne) {
		createAxisDensities(0, dataset);
	}
	else if(datasetTwo && dataset == *datasetTwo) {
		createAxisDensities(1, dataset);
	}
	Invalidate();
	
	redisplayWindow();
}
bool ParallelCoordsView::DecreaseLineOpacity() {
	if(lineOpacity > 0.F) {
		lineOpacity -= lineOpacityIncrement;
		if(lineOpacity < 0.F) {
			lineOpacity = 0;
		}
		Invalidate();
		redisplayWindow();
		return true;
	}
	else return false;
}
bool ParallelCoordsView::IncreaseLineOpacity() {
	if(lineOpacity < 1.F) {
		lineOpacity += lineOpacityIncrement;
		if(lineOpacity > 1.F) {
			lineOpacity = 1;
		}
		Invalidate();
		redisplayWindow();
		return true;
	}
	else return false;
}