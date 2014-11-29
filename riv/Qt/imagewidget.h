#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QGLWidget>

#include "RIV/Views/ImageView.h"

class ImageWidget : public QGLWidget, public RIVImageView
{
    Q_OBJECT
public:
    ImageWidget(RIVDataSet* dataset,  BMPImage* image, RIVColorProperty* color, RIVSizeProperty* size, QWidget *parent = 0);
	~ImageWidget();
	void initializeGL();
    void resizeGL(int w, int h);
	
protected:
//	void paintEvent(QPaintEvent *event);
	void paintGL();
signals:

public slots:

};

#endif // IMAGEWIDGET_H
