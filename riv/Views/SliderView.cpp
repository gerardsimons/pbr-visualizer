//
//  SliderView.cpp
//  embree
//
//  Created by Gerard Simons on 03/01/15.
//
//

#include <algorithm>

#include "SliderView.h"
#include "../Configuration.h"
#include "../Graphics/graphics_helper.h"

RIVSliderView* RIVSliderView::instance = NULL;
int RIVSliderView::windowHandle = -1;

void RIVSliderView::resetPointers() {
	pointerOneX = leftBound;
	pointerTwoX = rightBound - pointerWidth;
}

RIVSliderView::RIVSliderView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, HistogramSet<float,ushort>* distributionsOne, HistogramSet<float,ushort>* distributionsTwo) : RIVDataView(datasetOne,datasetTwo), distributionsOne(distributionsOne), distributionsTwo(distributionsTwo) {
	
	if(!instance) {
		instance = this;
	}
	else {
		throw std::runtime_error("Only 1 instance of sliderview allowed.");
	}
	paddingX = 0;
	paddingX = 0;
	membershipHistogramOne = Histogram<float>("membership_one", -1, 1, histogramBins);
	membershipHistogramTwo = Histogram<float>("membership_two", -1, 1, histogramBins);
	
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
		
		glClearColor(1,1,1,0);
		glClear( GL_COLOR_BUFFER_BIT );
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
        int effectiveHeight = top - bottom;
        int barBottom = effectiveHeight / 2.F - .1F * effectiveHeight;
        int barTop = effectiveHeight / 2.F + .1F * effectiveHeight;
        
        riv::ColorMap membershipColors = colors::redGrayBlueColorMap();
        int barMiddle = (rightBound - leftBound) / 2;
        int barWidth = rightBound - leftBound;
        float barHeight = barTop - barBottom;
		//Draw the gradient bar
		glBegin(GL_QUAD_STRIP);
        
		//Left
        riv::Color red = membershipColors.ComputeColor(0);
		glColor3f(red.R,red.G,red.B);
		glVertex2f(leftBound, barBottom);
		glVertex2f(leftBound, barTop);
        
        //Middle
        riv::Color gray = membershipColors.ComputeColor(0.5);
        glColor3f(gray.R,gray.G,gray.B);
        glVertex2f(barMiddle, barBottom);
        glVertex2f(barMiddle, barTop);
        
		//Right
                  riv::Color blue = membershipColors.ComputeColor(1);
		glColor3f(blue.R,blue.G,blue.B);
		glVertex2f(rightBound, barBottom);
		glVertex2f(rightBound, barTop);
		glEnd();
        
        float binWidth = barWidth / (float)histogramBins;
        glColor3f(0, 0, 0);
        int xDivider = leftBound;
        
        float maxValueHistogramOne = membershipHistogramOne.MaximumNormalizedValue();
        float maxValueHistogramTwo = membershipHistogramTwo.MaximumNormalizedValue();
        
        float max = std::max(maxValueHistogramOne,maxValueHistogramTwo);

        float maxBarHeight = top - barTop;
        for(int i = 0 ; i < histogramBins ; ++i) {

            
            float histValue;
            if(i < histogramBins / 2) {
                histValue = membershipHistogramTwo.NormalizedValue(i);
            }
            else histValue = membershipHistogramOne.NormalizedValue(i);
            float binHeight = (histValue / max) * maxBarHeight;
            float bottom = barTop - barHeight - binHeight;
            float top = barTop + binHeight;
//            printf("Draw bin with height %f\n",binHeight);
            riv::Color color = membershipColors.ComputeColor((histogramBins - i) / (float)histogramBins);
            glColor3f(color.R, color.G, color.B);
            glRectf(xDivider, bottom, xDivider + binWidth, top);
            glColor3f(0, 0, 0);
            glBegin(GL_LINE_LOOP);
            glVertex2f(xDivider, bottom);
            glVertex2f(xDivider, top);
            glVertex2f(xDivider + binWidth, top);
            glVertex2f(xDivider + binWidth, bottom);
            glEnd();
            xDivider += binWidth;
        }

		
		//Draw pointers
		glColor3f(0, 0, 0);
		glBegin(GL_QUADS);
		
		glVertex2f(pointerOneX - pointerWidth / 2.F, barTop);
		glVertex2f(pointerOneX - pointerWidth / 2.F, barBottom);
		glVertex2f(pointerOneX + pointerWidth / 2.F, barBottom);
		glVertex2f(pointerOneX + pointerWidth / 2.F, barTop);
		
		glVertex2f(pointerTwoX - pointerWidth / 2.F, barTop);
		glVertex2f(pointerTwoX - pointerWidth / 2.F, barBottom);
		glVertex2f(pointerTwoX + pointerWidth / 2.F, barBottom);
		glVertex2f(pointerTwoX + pointerWidth / 2.F, barTop);
		
		glEnd();
		

		
		//Draw uniqueness histogram one
		//		int bins = membershipHistogramOne.NumberOfBins();
		//	float maxHeight = top;
		//	float minHeight = bottom;
		
		//Draw scale


		float x = leftBound;
		
		x = leftBound;
		int centerBar = height / 2.F;
		
		//		printf("LEFT HISTOGRAM BARS DRAWING : \n\n");
		//		membershipHistogramOne.Print();

		
		//TODO: Pray to god histogramBins is divisble by 2?
//		for(int i = 0 ; i < histogramBins ; ++i) { //Draw them backwards as the most unique should be to the left
//			float histValue = membershipHistogramOne.NormalizedValue(i);
//			float barHeight = histValue * maxBarHeight;
//			
//			glColor3f( 1, 0, 0);
//			glRectf(x, barTop, x+binWidth, barTop + barHeight);
//			
//			std::string nrMembers = std::to_string(rowBinMembershipsOne[i].size());
////			drawText(nrMembers, x + binWidth / 2.F, centerBar, .07F);
//			
//			x += binWidth;
//		}
//		x = leftBound;
//		
//		//				printf("RIGHT HISTOGRAM BARS DRAWING : \n\n");
//		//				membershipHistogramTwo.Print();
//		
//		for(int i = 0 ; i < histogramBins ; ++i) { //Draw them backwards as the most unique should be to the left
//			std::string nrMembers = std::to_string(rowBinMembershipsTwo[i].size());
//			//			nrMembers = std::to_string(i);
//			
//			float histValue = membershipHistogramTwo.NormalizedValue(i);
//			float barHeight = histValue * maxBarHeight;
//			//						printf("bar height (i = %d) = %f\n",i,barHeight);
//			//					printf("glRectf(%f,%f,%f,%f)\n",x,bottom,x+binWidth,bottom+barHeight);
//			glColor3f(0, 0, 1);
//			glRectf(x, barBottom, x+binWidth, barBottom - barHeight);
//			
////			drawText(nrMembers, x + binWidth / 2.F, centerBar, .07F);
//			x += binWidth;
//		}
		
        //Draw contour around the entire bar
//		glColor3f(0, 0, 0);
//		glLineWidth(2);
//		glBegin(GL_LINES);
//		glVertex2f(leftBound, barBottom);
//		glVertex2f(rightBound, barBottom);
//		glVertex2f(leftBound, barTop - 1);
//		glVertex2f(rightBound, barTop - 1);
//		glEnd();
		
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
void RIVSliderView::createMembershipData() {
	createMembershipData(*datasetOne,true);
	createMembershipData(*datasetTwo,false);
}
void RIVSliderView::createMembershipData(RIVDataSet<float,ushort>* datasetSource, bool isLeftSet) {
	if(isLeftSet) {
		membershipHistogramOne.Clear();
		rowBinMembershipsOne.clear();
	}
	else {
		membershipHistogramTwo.Clear();
		rowBinMembershipsTwo.clear();
	}
	if(selectedRecords.size()) {
		
		RIVTable<float,ushort>* table = datasetSource->GetTable(selectedTable);
		RIVTable<float,ushort>* membershipTable = datasetSource->GetTable(selectedMembershipTable);
		
		if(!membershipTable) {
			membershipTable = datasetSource->CreateTable(selectedMembershipTable);
		}
		
		RIVRecord<float>* membershipRecord = membershipTable->GetRecord<float>(MEMBERSHIP);
		if(!membershipRecord) {
			membershipRecord = membershipTable->CreateRecord<float>(MEMBERSHIP,-1,1);
		}
		else {
			membershipRecord->Clear();
		}
		//		auto tables = datasetSource->GetTables();
		
		size_t row = 0;
		TableIterator* iterator = table->GetIterator();
		
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
						
						float diff;
						
						if(binValueTwo > binValueOne) {
							diff = (binValueTwo - binValueOne) / binValueTwo;
						}
						else {
							diff = -1 * (binValueOne - binValueTwo) / binValueOne;
						}
						//						float binValueTwo = histogramTwo->NormalizedValue(bin);
						//						float binValueDelta;
						
						//						float diff = binValueTwo - binValueOne;
						//					if(diff > maxDifference) {
						//						maxDifference = diff;
						//					}
						//						printf("diff = %f\n",diff);
						totalDiff += diff;
					}
				}
				//				sumMaxDifferences += maxDifference;
			});
			//Normalize it
			totalDiff /= selectedRecords.size();
			//			printf("totalDiff = %f\n",totalDiff);
			membershipRecord->AddValue(totalDiff);
			
			if(isLeftSet) {
				int bin = membershipHistogramOne.Add(totalDiff);
				rowBinMembershipsOne[bin].push_back(row);
			}
			else {
				int bin = membershipHistogramTwo.Add(totalDiff);
				rowBinMembershipsTwo[bin].push_back(row);
			}
		}
		
		//Normalize
		//		float maxValue = membershipRecord->Max();
		//		for(size_t row = 0 ; row < membershipRecord->Size() ; ++row) {
		//			float oldValue = membershipRecord->Value(row);
		//			membershipRecord->SetValue(row, oldValue / maxValue);
		//		}
		
