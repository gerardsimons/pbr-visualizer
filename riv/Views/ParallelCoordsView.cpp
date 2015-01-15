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

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColorOne, RIVColorProperty *rayColorOne,RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo,RIVSliderView* sliderView) : RIVDataView(datasetOne,datasetTwo,x,y,width,height,paddingX,paddingY), pathColorOne(pathColorOne), rayColorOne(rayColorOne), pathColorTwo(pathColorTwo), rayColorTwo(rayColorTwo), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo), sliderView(sliderView) {
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	linesAreDirty = true;
	axesAreDirty = true;
	identifier = "ParallelCoordsView";
	instance = this;
}
ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, RIVColorProperty *pathColorOne, RIVColorProperty *rayColorOne, RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo,RIVSliderView* sliderView) :
RIVDataView(datasetOne,datasetTwo), pathColorOne(pathColorOne), rayColorOne(rayColorOne),pathColorTwo(pathColorTwo), rayColorTwo(rayColorTwo), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo), sliderView(sliderView)
{
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	linesAreDirty = true;
	axesAreDirty = true;
	identifier = "ParallelCoordsView";
	instance = this;
}

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty* pathColor, RIVColorProperty *rayColor,RIVSliderView* sliderView) : RIVDataView(dataset,x,y,width,height, paddingX, paddingY), distributionsOne(distributionsOne),sliderView(sliderView) {
    if(instance != NULL) {
        throw "Only 1 instance allowed.";
    }
	this->pathColorOne = pathColor;
	this->rayColorOne = rayColor;
    linesAreDirty = true;
    axesAreDirty = true;
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
    linesAreDirty = true;
    axesAreDirty = true;
    identifier = "ParallelCoordsView";
	instance = this;
}

