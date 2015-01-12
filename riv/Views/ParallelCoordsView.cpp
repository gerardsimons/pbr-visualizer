#include "ParallelCoordsView.h"
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

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *pathColorOne, RIVColorProperty *rayColorOne,RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo) : RIVDataView(datasetOne,datasetTwo,x,y,width,height,paddingX,paddingY), pathColorOne(pathColorOne), rayColorOne(rayColorOne), pathColorTwo(pathColorTwo), rayColorTwo(rayColorTwo), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo) {
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	linesAreDirty = true;
	axesAreDirty = true;
	identifier = "ParallelCoordsView";
	instance = this;
}
ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, RIVColorProperty *pathColorOne, RIVColorProperty *rayColorOne, RIVColorProperty *pathColorTwo, RIVColorProperty *rayColorTwo) :
RIVDataView(datasetOne,datasetTwo), pathColorOne(pathColorOne), rayColorOne(rayColorOne),pathColorTwo(pathColorTwo), rayColorTwo(rayColorTwo), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo)
{
	if(instance != NULL) {
		throw "Only 1 instance allowed.";
	}
	linesAreDirty = true;
	axesAreDirty = true;
	identifier = "ParallelCoordsView";
	instance = this;
}

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty* pathColor, RIVColorProperty *rayColor) : RIVDataView(dataset,x,y,width,height, paddingX, paddingY), distributionsOne(distributionsOne) {
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

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** dataset, HistogramSet<float,ushort>* distributionsOne, RIVColorProperty *pathColor, RIVColorProperty* rayColor) : RIVDataView(dataset), distributionsOne(distributionsOne) {
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
	int y = paddingY; //View port takes care of startY
	int axisHeight = height - 2 * paddingY;
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
					//				printf("Record %s has min-max : ",record->name.c_str());
					//				std::cout << " " << minMax.first << ", " << minMax.second << std::endl;
					std::string name = record->name;
					axisGroup.CreateAxis(record, x, y, axisWidth, axisHeight, std::min(minMax.first,otherMinMax.first), std::max(minMax.second,otherMinMax.second), record->name, divisionCount,distributionsOne->GetHistogram<Type>(name),distributionsTwo->GetHistogram<Type>(name));
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
					
					axisGroup.CreateAxis(record, x, y, axisHeight, axisWidth, minMax.first, minMax.second, name, divisionCount, distributionsOne->GetHistogram<Type>(name));
					axisIndex++;
				}
			});
			axisGroups.push_back(axisGroup);
		}
	}
	printf("Create axes finished\n");
}

void ParallelCoordsView::drawSelectionBoxes() {
	if(selectedAxis.size()) {
		for(auto& axisGroup : axisGroups) {
			tuple_for_each(axisGroup.axes, [&](auto tAxes) {
				for(auto& axis : tAxes) {
					if(axis->HasSelectionBox) {
						printf("Drawing selection box on axis %s\n",selectedAxis.c_str());
						
						glColor3f(1, 0, 0);
						glBegin(GL_LINE_STRIP);
						
						glVertex3f(axis->selection.start.x,axis->selection.start.y,0);
						glVertex3f(axis->selection.end.x,axis->selection.start.y,0);
						
						glVertex3f(axis->selection.end.x,axis->selection.end.y,0);
						glVertex3f(axis->selection.start.x,axis->selection.end.y,0);
						
						glVertex3f(axis->selection.start.x,axis->selection.start.y,0);
						
						glEnd();
					}
				}
			});
		}
	}
}
void ParallelCoordsView::drawDensities() {
	//Draw the densities
	float textColor[3] = {1,1,0};
	float epsilon = 0.01F;
	
	for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto axis : tAxes) {
				
				auto histogramOne = axis->densityHistogramOne;
				auto histogramTwo = axis->densityHistogramTwo;
				
				int numBins = histogramOne->NumberOfBins();
				int startX = axis->x - axis->width / 2.F;
				int endX = startX + axis->width;
				int height = axis->height / (float)numBins;
				
//				printf("Histogram one \n");
//				histogramOne->Print();
//				printf("Histogram two \n");
//				histogramTwo->Print();
				
				for(int i = 0 ; i < numBins ; ++i) {
					int startY = i * height + axis->y;
					int endY = startY + height;
//					int centerY = startY + (endY - startY) / 2.F;
					
					float valueOne = histogramOne->NormalizedValue(i);
					float valueTwo = histogramTwo->NormalizedValue(i);
					
					float valueDelta = valueOne - valueTwo;
					
					//If the values are rougly equal
					if(valueDelta < epsilon && valueDelta > -epsilon) {
						glColor3f(.8F, .8F, .8F);
						continue;
					}
					if(valueOne > valueTwo) {
						float colorValue = valueDelta / valueOne;
//						std::string text = std::to_string(colorValue);
//						drawText(text,axis->x,centerY - 15,textColor,.1F);
//						printf("glColor3f(%f,0,0)\n",colorValue);
						glColor3f(colorValue, 0, 1-colorValue);
					}
					else {
						float colorValue = -valueDelta / valueTwo;
//						std::string text = std::to_string(colorValue);
//						drawText(text,axis->x,centerY - 15,textColor,.1F);
//						printf("glColor3f(0,0,%f)\n",colorValue);
						glColor3f(1-colorValue, 0, colorValue);
					}
					glRectf(startX, startY, endX, endY);
				}
				
				glColor3f(0,0,0);
				glBegin(GL_LINES);
				for(int i = 0 ; i < numBins ; ++i) {
					int y = i * height + axis->y;
					glVertex2f(startX, y);
					glVertex2f(startX + axis->width, y);
				}
				glEnd();
			}
		});
	}
}

