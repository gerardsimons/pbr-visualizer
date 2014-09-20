#include "window.h"

#include <QMouseEvent>

Window::Window(const RIVDataSet& dataset, BMPImage* image, const MeshModel& model, QWidget *parent) : QMainWindow(parent) {
	this->dataset = dataset;
	this->image = image;
	this->model = model;
	
	createWidgets();
}

void Window::createWidgets() {

//    RIVTable *imageTable = dataset.GetTable("image");
//    RIVTable *pathTable = dataset.GetTable("path");
    RIVTable *intersectionsTable = dataset.GetTable("intersections");
    
    RIVRecord* bounceRecord = intersectionsTable->GetRecord("bounce#");
	//    RIVRecord* xRecord = intersectionsTable->GetRecord("intersection X");
	
	//	RIVRecord* throughputOne = pathTable->GetRecord("throughput 1");
	//	RIVRecord* throughputTwo = pathTable->GetRecord("throughput 2");
	//	RIVRecord* throughputThree = pathTable->GetRecord("throughput 3");
	
	//	RIVRecord* spectrumR = intersectionsTable->GetRecord("spectrum 1");
	//	RIVRecord* spectrumG = intersectionsTable->GetRecord("spectrum 2");
	//	RIVRecord* spectrumB = intersectionsTable->GetRecord("spectrum 3");
    
	//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty(pathTable,colors::GREEN,colors::RED);
    
    ColorMap jetColorMap = colors::jetColorMap();
    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,bounceRecord,jetColorMap);
	RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(2);
	//    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionsTable,xRecord,jetColorMap);
	//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(pathTable,throughputOne,throughputTwo,throughputThree);
	//	RIVColorProperty *colorProperty = new RIVColorRGBProperty<float>(intersectionsTable,spectrumR,spectrumG,spectrumB);
	
	
	sceneWidget = new SceneWidget(&dataset,colorProperty,sizeProperty,this);
	pcWidget = new PCWidget(&dataset,colorProperty,sizeProperty,this);
	imageWidget = new ImageWidget(&dataset,image,colorProperty,sizeProperty,this);
	heatmapWidget = new HeatmapWidget(&dataset,this);
	
	dataset.AddFilterListener(sceneWidget);
	dataset.AddFilterListener(pcWidget);
	
	sceneWidget->SetModelData(model);
}

void Window::resizeEvent ( QResizeEvent* event ) {

	printf("MainWindow ResizeEvent\n");

	//Reshape widgets according to new window size
	int windowWidth = event->size().width();
	int windowHeight = event->size().height();
	
	//The bottom 3 views are all square, determined by the smallest dimension available
	int squareSize = windowHeight / 2;
	if(windowWidth < windowHeight) {
		squareSize = windowWidth / 2;
	}
	
	pcWidget->move(0, 0);
	sceneWidget->move(0,squareSize);
	imageWidget->move(squareSize,squareSize);
	heatmapWidget->move(squareSize * 2, squareSize);
	
	sceneWidget->resize(squareSize,squareSize);
	imageWidget->resize(squareSize,squareSize);
	heatmapWidget->resize(squareSize,squareSize);
	pcWidget->resize(windowWidth, windowHeight / 2);
}

void Window::keyPressEvent(QKeyEvent *event) {
	printf("Window keyPressEvent key=%d : ",event->key());
	switch (event->key()) {
		case Qt::Key_C:
			sceneWidget->ToggleDrawIntersectionPoints();
			sceneWidget->repaint();
			break;
		default:
			printf(" ... does nothing.");
	}
	printf("\n");
}