void ParallelCoordsView::createAxes() {
	printf("Create axes\n");
    axisGroups.clear();
	size_t total_nr_of_records = (*datasetOne)->TotalNumberOfRecords();
	//    int y = startY + paddingY;
	int y = 1.5F * paddingY; //View port takes care of startY
	int axisHeight = height - 2.5F * paddingY;
	int axisWidth = 20;

	float delta = 1.F / (total_nr_of_records - 1) * (width - 2 * paddingX);
	std::vector<RIVTable<float,ushort>*>* tablePointers = (*datasetOne)->GetTables();
	size_t axisIndex = 0;
	//		std::vector<std::string> pathTableRecordNames = {"pixel x","pixel y","R","G","B","throughput R","throughput G","throughput B","renderer","depth"};
	const int divisionCount = 4;
	if(datasetTwo) {
		for(size_t j = 0 ; j < tablePointers->size() ; j++) {
			std::vector<RIVTable<float,ushort>*>* otherTablePointers = (*datasetTwo)->GetTables();
			RIVTable<float,ushort> *table = tablePointers->at(j);
			RIVTable<float,ushort> *otherTable = otherTablePointers->at(j);
			
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
		for(size_t j = 0 ; j < tablePointers->size() ; j++) {
			RIVTable<float,ushort> *table = tablePointers->at(j);
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

					axisGroup.CreateAxis(record, x, y, axisHeight, axisWidth, minMax.first, minMax.second, name, divisionCount, dataHistOne);
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
					printf("Drawing selection box on axis %s\n",selectedAxis.c_str());
					
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
				
				auto& histogramOne = axis->densityHistogramOne;
				auto& histogramTwo = axis->densityHistogramTwo;
				
				int numBins = histogramOne.NumberOfBins();
				int startX = axis->x - axis->width / 2.F;
				int endX = startX + axis->width;
				int height = axis->height / (float)numBins;
				
				float maxValueOne = histogramOne.MaximumValue();
				float maxValueTwo = histogramTwo.MaximumValue();
				
//				printf("Histogram one \n");
//				histogramOne->Print();
//				printf("Histogram two \n");
//				histogramTwo->Print();
				
				for(int i = 0 ; i < numBins ; ++i) {
					int startY = i * height + axis->y;
					int endY = startY + height;
//					int centerY = startY + (endY - startY) / 2.F;
					
					float binValueOne = histogramOne.BinValue(i);
					float binValueTwo = histogramTwo.BinValue(i);
					
					size_t nrElementsOne = histogramOne.NumberOfElements();
					size_t nrElementsTwo = histogramTwo.NumberOfElements();
					
					float normalizedValueOne = 0;
					float normalizedValueTwo = 0;
					if(nrElementsOne)
						normalizedValueOne = binValueOne / histogramOne.NumberOfElements();
					if(nrElementsTwo)
						normalizedValueTwo = binValueTwo / histogramTwo.NumberOfElements();
					
//					float valueDelta = valueTwo - valueTwo;
					
					float blueColorValue = ((normalizedValueTwo - normalizedValueOne) / normalizedValueTwo + 1) / 2.F;
//					float redColorValue = ((normalizedValueOne - normalizedValueTwo) / normalizedValueOne + 1) / 2.F;
					
					float h,s,v;
					RGBtoHSV(1-blueColorValue, 0, blueColorValue, &h, &s, &v);
					
//					if(redColorValue < 0) {
//						redColorValue = 0;
//					}
//					if(blueColorValue < 0) {
//						blueColorValue = 0;
//					}
			
//					float sInterpolated;
//					if(blueColorValue > 0.5F) {
//						sInterpolated = binValueOne / maxValueOne;
//					}
//					else {
//						sInterpolated = binValueTwo / maxValueTwo;
//					}
//					
//					if(sInterpolated < 0.3) {
//												sInterpolated = 0.3;
//					}
//					else if(sInterpolated > 0.7) {
//						sInterpolated = 0.7;
//					}
//					printf("sInterpolated = %f\n",sInterpolated);
					
					float r,g,b;
					HSVtoRGB(&r, &g, &b, h, s, v);
					
					glColor3f(r, g, b);
					glRectf(startX, startY, endX, endY);
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
	
    glColor3f(1,1,1);
    glLineWidth(1.F);
	
//    for(auto& axisGroup : axisGroups) {
//		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
//			for(auto& axis : tAxes) {
//				float halfWidth = axis->width / 2.F;
//				
//				glRectf(axis->x - halfWidth, axis->y, axis->x + halfWidth, axis->y + axis->height);
//			}
//		});
//    }
	
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
    axesAreDirty = false;
}

void ParallelCoordsView::drawLines(int datasetId, RIVDataSet<float,ushort>* dataset, RIVColorProperty* pathColors, RIVColorProperty* rayColors) {
//	linesAreDirty = true;

	size_t lineIndex = 0;
	
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
//	printf("Drawing lines for dataset %d\n",dataset->datasetID);
//	reporter::startTask("drawLines");
	
	for(auto &axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto axis : tAxes) {
				axis->GetHistogram(datasetId)->Clear();
			}
		});
	}
	
	glLineWidth(1);
	for(auto &axisGroup : axisGroups) {
		size_t row = 0;
		auto table = dataset->GetTable(axisGroup.tableName);
		TableIterator* iterator = table->GetIterator();
		
		//Find what color property to use for this table
		RIVColorProperty* colorProperty = pathColors;
		if(table->GetName() == INTERSECTIONS_TABLE) {
			colorProperty = rayColors;
		}

		//You gotta love 'auto'!
		riv::Color lineColor;
		while(iterator->GetNext(row)) {
			if(colorProperty->ComputeColor(table, row, lineColor)) {
				glColor4f(lineColor.R, lineColor.G, lineColor.B, 0.1F);
				glBegin(GL_LINE_STRIP);
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto axis : tAxes) {
						
						auto record = table->GetRecord<decltype(axis->minValue)>(axis->name);
						auto value = record->Value(row);
						
						axis->GetHistogram(datasetId)->Add(value);
						
						float x = axis->x;
						float y = axis->PositionOnScaleForValue(value);
						float halfWidth = axis->width / 2.F;
						
						glVertex3f(x - halfWidth, y, 0);
						glVertex3f(x + halfWidth, y, 0);
					}
					++lineIndex;
				});
				glEnd();
			}
		}
		linesAreDirty = false;
	}
