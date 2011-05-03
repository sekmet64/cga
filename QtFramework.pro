SOURCES += main.cpp \
    MainWindow.cpp \
    SideWidget.cpp \
    GLWidget.cpp \
    Core/Polygons2.cpp \
    Core/PointClouds2.cpp
FORMS += \
    SideWidget.ui \
    MainWindow.ui
HEADERS += MainWindow.h \
    SideWidget.h \
    GLWidget.h \
    Core/DCoordinates3.h \
    Core/DCoordinates2.h \
    Core/Polygons2.h \
    Core/PointClouds2.h

INCLUDEPATH += Dependencies/Include

LIBS += Dependencies/Lib/GL/glew32.lib \
    Dependencies/Lib/GL/glew32.dll

QT += opengl