void ParallelCoordsView::drawAxes() {
	
    glColor3f(1,1,1);
    glLineWidth(1.F);
	
//    glBegin(GL_LINES);
    for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto& axis : tAxes) {
				float halfWidth = axis->width / 2.F;
				
				glRectf(axis->x - halfWidth, axis->y, axis->x + halfWidth, axis->y + axis->height);
				
//				glVertex3f(axis->x,axis->y,0);
//				glVertex3f(axis->x,axis->y+axis->height,0);
			}
		});
    }
//    glEnd();
	
    glColor3f(1,0,0);
    glLineWidth(1.F);
    
    float textColor[3] = {0,0,0};
    
    //Draw texts
	
	for(auto& axisGroup : axisGroups) {
		tuple_for_each(axisGroup.axes, [&](auto tAxes) {
			for(auto& axis : tAxes) {
				std::string& text = axis->name;
				
				drawText(text,axis->x,axis->y - 15,textColor,.1F);
				
				auto &scale = axis->scale;
				
				//Draw the scales indicators
				if(scale.size() > 0) {
					for(size_t j = 0 ; j < scale.size() ; j++) {
						float value = axis->ValueOnScale(scale[j]);
						int height = axis->PositionOnScaleForScalar(scale[j]);
						
						//Draw small line to indicate
						//					glBegin(GL_LINES);
						//					glVertex2f(axis.x, height);
						//					glVertex2f(axis.x + 10, height);
						//					glEnd();
						
						std::string text = std::to_string(value);
						
						char buffer[4];
						sprintf(buffer,"%.2f",value);
						
						drawText(buffer,4,axis->x - 12,height,textColor,.1F);
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
	
	printf("Drawing lines for dataset %d\n",dataset->datasetID);
	reporter::startTask("drawLines");
	
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
	printf("Parallel coordinates view drew %zu polylines\n",lineIndex);
	reporter::stop("drawLines");
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
//	createAxes();
}

size_t drawCount = 0;
void ParallelCoordsView::Draw() {
	//    printf("linesAreDirty = %d axesAreDirty = %d\n",linesAreDirty,axesAreDirty);
	
	printHeader("PC VIEW DRAW");
	
	std::string taskName = "ParallelCoordsView Draw";
	reporter::startTask(taskName);
	printf("\n");
	
	if(axesAreDirty && linesAreDirty && selectionIsDirty) {
		printf("Clear PCV window\n");
        glClearColor(0.9, 0.9, 0.9, 0.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
		
	//Draw the axes
	if(axesAreDirty)
        drawAxes();
        
        //Draw the lines from each axis
	if(linesAreDirty) {
		if(drawDataSetOne)
			drawLines(0,*datasetOne,pathColorOne,rayColorOne);
		if(datasetTwo) {
			if(drawDataSetTwo)
				drawLines(1,*datasetTwo,pathColorTwo,rayColorTwo);
		}
	}
	
	drawDensities();
	
	if(selectionIsDirty)
		drawSelectionBoxes();
        
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

//Filter object from axis type
//template <typename T>
//RangeFilter* filter_from_axis(const ParallelCoordsAxis<T>& axis, T lowerBound, T upperBound) {
//	return new RangeFilter<T>(
//}

bool ParallelCoordsView::HandleMouse(int button, int state, int x, int y) {
	//    printf("PCV HandleMouse\n");
//    ToViewSpaceCoordinates(&x, &y);
    y = height - y;
	if(containsPoint(x,y) || isDragging) {
		//What axis was selected
        if(state == GLUT_DOWN) {
            int proximityMax = 50;
            
            //            printf("(x,y)=(%d,%d)",x,y);
			for(auto& axisGroup : axisGroups) {
				bool found = false;
				tuple_for_each(axisGroup.axes, [&](auto tAxes) {
					for(auto& axis : tAxes) {
						int distanceX = abs(axis->x - x);
	//                    printf("distance=%d\n",distanceX);
						
						if(distanceX < proximityMax) {
							//Close enough..
							
							axis->selection.start.x = axis->x - 10;
							axis->selection.end.x = axis->x + 10;
							axis->selection.start.y = y;
							axis->selection.end.y = y;
							
							axis->HasSelectionBox = true;
							found = true;
							
							selectedAxis = axis->name;
							selection = &axis->selection;
							
							isDragging = true;
							axesAreDirty = true;
							linesAreDirty = true;
							
	                        printf("Selected axis %s\n",axis->name.c_str());
							
							return;
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
            if(isDragging && selectedAxis.size() > 0) {
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
            return true;
        }
		return true;
	}
    return false;
}

bool ParallelCoordsView::HandleMouseMotion(int x, int y) {
    y = height - y;
    if(isDragging && selectedAxis.size()) {
		
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
		return true;
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
