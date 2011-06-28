SOURCES += main.cpp \
    MainWindow.cpp \
    SideWidget.cpp \
    GLWidget.cpp \
    Core/Polygons2.cpp \
    Core/PointClouds2.cpp \
    Core/TriangulatedMeshes3.cpp \
    Core/Materials.cpp \
    Core/Lights.cpp \
    Core/Polygon/Vertex.cpp \
    Core/Polygon/Edge.cpp \
    Core/PointCloud3.cpp \
    Core/EdgeList/dcedgelist.cpp \
    Core/EdgeList/halfedge.cpp \
    Core/EdgeList/vertex3.cpp \
    Core/EdgeList/facet.cpp
FORMS += \
    SideWidget.ui \
    MainWindow.ui
HEADERS += MainWindow.h \
    SideWidget.h \
    GLWidget.h \
    Core/DCoordinates3.h \
    Core/DCoordinates2.h \
    Core/Polygons2.h \
    Core/PointClouds2.h \
    Core/TriangulatedMeshes3.h \
    Core/TriangularFaces.h \
    Core/TCoordinates4.h \
    Core/Materials.h \
    Core/Lights.h \
    Core/HCoordinates3.h \
    Core/Colors4.h \
    Core/Exceptions.h \
    Core/Polygon/Vertex.h \
    Core/Polygon/Edge.h \
    Core/PointCloud3.h \
    Core/EdgeList/dcedgelist.h \
    Core/EdgeList/halfedge.h \
    Core/EdgeList/vertex3.h \
    Core/EdgeList/facet.h

INCLUDEPATH += Dependencies/Include

LIBS += Dependencies/Lib/GL/glew32.lib \
    Dependencies/Lib/GL/glew32.dll

QT += opengl

QMAKE_CXXFLAGS += -std=c++0x
