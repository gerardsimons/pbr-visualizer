//
//  SliderView.cpp
//  embree
//
//  Created by Gerard Simons on 03/01/15.
//
//

#include <algorithm>
#include <set>

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
	
	membershipHistogramOne = Histogram<float>("uniqueness_one", 0, 1, histogramBins);
	membershipHistogramTwo = Histogram<float>("uniqueness_two", 0, 1, histogramBins);
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
		
//		printHeader("DRAW SLIDERVIEW",60);
		if(leftBound > 0) {
			
		}
		
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
//		int bins = membershipHistogramOne.NumberOfBins();
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
		int centerBar = height / 2.F;

		//Draw histogram bars of the left (red) dataset
//		printf("LEFT HISTOGRAM BARS DRAWING : \n\n");
//		membershipHistogramOne.Print();
		

		for(int i = 0 ; i < histogramBins ; ++i) { //Draw them backwards as the most unique should be to the left
			float histValue = membershipHistogramOne.NormalizedValue(i);
			float barHeight = histValue * maxBarHeight;
			
			glColor3f(1, 0, 0);
			glRectf(x, bottom, x+binWidth, bottom + barHeight);
			
			std::string nrMembers = std::to_string(rowBinMembershipsOne[i].size());
			drawText(nrMembers, x + binWidth / 2.F, centerBar, .07F);
			


//			printf("bar height (i = %d) = %f\n",i,barHeight);
	//		printf("glRectf(%f,%f,%f,%f)\n",x,bottom,x+binWidth,bottom+barHeight);


			x -= binWidth;
		}
		x = barWidth / 2.F;
//		printf("RIGHT HISTOGRAM BARS DRAWING : \n\n");
//		membershipHistogramTwo.Print();


		for(int i  = 0 ; i < histogramBins ; ++i) { //Draw them backwards as the most unique should be to the left
			std::string nrMembers = std::to_string(rowBinMembershipsTwo[i].size());

			float histValue = membershipHistogramTwo.NormalizedValue(i);
			float barHeight = histValue * maxBarHeight;
//			printf("bar height (i = %d) = %f\n",i,barHeight);
	//		printf("glRectf(%f,%f,%f,%f)\n",x,bottom,x+binWidth,bottom+barHeight);
			glColor3f(0, 0, 1);
			glRectf(x, bottom, x+binWidth, bottom + barHeight);
			
			drawText(nrMembers, x + binWidth / 2.F, centerBar, .07F);
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
void RIVSliderView::createMembershipData(RIVDataSet<float,ushort>* datasetSource, std::map<int,std::vector<size_t>>& rowBinMembership) {
	
//	printf("Creating membership data\n\n");
	
	std::set<std::string> selectedRecords;
//	selectedRecords.insert(INTERSECTION_R);
//	selectedRecords.insert(INTERSECTION_G);
//	selectedRecords.insert(INTERSECTION_B);
	
//	selectedRecords.insert(POS_X);
//	selectedRecords.insert(POS_Y);
//	selectedRecords.insert(POS_Z);
	
	selectedRecords.insert(PATH_R);
	selectedRecords.insert(PATH_G);
	selectedRecords.insert(PATH_B);
	
	rowBinMembership.clear();
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
					
					if(selectedRecords.find(record->name) != selectedRecords.end()) {
					
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
	//					if(diff > maxDifference) {
	//						maxDifference = diff;
	//					}
						totalDiff += diff;
					}
				}
//				sumMaxDifferences += maxDifference;
			});
			//Normalize it
			totalDiff /= selectedRecords.size();
//			printf("totalDiff = %f\n",totalDiff);
			if(totalDiff > 0) { //Belongs to right dataset
				int bin = membershipHistogramTwo.Add(totalDiff);
				rowBinMembership[bin].push_back(row);
			}
			else if(totalDiff < 0) {
				int bin = membershipHistogramOne.Add(-totalDiff);
				rowBinMembership[bin].push_back(row);
			}
		}
	}
