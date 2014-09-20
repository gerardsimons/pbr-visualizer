#-------------------------------------------------
#
# Project created by QtCreator 2014-09-18T10:32:08
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_widgets
TEMPLATE = app


SOURCES += main.cpp\
        scenewidget.cpp\
    RIV/Data/Cluster.cpp \
    RIV/Data/ClusterSet.cpp \
    RIV/Data/DataFileReader.cpp \
    RIV/Data/DataSet.cpp \
    RIV/Data/Filter.cpp \
    RIV/Data/Iterator.cpp \
    RIV/Data/Record.cpp \
    RIV/Data/Reference.cpp \
    RIV/Data/Table.cpp \
    RIV/Geometry/TriangleMesh.cpp \
    RIV/Graphics/BMPImage.cpp \
    RIV/Graphics/ColorMap.cpp \
    RIV/Graphics/ColorPalette.cpp \
    RIV/Graphics/loadppm.cpp \
    RIV/Octree/Octree.cpp \
    RIV/Octree/OctreeNode.cpp \
    RIV/Views/3DView.cpp \
    RIV/Views/HeatMapView.cpp \
    RIV/Views/ImageView.cpp \
    RIV/Views/ParallelCoordsAxis.cpp \
    RIV/Views/ParallelCoordsView.cpp \
    RIV/helper.cpp \
    RIV/main.cpp \
    RIV/reporter.cpp \
    imagewidget.cpp \
    heatmapwidget.cpp \
    window.cpp \
    uiwidget.cpp \
    pcwidget.cpp


HEADERS  += scenewidget.h\
    RIV/Data/Cluster.h \
    RIV/Data/ClusterSet.h \
    RIV/Data/DataFileReader.h \
    RIV/Data/DataSet.h \
    RIV/Data/DataSetListener.h \
    RIV/Data/Filter.h \
    RIV/Data/Iterator.h \
    RIV/Data/Record.h \
    RIV/Data/Reference.h \
    RIV/Data/Table.h \
    RIV/Geometry/Geometry.h \
    RIV/Geometry/matrix.h \
    RIV/Geometry/MeshModel.h \
    RIV/Geometry/Ray.h \
    RIV/Geometry/RIVVector.h \
    RIV/Geometry/TriangleMesh.h \
    RIV/Geometry/Vec3D.h \
    RIV/Graphics/BMPImage.h \
    RIV/Graphics/ColorMap.h \
    RIV/Graphics/ColorPalette.h \
    RIV/Graphics/ColorProperty.h \
    RIV/Graphics/Evaluator.h \
    RIV/Graphics/graphics_helper.h \
    RIV/Graphics/loadppm.h \
    RIV/Graphics/Property.h \
    RIV/Graphics/SizeProperty.h \
    RIV/Octree/Octree.h \
    RIV/Octree/OctreeConfig.h \
    RIV/Octree/OctreeNode.h \
    RIV/Views/3DView.h \
    RIV/Views/DataView.h \
    RIV/Views/HeatMapView.h \
    RIV/Views/ImageView.h \
    RIV/Views/ParallelCoordsAxis.h \
    RIV/Views/ParallelCoordsAxisGroup.h \
    RIV/Views/ParallelCoordsView.h \
    RIV/AntTweakBar.h \
    RIV/helper.h \
    RIV/reporter.h \
    RIV/sandbox.h \
    RIV/trackball.h \
    imagewidget.h \
    heatmapwidget.h \
    window.h \
    uiwidget.h \
    pcwidget.h
