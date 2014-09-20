#include "heatmapwidget.h"

HeatmapWidget::HeatmapWidget(RIVDataSet* dataset, QWidget *parent) :
    QGLWidget(parent), RIVHeatMapView(dataset) {
	
}

void HeatmapWidget::initializeGL() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
}
void HeatmapWidget::resizeGL(int w, int h) {
	Reshape(w, h);
}
void HeatmapWidget::paintGL() {
	Draw();
}
