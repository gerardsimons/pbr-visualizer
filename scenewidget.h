#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QGLWidget>

#include "DataView.h"
#include "RIV/Views/3DView.h"
//#include "RIV/Geometry/MeshModel.h"
//#include "RIV/Octree/Octree.h"
//#include "RIV/Geometry/Ray.h"

class SceneWidget : public QGLWidget, public RIV3DView {
	
    Q_OBJECT
	
private:
	
public:
	//Constructors & Destructors
    explicit SceneWidget(RIVDataSet* dataset, RIVColorProperty* color, RIVSizeProperty* size, QWidget *parent = NULL);
	explicit SceneWidget(SceneWidget& newSceneWidget);
    ~SceneWidget();
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
#endif // SCENEWIDGET_H
