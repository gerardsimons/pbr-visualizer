//
//  SliderView.cpp
//  embree
//
//  Created by Gerard Simons on 03/01/15.
//
//

#include "SliderView.h"
#include "../Configuration.h"
#include "../Graphics/graphics_helper.h"

RIVSliderView* RIVSliderView::instance = NULL;
int RIVSliderView::windowHandle = -1;

void RIVSliderView::resetPointers() {
	pointerOneX = leftBound;
	pointerTwoX = rightBound - pointerWidth;
}

RIVSliderView::RIVSliderView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo, const riv::ColorMap& colorMap) : RIVDataView(datasetOne,datasetTwo), colorMap(colorMap), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo) {
	if(!instance) {
		instance = this;
	}
	else {
		throw std::runtime_error("Only 1 instance of sliderview allowed.");
	}
	
	uniquenessHistogramOne = Histogram<float>("uniqueness_one", 0, 1, histogramBins);
	uniquenessHistogramTwo = Histogram<float>("uniqueness_two", 0, 1, histogramBins);
}

void RIVSliderView::Mouse(int state,int button,int x,int y) {
	if(instance) {
		instance->HandleMouse(state, button, x, y);
	}
}
void RIVSliderView::Motion(int x, int y) {
	if(instance) {
		instance->HandleMouseMotion(x, y);
	}
}
void RIVSliderView::DrawInstance() {
	if(instance != NULL) {
		instance->Draw();
	}
	else {
		printf("No instance to draw.\n");
	}
}

void RIVSliderView::ReshapeInstance(int width, int height) {
	if(instance != NULL) {
		instance->Reshape(width,height);
	}
	else printf("No instance to reshape");
}

