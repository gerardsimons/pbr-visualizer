#include "scenewidget.h"

#include <QMouseEvent>

//Init instance to draw
//RIV3DView* RIV3DView::instance = NULL;
//int RIV3DView::windowHandle = -1;

SceneWidget::SceneWidget(RIVDataSet* dataset, PBRTConfig* pbrtConfig, RIVColorProperty* color, RIVSizeProperty* size, QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent), RIV3DView(dataset,pbrtConfig,color,size)
{
	printf("SceneWidget initialized\n");

    setAutoFillBackground(false);
}

//SceneWidget::SceneWidget(SceneWidget& newSceneWidget)  {
//	printf("WARNING: Copy constructor not implemented.\n");
//}

SceneWidget::~SceneWidget() {
	
}

void SceneWidget::mousePressEvent(QMouseEvent *event)
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

void SceneWidget::mouseMoveEvent(QMouseEvent *event)
{
//	printf("mouseMoveEvent type =%d\n",event->type());
	HandleMouseMotion(event->pos().x(), event->pos().y());
//    if ((event->buttons() & Qt::LeftButton) && scribbling)
//        drawLineTo(event->pos());
	repaint();
}

void SceneWidget::mouseReleaseEvent(QMouseEvent *event)
{
//	printf("mouseReleaseEvent type =%d\n",event->type());
	HandleMouse(GLUT_LEFT_BUTTON, GLUT_UP, event->pos().x(), event->pos().y());
//    if (event->button() == Qt::LeftButton && scribbling) {
//        drawLineTo(event->pos());
//        scribbling = false;
//    }
	repaint();
}

void SceneWidget::keyPressEvent(QKeyEvent *event) {
	printf("keyPressEvent key=%d\n",event->key());
	if(event->key() == Qt::Key_C) {
		ToggleDrawIntersectionPoints();
	}
}

void SceneWidget::initializeGL() {
	glEnable(GL_DEPTH_TEST);
}

void SceneWidget::resizeGL(int width, int height) {
	Reshape(width, height);
}

void SceneWidget::OnDataSetChanged() {
	//    printf("3D View received on filter change.");
	createPaths();
	
	updateGL();
	
	isDirty = true;
}

void SceneWidget::paintGL()
{
	makeCurrent();
	
//    QPainter painter;
//    painter.begin(this);
	
	Draw();
}