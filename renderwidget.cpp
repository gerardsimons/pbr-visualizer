#include "renderwidget.h"

#include <QMouseEvent>

//Init instance to draw
//RIV3DView* RIV3DView::instance = NULL;
//int RIV3DView::windowHandle = -1;

RenderWidget::RenderWidget(RIVDataSet* dataset, RIVColorProperty* color, RIVSizeProperty* size, QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent), RIVRenderView(dataset)
{
	printf("RenderWidget initialized\n");
	
	setAutoFillBackground(false);
}

//RenderWidget::RenderWidget(RenderWidget& newRenderWidget)  {
//	printf("WARNING: Copy constructor not implemented.\n");
//}

RenderWidget::~RenderWidget() {
	
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
//! [11] //! [12]
{
	//	printf("mousePressEvent type =%d\n",event->type());
	HandleMouse(GLUT_LEFT_BUTTON, GLUT_DOWN, event->pos().x(), event->pos().y());
	//    if (event->button() == Qt::LeftButton) {
	//        lastPoint = event->pos();
	//        scribbling = true;
	//    }
	repaint();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
	//	printf("mouseMoveEvent type =%d\n",event->type());
	HandleMouseMotion(event->pos().x(), event->pos().y());
	//    if ((event->buttons() & Qt::LeftButton) && scribbling)
	//        drawLineTo(event->pos());
	repaint();
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
	//	printf("mouseReleaseEvent type =%d\n",event->type());
	HandleMouse(GLUT_LEFT_BUTTON, GLUT_UP, event->pos().x(), event->pos().y());
	//    if (event->button() == Qt::LeftButton && scribbling) {
	//        drawLineTo(event->pos());
	//        scribbling = false;
	//    }
	repaint();
}

void RenderWidget::keyPressEvent(QKeyEvent *event) {
	printf("keyPressEvent key=%d\n",event->key());
	if(event->key() == Qt::Key_C) {
//		ToggleDrawIntersectionPoints();
	}
}

void RenderWidget::initializeGL() {
	glEnable(GL_DEPTH_TEST);
}

void RenderWidget::resizeGL(int width, int height) {
	Reshape(width, height);
}

void RenderWidget::OnDataSetChanged() {
	//    printf("3D View received on filter change.");
//	createPaths();
	CreateImageFilm();
	
	updateGL();
	
//	isDirty = true;
}

void RenderWidget::paintGL()
{
	makeCurrent();
	
	//    QPainter painter;
	//    painter.begin(this);
	
	Draw();
}