void RIVSliderView::Draw() {
	
	if(needsRedraw) {
		
		printHeader("DRAW SLIDERVIEW",60);
		
		glClearColor(1,1,1,0);
		glClear( GL_COLOR_BUFFER_BIT );
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glBegin(GL_QUAD_STRIP);
		
		//Left
		glColor3f(1,0,0);
		glVertex2f(leftBound, bottom);
		glVertex2f(leftBound, top);
		
		//Right
		glColor3f(0,0,1);
		glVertex2f(rightBound, bottom);
		glVertex2f(rightBound, top);
		
		glEnd();
		
		//Draw pointers
		glColor3f(0, 0, 0);
		glBegin(GL_QUADS);
		
		glVertex2f(pointerOneX, top);
		glVertex2f(pointerOneX, bottom);
		glVertex2f(pointerOneX + pointerWidth, bottom);
		glVertex2f(pointerOneX + pointerWidth, top);
		
		glColor3f(1,1,0);
		glVertex2f(pointerTwoX, top);
		glVertex2f(pointerTwoX, bottom);
		glVertex2f(pointerTwoX + pointerWidth, bottom);
		glVertex2f(pointerTwoX + pointerWidth, top);
		
		glEnd();
		
		int barWidth = rightBound - leftBound;
		
		//Draw uniqueness histogram one
//		int bins = uniquenessHistogramOne.NumberOfBins();
	//	float maxHeight = top;
	//	float minHeight = bottom;
		
		//Draw scale
		float binWidth = barWidth / (float)histogramBins / 2.F;
		float maxBarHeight = top - bottom;
		float x = leftBound;
		
		glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		float black[] = {0,0,0};
		
		for(int i = 0 ; i < 2*histogramBins ; ++i) {
			glVertex2f(x, bottom);
			glVertex2f(x, top);
			
			x += binWidth;
		}
		
		glEnd();
		x = leftBound;
		for(int i = histogramBins ; i > 0 ; --i) {
			glVertex2f(x, bottom);
			glVertex2f(x, top);
			
//			drawText(std::to_string(i/(float)histogramBins), x, top-10, black, .1F);
			x += binWidth;
		}
		
		x = barWidth / 2.F - binWidth;

		//Draw histogram bars of the left (red) dataset
		printf("LEFT HISTOGRAM BARS DRAWING : \n\n");
		uniquenessHistogramOne.Print();
		glColor3f(1, 0, 0);
		for(int i = 0 ; i < histogramBins ; ++i) { //Draw them backwards as the most unique should be to the left
			float histValue = uniquenessHistogramOne.NormalizedValue(i);
			float barHeight = histValue * maxBarHeight;
//			printf("bar height (i = %d) = %f\n",i,barHeight);
	//		printf("glRectf(%f,%f,%f,%f)\n",x,bottom,x+binWidth,bottom+barHeight);
			glRectf(x, bottom, x+binWidth, bottom + barHeight);
			x -= binWidth;
		}
		x = barWidth / 2.F;
		printf("RIGHT HISTOGRAM BARS DRAWING : \n\n");
		uniquenessHistogramTwo.Print();
//		x = leftBound + (barWidth / 2.F);
		//Draw histogram bars of the left (red) dataset
		glColor3f(0, 0, 1);
		for(int i  = 0 ; i < histogramBins ; ++i) { //Draw them backwards as the most unique should be to the left
			float histValue = uniquenessHistogramTwo.NormalizedValue(i);
			float barHeight = histValue * maxBarHeight;
//			printf("bar height (i = %d) = %f\n",i,barHeight);
	//		printf("glRectf(%f,%f,%f,%f)\n",x,bottom,x+binWidth,bottom+barHeight);
			glRectf(x, bottom, x+binWidth, bottom + barHeight);
			x += binWidth;
		}
		
		glFlush();
		glutSwapBuffers();
			
	}
}
void RIVSliderView::Reshape(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	
	bottom = paddingY;
	top = height - paddingY;
	
	leftBound = paddingX;
	rightBound = width - 2 * paddingX;
	
	resetPointers();
	
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void RIVSliderView::createHistograms(RIVDataSet<float,ushort>* datasetSource) {
	
	printf("Creating uniqueness histogram\n\n");
	
	auto tables = datasetSource->GetTables();

	for(size_t i = 0 ; i < tables->size() ; ++i) {
		auto table = tables->at(i);
		size_t row = 0;
		TableIterator* iterator = table->GetIterator();
		size_t totalNumberRecords = table->NumberOfRecords();
		while(iterator->GetNext(row)) {
			float totalDiff = 0;
			auto& allRecords = table->GetAllRecords();
			tuple_for_each(allRecords, [&](auto tRecords) {
				for(auto record : tRecords) {
					
					auto value = record->Value(row);
					
					typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
					
					auto histogramOne = distributionsOne->GetHistogram<Type>(record->name);
					auto histogramTwo = distributionsTwo->GetHistogram<Type>(record->name);
					
					//What bin does this value belong to?
					int bin = histogramOne->BinForValue(value);
					
					//Dataset one
					float binValueOne = histogramOne->NormalizedValue(bin);
					float binValueTwo = histogramTwo->NormalizedValue(bin);
					//						float binValueTwo = histogramTwo->NormalizedValue(bin);
					//						float binValueDelta;
					
					float diff = binValueTwo - binValueOne;
					totalDiff += diff;
				}
			
			});
			//Normalize it
			totalDiff /= totalNumberRecords;
//			printf("totalDiff = %f\n",totalDiff);
			if(totalDiff > 0) { //Belongs to right dataset
				uniquenessHistogramTwo.Add(totalDiff);
			}
			else if(totalDiff < 0) {
				uniquenessHistogramOne.Add(-totalDiff);
			}
		}
	}
	printf("uniqueness histogram one : \n");
	uniquenessHistogramOne.Print();
	printf("uniqueness histogram two : \n");
	uniquenessHistogramTwo.Print();
	
}
void RIVSliderView::filterDataSet(RIVDataSet<float,ushort>* dataset, HistogramSet<float,ushort>* distributions, bool isLeftSet, float minBound, float maxBound) {
	
	auto tables = dataset->GetTables();
	
	printHeader("MEMBERSHIP FILTERING");
//	if(isLeftSet) {
////		minBound += 1;
//		maxBound = 0;
//	}
//	else {
//		minBound = 0;
//	}
	size_t rowsFiltered = 0;
	printf("minbound = %f\n",minBound);
	printf("maxBound = %f\n",maxBound);
	dataset->StartFiltering();
	for(size_t i = 0 ; i < tables->size() ; ++i) {
		auto table = tables->at(i);
		//Row filter that manually filters rows according to their row number
		std::map<size_t,bool> selectedRows;
		size_t row = 0;
		TableIterator* iterator = table->GetIterator();
		table->ClearRowFilters();
		size_t totalNumberRecords = table->NumberOfRecords();
		while(iterator->GetNext(row)) {
			float totalDiff = 0;
			auto& allRecords = table->GetAllRecords();
			tuple_for_each(allRecords, [&](auto tRecords) {
				if(selectedRows[row] == false) {
					for(auto record : tRecords) {
						
						auto value = record->Value(row);
						
						typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
						
						auto histogramOne = distributionsOne->GetHistogram<Type>(record->name);
						auto histogramTwo = distributionsTwo->GetHistogram<Type>(record->name);
						
						//What bin does this value belong to?
						int bin = histogramOne->BinForValue(value);
						
						//Dataset one
						float binValueOne = histogramOne->NormalizedValue(bin);
						float binValueTwo = histogramTwo->NormalizedValue(bin);
						//						float binValueTwo = histogramTwo->NormalizedValue(bin);
						//						float binValueDelta;
						
						float diff = binValueTwo - binValueOne;
						totalDiff += diff;
						
//						auto histogramOne = distributions->GetHistogram<Type>(record->name);
////						auto histogramTwo = distributionsTwo->GetHistogram<Type>(record->name);
//						
//						//What bin does this value belong to?
//						int bin = histogramOne->BinForValue(value);
//						
//						//Dataset one
//						float binValue = histogramOne->NormalizedValue(bin);
////						float binValueTwo = histogramTwo->NormalizedValue(bin);
////						float binValueDelta;
//						
//						float diff = std::min(binValue - minBound,maxBound - binValue);
//						totalDiff += diff;
					}
				}
			});
			totalDiff /= totalNumberRecords;
//			printf("Total diff = %f\n",totalDiff);
//			if(isLeftSet) {
				if((totalDiff >= minBound && totalDiff <= maxBound)) {
					selectedRows[row] = true;
				}
				else {
//					printf("Row %zu did not pass the membership test\n",row);
					++rowsFiltered;
				}
//			}
//			else if(totalDiff > 0) {
//				if((totalDiff >= minBound && totalDiff <= maxBound)) {
//					selectedRows[row] = true;
//				}
//			}
		}
		riv::RowFilter* rowFilter = new riv::RowFilter(table->name,selectedRows);

		dataset->AddFilter(rowFilter);
	}
	dataset->StopFiltering();
	if(rowsFiltered) {
		printf("%zu rows filtered : \n\n",rowsFiltered);
//		dataset->PrintFilteredRows();
	}
	else {
		printf("Nothing filtered.\n");
	}
	printf("\n");
//	dataset->Print();
}
void RIVSliderView::filterDataSets() {
	float minBound = (pointerOneX - leftBound) / (float)(rightBound - leftBound) * 2 - 1;
	float maxBound = (pointerTwoX - leftBound) / (float)(rightBound - leftBound) * 2 - 1;
	
	if(minBound > maxBound) {
		float temp = minBound;
		minBound = maxBound;
		maxBound = temp;
	}
	
//	minBound = -1;
//	maxBound = 1;
	
	printf("SLIDER MIN BOUND = %f\n",minBound);
	printf("SLIDER MAX BOUND = %f\n",maxBound);

	printf("LEFT DATASET FILTER BINS:  \n");
	filterDataSet(*datasetOne, distributionsOne, true, minBound, maxBound);
	printf("RIGHT DATASET FILTER BINS:  \n");
	filterDataSet(*datasetTwo, distributionsTwo, false, minBound, maxBound);
}
bool RIVSliderView::HandleMouse(int button, int state, int x, int y) {
	float minDistance = 10;
	bool mouseCaught = false;
	printf("state = %d\n",state);
	if(state == GLUT_DOWN) {
		//Is there a pointer close by?
		
		int distance = abs(pointerOneX - x);
		int distanceTwo = abs(pointerTwoX - x);
		if(distance < minDistance) {
			selectedPointer = &pointerOneX;
			printf("Pointer one selected.\n");
			mouseCaught = true;
		}
		else if(distanceTwo < minDistance) {
			selectedPointer = &pointerTwoX;
			printf("Pointer two selected.\n");
			mouseCaught = true;
		}
		else if(x - pointerOneX > 0 && x - pointerTwoX < 0) { //It is in between the two pointers, try and move them both
			movePointers = true;
			startDragX = x;
		}
	}
	else if(state == GLUT_UP) {
		int distance = abs(x - startDragX);
		if(selectedPointer || (movePointers && distance > 1)) {
			selectedPointer = NULL;
			printf("Pointer de-selected.\n");
			mouseCaught = true;
			
			filterDataSets();
			movePointers = false;
		}
	}
	if(mouseCaught) {
		redisplayWindow();
	}
	return mouseCaught;
}
bool RIVSliderView::HandleMouseMotion(int x, int y) {
//	printf("Slider mouse motion(%d,%d)\n",x,y);
	if(selectedPointer && x >= paddingX && x < width - 2 * paddingX) {
		*selectedPointer = x;
		Invalidate();
		glutPostRedisplay();
		return true;
	}
	else if(movePointers) {
		
		int deltaX = x - startDragX;
		
//		printf("DeltaX = %d\n",deltaX);
		
		int newPointerOneX = startPointerOneDragX + deltaX;
		int newPointerTwoX = startPointerTwoDragX + deltaX;
		
		if(newPointerOneX < rightBound && newPointerTwoX < rightBound && newPointerOneX > leftBound && newPointerTwoX > leftBound) {
			pointerOneX = newPointerOneX;
			pointerTwoX = newPointerTwoX;
			
			Invalidate();
			glutPostRedisplay();
			return true;
		}
	}
	return false;
}
void RIVSliderView::redisplayWindow() {
	int currentWindow = glutGetWindow();
	glutSetWindow(RIVSliderView::windowHandle);
	needsRedraw = true;
	glutPostRedisplay();
	//Return window to given window
	glutSetWindow(currentWindow);
}
//Dataset listener functions
void RIVSliderView::OnFiltersChanged() {
	//Do nothing...?
}
void RIVSliderView::OnDataChanged(RIVDataSet<float,ushort>* source) {
	printf("*** RIVSliderView received a on dataset changed notification!\n" );
	if(source == *datasetOne) {
		printHeader("CREATE MEMBERSHIP HISTOGRAM ONE");
		uniquenessHistogramOne.Clear();
		createHistograms(source);
	}
	else if(source == *datasetTwo) {
		uniquenessHistogramTwo.Clear();
		printHeader("CREATE MEMBERSHIP HISTOGRAM TWO");
		createHistograms(source);
	}
	else {
		printf("UNKNOWN DATASET\n");
	}
	
	redisplayWindow();
	
}