//	printf("Parallel coordinates view drew %zu polylines\n",lineIndex);
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
    
	//    printf("New width = %d\n",width);
	//    printf("New height = %d\n",height);
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    paddingX = 30;
    paddingY = 20;
    
    axesAreDirty = true;
    linesAreDirty = true;
	
	InitializeGraphics();
}

void ParallelCoordsView::InitializeGraphics() {
	//Recreate the axes according to the new layout
//	createAxes();
}

size_t drawCount = 0;
void ParallelCoordsView::Draw() {
	//    printf("linesAreDirty = %d axesAreDirty = %d\n",linesAreDirty,axesAreDirty);
//	printHeader("PC VIEW DRAW");
	
	std::string taskName = "ParallelCoordsView Draw";
	reporter::startTask(taskName);
	printf("\n");
	
	if(axesAreDirty && linesAreDirty && selectionIsDirty) {
//		printf("Clear PCV window\n");
        glClearColor(0.9, 0.9, 0.9, 0.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
        
        //Draw the lines from each axis
	if(linesAreDirty) {
		if(drawDataSetOne)
			drawLines(0,*datasetOne,pathColorOne,rayColorOne);
		if(datasetTwo) {
			if(drawDataSetTwo)
				drawLines(1,*datasetTwo,pathColorTwo,rayColorTwo);
		}
	}
	
	drawAxes();
	
	//Draw the axes
	if(axesAreDirty)
		drawAxesExtras();
	
	if(selectionIsDirty)
		drawSelectionBoxes();
	
	glFlush();
	glutSwapBuffers();
	
	reporter::stop(taskName);
}

void ParallelCoordsView::toggleDrawDataSetOne() {
	drawDataSetOne = !drawDataSetOne;
	
	axesAreDirty = true;
	selectionIsDirty = true;
	linesAreDirty = true;
	
	redisplayWindow();
}

void ParallelCoordsView::toggleDrawDataSetTwo() {
	drawDataSetTwo = !drawDataSetTwo;
	
	axesAreDirty = true;
	selectionIsDirty = true;
	linesAreDirty = true;
	
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
			
			for(int i = 0 ; i < axisGroups.size() ; i++) {
				
				auto& axisGroup = axisGroups[i];
				bool found = false;
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto& axis : tAxes) {
						
						int distanceX = abs(axis->x - x);
						
						if(distanceX < proximityMax) {
							if(axis->HasSelectionBox) {
								//Is it inside the selection box?
								if((y >= axis->selection.start.y && y <= axis->selection.end.y) || (y >= axis->selection.end.y && y <= axis->selection.start.y)) {
									selectedAxis = axis->name;
									dragSelectionBox = true;
									createSelectionBox = false;
									startDragBoxY = y;
									dragBoxLastY = y;
									
									found = true;
									
									axesAreDirty = true;
									linesAreDirty = true;
									selectionBoxChanged = false;
									
									return;
								}
							}
							//Is it at least inside the axis
							else if(y > axis->y) {

								if(y < axis->y + axis->height) {
									
									//Close enough..
									axis->selection.start.x = axis->x - 10;
									axis->selection.end.x = axis->x + 10;
									axis->selection.start.y = y;
									axis->selection.end.y = y;
									
									startDragBoxY = y;
									
									axis->HasSelectionBox = true;
									found = true;
									
									selectedAxis = axis->name;
									selection = &axis->selection;
									
									selectionBoxChanged = false;
									createSelectionBox = true;
									axesAreDirty = true;
									linesAreDirty = true;
									
									printf("Selected axis %s\n",axis->name.c_str());
									return;
								}
							}
							else {
								printf("Toggle selection of axis %s.\n",axis->name.c_str());
								axis->isSelected = !axis->isSelected;
								
								//Deselect all the other table records
								for(int j = 0 ; j < axisGroups.size() ; j++) {
									if(i != j) {
										auto& otherAxisGroup = axisGroups[j];
										axisGroup.isSelected = false;
										tuple_for_each(otherAxisGroup.axes, [&](auto otherTAxes) {
											for(auto& otherAxis : otherTAxes) {
												otherAxis->isSelected = false;
											}
										});
									}
								}
					
								if(axis->isSelected) {
									sliderView->AddSelectedRecord(axisGroup.tableName, axis->name);
								}
								else {
									sliderView->RemoveSelectedRecord(axis->name);
								}
								
								axesAreDirty = true;
								linesAreDirty = true;
								
								found = true;
								
								glutPostRedisplay();
								
								return;
							}
						}
					}
				});
				if(found) {
					return true;
				}
			}
			selectedAxis.clear();
        }
        else if(state == GLUT_UP) { //Finish selection
            if(selectionBoxChanged && selectedAxis.size() > 0) {
				filterData();
            }
			else {
				for(auto& axisGroup : axisGroups) {
					tuple_for_each(axisGroup.axes, [&](auto tAxes) {
						for(auto& axis : tAxes) {
							if(axis->name == selectedAxis) {
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
								axis->HasSelectionBox = false;
								selectedAxis.clear();
								(*datasetOne)->StopFiltering();
								if(datasetTwo) {
									(*datasetTwo)->StopFiltering();
								}
							}
						}
					});
				}
			}
            return true;
        }
		return true;
	}
    return false;
}