//	membershipHistogramOne.Print();
//	membershipHistogramTwo.Print();
	
}
void RIVSliderView::filterDataSet(RIVDataSet<float,ushort>* dataset, HistogramSet<float,ushort>* distributions, bool isLeftSet, float minBound, float maxBound, std::map<int,std::vector<size_t>>& rowBinMembership) {
	
	auto tables = dataset->GetTables();
	dataset->ClearRowFilters();
	
//	printHeader("MEMBERSHIP FILTERING");
	if(isLeftSet) {
//		minBound += 1;
		minBound = std::min(0.F,minBound);
		maxBound = std::min(0.F,maxBound);
	}
	else {
		minBound = std::max(0.F,minBound);
		maxBound = std::max(0.F,maxBound);
	}
	int rowsFiltered = 0;
//	printf("minbound = %f\n",minBound);
//	printf("maxBound = %f\n",maxBound);
	dataset->StartFiltering();
	for(size_t i = 0 ; i < tables->size() ; ++i) {
		
		auto table = tables->at(i);
		
		//How many rows are going be filtered for this table
		int tableRowsFiltered = table->NumberOfRows();
		
		//Row filter that manually filters rows according to their row number
		std::map<size_t,bool> selectedRows;
		
		int binRight = floor(std::abs(minBound) * histogramBins);
		int binLeft = ceil(std::abs(maxBound) * histogramBins);
		
		//Make sure bin left really is the left most (smallest) bin
		if(binLeft > binRight) {
			int temp = binLeft;
			binLeft = binRight;
			binRight = temp;
		}
		
		for(int bin = binLeft ; bin < binRight ; ++bin) {
			std::vector<size_t>& rows = rowBinMembership[bin];
			for(size_t row : rows) {
				selectedRows[row] = true;
			 }
			tableRowsFiltered -= rows.size();
		}
		
		riv::RowFilter* rowFilter = new riv::RowFilter(table->name,selectedRows);
		dataset->AddFilter(rowFilter);
		
		rowsFiltered += tableRowsFiltered;
	}
	dataset->StopFiltering();
	if(rowsFiltered) {
		printf("%d rows (%d new) filtered : \n\n",rowsFiltered,(rowsFiltered - lastRowsFiltered));
//		dataset->PrintFilteredRows();
	}
	else {
		printf("Nothing filtered.\n");
	}
	lastRowsFiltered = rowsFiltered;
//	printf("\n");
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
	filterDataSet(*datasetOne, distributionsOne, true, minBound, maxBound,rowBinMembershipsOne);
	printf("RIGHT DATASET FILTER BINS:  \n");
	filterDataSet(*datasetTwo, distributionsTwo, false, minBound, maxBound,rowBinMembershipsTwo);
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
//	x = 1430;
//	printf("Slider mouse motion(%d,%d)\n",x,y);
	bool snapToGrid = false;
	if(selectedPointer && x >= paddingX && x <= width - 2 * paddingX) {
		int snapX = x;
		if(snapToGrid) { //TODO: NOT WORKING
			float barWidth = rightBound - leftBound;
			float binWidth = barWidth / (float)histogramBins / 2.F;
			
			float percentagePos = (x - leftBound) / barWidth;
			float position = percentagePos * (float)binWidth * (float)histogramBins * 2.F;

			snapX = round(position);
			
//			int barWidth = rightBound - leftBound;
//			float binWidth = barWidth / (float)histogramBins / 2.F;
//			
//			int snapXBin = round((x - leftBound) / (float)barWidth * binWidth);
//			snapX = snapXBin * binWidth + leftBound;
			
			printf("percentage pos = %f\n",percentagePos);
			printf("position = %f\n",position);
//			snapX = snapXBin * binWidth + leftBound;
		}

//		printf("snapX = %d\n",snapX);
		*selectedPointer = snapX;
		Invalidate();
		glutPostRedisplay();
		return true;
	}
	else if(movePointers) {
		
		int deltaX = x - startDragX;
		
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
		membershipHistogramOne.Clear();
		createMembershipData(source,rowBinMembershipsOne);
	}
	else if(source == *datasetTwo) {
		membershipHistogramTwo.Clear();
		printHeader("CREATE MEMBERSHIP HISTOGRAM TWO");
		createMembershipData(source,rowBinMembershipsTwo);
	}
	else {
		printf("UNKNOWN DATASET\n");
	}
	
	redisplayWindow();
	
}
