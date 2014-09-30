#include "pcwidget.h"

#include <QMouseEvent>

PCWidget::PCWidget(RIVDataSet* dataset, RIVColorProperty* color, RIVSizeProperty* size, QWidget *parent) :
	ParallelCoordsView(dataset,color,size), QGLWidget(parent) {
	setAutoFillBackground(false);
//	setAutoBufferSwap(false);
}

void PCWidget::mousePressEvent(QMouseEvent *event)
//! [11] //! [12]
{
	printf("mousePressEvent type = %d (x,y) = (%d,%d)\n",event->type(),event->pos().x(),event->pos().y());
	HandleMouse(GLUT_LEFT_BUTTON, GLUT_DOWN, event->pos().x(),event->pos().y());
	//    if (event->button() == Qt::LeftButton) {
	//        lastPoint = event->pos();
	//        scribbling = true;
	//    }
}

void PCWidget::mouseMoveEvent(QMouseEvent *event)
{
	//	printf("mouseMoveEvent type =%d\n",event->type());
	printf("mouseMoveEvent type = %d (x,y) = (%d,%d)\n",event->type(),event->pos().x(),event->pos().y());
	HandleMouseMotion(event->pos().x(),event->pos().y());
	//    if ((event->buttons() & Qt::LeftButton) && scribbling)
	//        drawLineTo(event->pos());
	
//	updateGL();
}

void PCWidget::mouseReleaseEvent(QMouseEvent *event)
{
	//	printf("mouseReleaseEvent type =%d\n",event->type());
	HandleMouse(GLUT_LEFT_BUTTON, GLUT_UP, event->pos().x(), event->pos().y());
	//    if (event->button() == Qt::LeftButton && scribbling) {
	//        drawLineTo(event->pos());
	//        scribbling = false;
	//    }
	cached = false;
	updateGL();
}

void PCWidget::initializeGL()
{
	// Set up the rendering context, define display lists etc.:
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
}

void PCWidget::resizeGL(int width, int height)
{
	
	printf("PCWidget resize %dx%d\n",width,height);
	
	// setup viewport, projection etc.:
	ParallelCoordsView::width = width;
	ParallelCoordsView::height = height;
	
	paddingX = 20;
	paddingY = 20;
	
//	glutInitDisplayMode(GLUT_SINGLE);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	InitializeGraphics();
}

void PCWidget::paintGL()
{
	printf("PCWidget paintEvent()\n");
	
	makeCurrent();
	
	if(!cached) {
		cache = renderPixmap();
		cached = true;
	}
	else {
		QPainter painter;
		painter.begin(this);
		
		printf("Draw cached\n");
		painter.drawPixmap(0, 0, cache);
	}
	Draw();
}

//void PCWidget::resizeEvent ( QResizeEvent* event ) {

//	QGLWidget::resizeGL(event->g, int h)

//	printf("PCWidget ResizeEvent\n");

//	ParallelCoordsView::width = event->size().width();
//	ParallelCoordsView::height = event->size().height();
	
//	Reshape(event->size().width(), event->size().height());
//}