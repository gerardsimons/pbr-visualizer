#include "imagewidget.h"

ImageWidget::ImageWidget(RIVDataSet* dataset, BMPImage* image, RIVColorProperty* color, RIVSizeProperty* size, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
RIVImageView(dataset,image,color,size) {
	setAutoFillBackground(false);
}

ImageWidget::~ImageWidget() {
	//Nothing to do
}

void ImageWidget::resizeGL(int width, int height) {
	Reshape(width,height);
}

void ImageWidget::initializeGL() {
	//Use double buffering!
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable (GL_BLEND);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_2D);
	
    // Set nearest filtering mode for texture minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
    // Set bilinear filtering mode for texture magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	InitializeGraphics();
}

void ImageWidget::paintGL() {
	
	printf("ImageWidget paintGL\n");
	
//	makeCurrent();
	
//    glClearColor(0,1,1, 0.0);
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
//    QPainter painter;
//    painter.begin(this);
	
	Draw();
}
