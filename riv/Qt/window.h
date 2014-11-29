#ifndef WINDOW_H
#define WINDOW_H

#include "imagewidget.h"
#include "heatmapwidget.h"
#include "pcwidget.h"
#include "scenewidget.h"

class Window : public QMainWindow
{
    Q_OBJECT
private:
	RIVDataSet dataset;
	
	SceneWidget* sceneWidget = NULL;
	PCWidget* pcWidget = NULL;
	ImageWidget* imageWidget = NULL;
	HeatmapWidget* heatmapWidget = NULL;
public:
    explicit Window(const RIVDataSet& dataset, QWidget *parent = 0);
	
	//QT event callback functionss
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent ( QResizeEvent* event );
	void paintEvent(QPaintEvent*);
	void focusInEvent(QFocusEvent*);
signals:

public slots:

};

#endif // WINDOW_H
