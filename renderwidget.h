#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QGLWidget>

#include "DataView.h"
#include "RIV/Views/RenderView.h"

class RenderWidget : public QGLWidget, public RIVRenderView
{
    Q_OBJECT
	
private:
	
public:
	//Constructors & Destructors
	explicit RenderWidget(RIVDataSet* dataset, RIVColorProperty* color, RIVSizeProperty* size, QWidget *parent = NULL);
	//	explicit SceneWidget(SceneWidget& newSceneWidget);
	~RenderWidget();
	//Methods
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void OnDataSetChanged();
public slots:
	//    void animate();
	
protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
};

#endif // RENDERWIDGET_H
