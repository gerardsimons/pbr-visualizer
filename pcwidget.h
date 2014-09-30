#ifndef PCWIDGET_H
#define PCWIDGET_H

#include <QGLWidget>


#include "RIV/Views/ParallelCoordsView.h"

class PCWidget : public QGLWidget, public ParallelCoordsView
{
    Q_OBJECT
	
private:
	QPixmap cache;
	bool cached = false;
	
public:
    PCWidget(RIVDataSet* dataset, RIVColorProperty* color, RIVSizeProperty *size, QWidget *parent = 0);
	
	void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
//	void keyPressEvent(QKeyEvent* event);
//	void resizeEvent ( QResizeEvent* event );
	void initializeGL();
    void resizeGL(int w, int h);

signals:

public slots:

protected:
	void paintGL();
//void paintEvent(QPaintEvent *event);
	
};

#endif // PCWIDGET_H