//		membershipHistogramOne.Print();
//		membershipHistogramTwo.Print();
	}
}
void RIVSliderView::AddSelectedRecord(const std::string& tableName, const std::string &recordName) {
	if(tableName != selectedTable) {
		selectedRecords.clear();
		selectedTable = tableName;
		
		if(tableName == PATHS_TABLE) {
			//			membershipTableOne = pathMembershipOneTable;
			//			membershipTableTwo = pathMembershipTwoTable;
			selectedMembershipTable = PATH_MEMBERSHIP_TABLE;
		}
		else if(tableName == INTERSECTIONS_TABLE) {
			//			membershipTableOne = isectMembershipOneTable;
			//			membershipTableTwo = isectMembershipTwoTable;
			selectedMembershipTable = ISECT_MEMBERSHIP_TABLE;
		}
	}
	if(selectedRecords.find(recordName) == selectedRecords.end()) {
		selectedRecords.insert(recordName);
		
		if(tableName == PATHS_TABLE) {
			
			selectedMembershipTable = PATH_MEMBERSHIP_TABLE;
		}
		else if(tableName == INTERSECTIONS_TABLE) {
			
			selectedMembershipTable = ISECT_MEMBERSHIP_TABLE;
		}
		
		//Recreate the membership data
		createMembershipData();
		
		//Post for redraw
		redisplayWindow();
	}
}
void RIVSliderView::RemoveSelectedRecord(const std::string &recordName) {
	auto it = selectedRecords.find(recordName);
	if(it != selectedRecords.end()) {
		selectedRecords.erase(it);
		
		//Recreate the membership data
		createMembershipData();
		
		//Post for redisplay
		redisplayWindow();
		
		if(selectedRecords.size() == 0) {
			(*datasetOne)->DeleteTable(PATH_MEMBERSHIP_TABLE);
			(*datasetTwo)->DeleteTable(PATH_MEMBERSHIP_TABLE);
			filterDataSets();
		}
	}
	else {
		printf("No such record selected.");
	}
}
void RIVSliderView::filterDataSet(RIVDataSet<float,ushort>* dataset, HistogramSet<float,ushort>* distributions, bool isLeftSet, float minBound, float maxBound, std::map<int,std::vector<size_t>>& rowBinMembership, riv::RowFilter*& existingFilter) {
	
	if(selectedTable.size()) {
		//Remove the existing filter for this dataset if there was one
		dataset->StartFiltering();
		if(existingFilter) {
			dataset->ClearRowFilter(existingFilter);
		}
		
		//	printHeader("MEMBERSHIP FILTERING");
		if(isLeftSet) {
			//		minBound += 1;
			//		minBound = std::min(0.F,minBound);
			//		maxBound = std::min(0.F,maxBound);
		}
		else {
			//		minBound = std::max(0.F,minBound);
			//		maxBound = std::max(0.F,maxBound);
		}
		int rowsFiltered = 0;
		//	printf("minbound = %f\n",minBound);
		//	printf("maxBound = %f\n",maxBound);

		
		minBound = minBound * (histogramBins / 2.F) + (histogramBins / 2.F);
		maxBound = maxBound * (histogramBins / 2.F) + (histogramBins / 2.F);
		
		if(minBound > maxBound) {
			int temp = minBound;
			minBound = maxBound;
			maxBound = temp;
		}
		
		int binLeft = ceil(minBound);
		int binRight = floor(maxBound);
		
		//Make sure bin left really is the left most (smallest) bin
		
		
		//	for(size_t i = 0 ; i < tables->size() ; ++i) {
		
		auto table = dataset->GetTable(selectedTable);
		
		//How many rows are going be filtered for this table
		int tableRowsFiltered = 0;
		
		//Row filter that manually filters rows according to their row number
		std::map<size_t,bool> filteredRows;
		
		for(int bin = 0 ; bin < histogramBins ; ++bin) {
			if(bin < binLeft || bin > binRight) {
				std::vector<size_t>& rows = rowBinMembership[bin];
				for(size_t row : rows) {
					filteredRows[row] = true;
				}
				tableRowsFiltered += rows.size();
			}
		}
		
		
		
		existingFilter = new riv::RowFilter(table->name,filteredRows);
		dataset->AddFilter(existingFilter);
		
		rowsFiltered += tableRowsFiltered;
		//	}
		dataset->StopFiltering();
		if(rowsFiltered) {
//			printf("%d rows (%d new) filtered : \n\n",rowsFiltered,(rowsFiltered - lastRowsFiltered));
			//		dataset->PrintFilteredRows();
		}
		else {
//			printf("Nothing filtered.\n");
		}
		lastRowsFiltered = rowsFiltered;
	}
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
	
//	printf("SLIDER MIN BOUND = %f\n",minBound);
//	printf("SLIDER MAX BOUND = %f\n",maxBound);
	
//	printf("LEFT DATASET FILTER BINS:  \n");
	filterDataSet(*datasetOne, distributionsOne, true, minBound, maxBound,rowBinMembershipsOne,membershipFilterOne);
//	printf("RIGHT DATASET FILTER BINS:  \n");
	filterDataSet(*datasetTwo, distributionsTwo, false, minBound, maxBound,rowBinMembershipsTwo,membershipFilterTwo);
}
bool RIVSliderView::HandleMouse(int button, int state, int x, int y) {
	float minDistance = 10;
	bool mouseCaught = false;
	printf("state = %d\n",state);
	if(state == GLUT_DOWN) {
		//Is there a pointer close by?
		
		if(button == GLUT_LEFT_BUTTON) {
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
		else if(button == GLUT_RIGHT_BUTTON) {
			resetPointers();
			filterDataSets();
			mouseCaught = true;
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
			
			redisplayWindow();
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
void RIVSliderView::OnFiltersChanged(RIVDataSet<float,ushort>* source) {
	//WARNING: This creates erroneous and unstabe results... might be because of a loop created
//	if(source == *datasetOne) {
//		createMembershipData(source,true);
//	}
//	else if(source == *datasetTwo) {
//		createMembershipData(source,false);
//	}
}
void RIVSliderView::OnDataChanged(RIVDataSet<float,ushort>* source) {
	printf("*** RIVSliderView received a on dataset changed notification!\n" );
	if(source == *datasetOne) {
//		printHeader("CREATE MEMBERSHIP HISTOGRAM ONE");
		createMembershipData(source,true);
	}
	else if(source == *datasetTwo) {
//		printHeader("CREATE MEMBERSHIP HISTOGRAM TWO");
		createMembershipData(source,false);
	}
	else {
		printf("UNKNOWN DATASET\n");
	}
    filterDataSets();
	redisplayWindow();
	
}