void ParallelCoordsView::filterData() {
	for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto& axis : tAxes) {
				if(axis->name == selectedAxis) {
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
						selectedAxis.clear();
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

bool ParallelCoordsView::HandleMouseMotion(int x, int y) {
    y = height - y;
	if(selectedAxis.size()) {
		if(createSelectionBox) {
			//TODO: Not very efficient to find the axis everytime, but pointer sucks due to templates
			for(auto& axisGroup : axisGroups) {
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto& axis : tAxes) {
						if(axis->name == selectedAxis) {
							selection->end.y = axis->PositionOnScaleForViewY(y);
						}
					}
				});
			}
			if(std::abs(y - startDragBoxY) > 3) {
				startDragBoxY = y;
				filterData();
				linesAreDirty = true;
				selectionBoxChanged = true;
			}
			axesAreDirty = true;
			selectionIsDirty = true;
			glutPostRedisplay();
			return true;
		}
		else if(dragSelectionBox) {
			int deltaY = y - dragBoxLastY;
			for(auto& axisGroup : axisGroups) {
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto& axis : tAxes) {
						if(axis->name == selectedAxis) {
							selection->start.y += deltaY;
							selection->end.y += deltaY;
						}
					}
				});
			}
			if(std::abs(y - startDragBoxY) > 3) {
				startDragBoxY = y;
				filterData();
				linesAreDirty = true;
				selectionBoxChanged = true;
			}
			dragBoxLastY = y;
			selectionIsDirty = true;
			glutPostRedisplay();
			return true;
		}
	}
    return false;
}

void ParallelCoordsView::OnDataChanged(RIVDataSet<float,ushort>* source) {
	//Recreate the axes
	printf("ParallelCoordsView onDataChanged notified.\n");
	
	axesAreDirty = true;
	linesAreDirty = true;
	
	//Recreate the color property
	if(source == (*datasetOne)) {
		
		auto rgbPath = dynamic_cast<RIVColorRGBProperty<float>*>(pathColorOne);
		auto rgbRay = dynamic_cast<RIVColorRGBProperty<float>*>(rayColorOne);
		
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
		auto rgbPath = dynamic_cast<RIVColorRGBProperty<float>*>(pathColorTwo);
		auto rgbRay = dynamic_cast<RIVColorRGBProperty<float>*>(rayColorTwo);
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
	createAxes();
	
	redisplayWindow();
}

void ParallelCoordsView::redisplayWindow() {
	int currentWindow = glutGetWindow();
	glutSetWindow(ParallelCoordsView::windowHandle);
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
}

void ParallelCoordsView::OnFiltersChanged() {
    printf("ParallelCoordsView received a on filters changed callback.\n");
	
    linesAreDirty = true;
    axesAreDirty = true;
	
	redisplayWindow();
}
