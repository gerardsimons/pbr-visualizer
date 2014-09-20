#ifndef HEATMAPWIDGET_H
#define HEATMAPWIDGET_H

#include <QGLWidget>
#include "RIV/Views/HeatMapView.h"
//#include <QObject>

class HeatmapWidget : public QGLWidget, public RIVHeatMapView
{
    Q_OBJECT
public:
    HeatmapWidget(RIVDataSet* dataset, QWidget *parent = NULL);
	void initializeGL();
    void resizeGL(int w, int h);
	void paintGL();
signals:

public slots:

};

#endif // HEATMAPWIDGET_H
