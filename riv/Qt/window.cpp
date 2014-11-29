#include "window.h"

#include <QMouseEvent>

Window::Window(const RIVDataSet& dataset_, QWidget *parent) :
	QMainWindow(parent),
	pbrtConfiguration(config) {
		
//	dataset = dataset_;

	//Create widgets
//	RIVTable *intersectionsTable = dataset.GetTable("intersections");
//	RIVTable* pathTable = dataset.GetTable("path");
		
//    RIVRecord* bounceRecord = intersectionsTable->GetRecord("bounce#");
	//    RIVRecord* xRecord = intersectionsTable->GetRecord("intersection X");
	
	//	RIVRecord* throughputOne = pathTable->GetRecord("throughput 1");
	//	RIVRecord* throughputTwo = pathTable->GetRecord("throughput 2");
	//	RIVRecord* throughputThree = pathTable->GetRecord("throughput 3");
	
	//	RIVRecord* spectrumR = intersectionsTable->GetRecord("spectrum 1");
	//	RIVRecord* spectrumG = intersectionsTable->GetRecord("spectrum 2");
	//	RIVRecord* spectrumB = intersectionsTable->GetRecord("spectrum 3");
    
	//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty(pathTable,colors::GREEN,colors::RED);
    
//    ColorMap jetColorMap = colors::jetColorMap();
//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,bounceRecord,jetColorMap);
//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(intersectionsTable,"spectrum R","spectrum G","spectrum B");
//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(pathTable,"radiance R","radiance G","radiance B");
//	RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(.1F);
//	//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,xRecord,jetColorMap);
//
//	//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(intersectionsTable,spectrumR,spectrumG,spectrumB);
//
//		
//	sceneWidget = new SceneWidget(&dataset,&pbrtConfiguration,colorProperty,sizeProperty,this);
//	pcWidget = new PCWidget(&dataset,colorProperty,sizeProperty,this);
//	imageWidget = new ImageWidget(&dataset,image,colorProperty,sizeProperty,this);
//	heatmapWidget = new HeatmapWidget(&dataset,this);
//		
//	dataset.AddFilterListener(sceneWidget);
//	dataset.AddFilterListener(pcWidget);
//		
//	update();
}

void Window::focusInEvent( QFocusEvent* event )
{}

void Window::paintEvent(QPaintEvent* event) {
//	printf("Window paintEvent\n");
}

void Window::resizeEvent ( QResizeEvent* event ) {

//	printf("MainWindow ResizeEvent\n");
//
//	//Reshape widgets according to new window size
//	int windowWidth = event->size().width();
//	int windowHeight = event->size().height();
//	
//	//The bottom 3 views are all square, determined by the smallest dimension available
//	int squareSize = windowHeight / 2;
//	if(windowWidth < windowHeight) {
//		squareSize = windowWidth / 2;
//	}
//	
//	pcWidget->move(0, 0);
//	sceneWidget->move(0,squareSize);
//	imageWidget->move(squareSize,squareSize);
//	heatmapWidget->move(squareSize * 2, squareSize);
//	
//	sceneWidget->resize(squareSize,squareSize);
//	imageWidget->resize(squareSize,squareSize);
//	heatmapWidget->resize(squareSize,squareSize);
//	pcWidget->resize(windowWidth, windowHeight / 2);
}

void Window::keyPressEvent(QKeyEvent *event) {
//	printf("Window keyPressEvent key=%d : ",event->key());
//	bool redraw = true;
//	float camSpeed = 1;
//	switch (event->key()) {
//		case Qt::Key_C:
//			sceneWidget->ToggleDrawIntersectionPoints();
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_Escape: //ESC key
//			printf("Clear filters\n");
//			//            invalidateAllViews();
//			dataset.StartFiltering();
//			dataset.ClearFilters();
//			dataset.StopFiltering();
//			break;
//		case Qt::Key_B: // 'b' key
//			glutSwapBuffers();
//			printf("Manual swap buffers\n");
//			//            copy_buffer();
//			break;
//		case Qt::Key_L : // 'l' key, toggle lines drawing
//			sceneWidget->CyclePathSegment();
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_BracketLeft: // '[' key, increase path segment
//			sceneWidget->MovePathSegment(-.01F);
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_BracketRight:
//			sceneWidget->MovePathSegment(.01F);
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_H: // the 'h' from heatmap, toggle drawing the octree heatmap
//			sceneWidget->ToggleDrawHeatmap();
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_P: //The 'p' key, toggle drawing paths in 3D view
//			sceneWidget->ToggleDrawPaths();
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_T: // 't' key, use as temp key for some to-test function
//		{
//			std::vector<riv::Filter*> allFilters;
//			std::vector<ushort> selectedObjectIDs;
//			selectedObjectIDs.push_back(1);
//			selectedObjectIDs.push_back(1);
//			selectedObjectIDs.push_back(1);
//			selectedObjectIDs.push_back(1);
//			selectedObjectIDs.push_back(1);
//			for(size_t i = 0 ; i < selectedObjectIDs.size() ; ++i) {
//				riv::Filter* objectFilter = new riv::DiscreteFilter("object ID",selectedObjectIDs[i]);
//				riv::Filter* bounceFilter = new riv::DiscreteFilter("bounce#",i+1);
//				std::vector<riv::Filter*> fs;
//				fs.push_back(objectFilter);
//				fs.push_back(bounceFilter);
//				allFilters.push_back(new riv::ConjunctiveFilter(fs));
//			}
//			riv::GroupFilter* pathCreationFilter = new riv::GroupFilter(allFilters,dataset.GetTable("path"));
//			pathCreationFilter->Print();
//			printf("\n");
//			dataset.StartFiltering();
//			dataset.AddFilter("path", pathCreationFilter);
//			dataset.StopFiltering();
//			redraw = false;
//			break;
//		}
//		case Qt::Key_W: // 'w' key, move camera in Y direction
//			sceneWidget->MoveCamera(0,camSpeed,0);
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_S:
//			sceneWidget->MoveCamera(0, -camSpeed, 0);
//			sceneWidget->repaint();
//			break;
//		case Qt::Key_Up:
//			sceneWidget->MoveCamera(0,0,camSpeed);
//			sceneWidget->repaint();
//			//            uiView->MoveMenu(0,-10.F);
//			break;
//		case Qt::Key_Down:
//			sceneWidget->MoveCamera(0,0,-camSpeed);
//			sceneWidget->repaint();
//			//            uiView->MoveMenu(0,10);
//			break;
//		case Qt::Key_Left:
//			sceneWidget->MoveCamera(-camSpeed,0,0);
//			sceneWidget->repaint();
//			//            uiView->MoveMenu(-10.F,0);
//			break;
//		case Qt::Key_Right:
//			sceneWidget->MoveCamera(camSpeed,0,0);
//			sceneWidget->repaint();
//			//            uiView->MoveMenu(10.F,0);
//			break;
//		default:
//			printf(" ... does nothing.");
//	}
}