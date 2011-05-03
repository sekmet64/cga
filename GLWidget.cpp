#include "GLWidget.h"
#include <iostream>
#include <fstream>
#include "MainWindow.h"
#include "Core/Polygons2.h"
#include <QFileDialog>

using namespace cg;
using namespace std;

//--------------------------------
// special and default constructor
//--------------------------------
GLWidget::GLWidget(QWidget *parent): QGLWidget(QGLFormat(
        QGL::Rgba | QGL::DepthBuffer | QGL::DoubleBuffer),
        parent), _polygon(0)
{

}

//--------------------------------------------------------------------------------------
// this virtual function is called once before the first call to paintGL() or resizeGL()
//--------------------------------------------------------------------------------------
void GLWidget::initializeGL()
{
    // creating a perspective projection matrix
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    _aspect = (float)width() / (float)height();
    _z_near = 1.0;
    _z_far = 1000.0;
    _fovy = 45.0;

    gluPerspective(_fovy, _aspect, _z_near, _z_far);

    // setting the model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    _eye[0] = _eye[1] = 0.0, _eye[2] = 6.0;
    _center[0] = _center[1] = _center[2] = 0.0;
    _up[0] = _up[2] = 0.0, _up[1] = 1.0;

    gluLookAt(_eye[0], _eye[1], _eye[2], _center[0], _center[1], _center[2], _up[0], _up[1], _up[2]);

    // enabling depth test
    glEnable(GL_DEPTH_TEST);

    // setting the color of background
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // initial values of transformation parameters
    _zoom = 1.0;
    _angle_x = _angle_y = _angle_z = 0.0;
    _trans_x = _trans_y = _trans_z = 0.0;

    glewInit();
}


GLWidget::~GLWidget()
{

}

//-----------------------
// the rendering function
//-----------------------
void GLWidget::paintGL()
{
    // clears the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // stores/duplicates the original model view matrix
    glPushMatrix();

        // applying transformations
        glRotatef(_angle_x, 1.0, 0.0, 0.0);
        glRotatef(_angle_y, 0.0, 1.0, 0.0);
        glRotatef(_angle_z, 0.0, 0.0, 1.0);

        glTranslated(_trans_x, _trans_y, _trans_z);

        glScaled(_zoom, _zoom, _zoom);

        _point_cloud.RenderPoints();

        if (_polygon != 0)
        {
            _polygon->Render();
        }

    // pops the current matrix stack, replacing the current matrix with the one below it on the stack,
    // i.e., the original model view matrix is restored
    glPopMatrix();

}

//----------------------------------------------------------------------------
// when the main window is resized one needs to redefine the projection matrix
//----------------------------------------------------------------------------
void GLWidget::resizeGL(int w, int h)
{
    // setting the new size of the rendering context
    glViewport(0, 0, w, h);

    // redefining the projection matrix
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    _aspect = (float)w / (float)h;

    gluPerspective(_fovy, _aspect, _z_near, _z_far);

    // switching back to the model view matrix
    glMatrixMode(GL_MODELVIEW);

    updateGL();
}

//-----------------------------------
// implementation of the public slots
//-----------------------------------

void GLWidget::set_angle_x(int value)
{
    if (_angle_x != value)
    {
        _angle_x = value;
        updateGL();
    }
}

void GLWidget::set_angle_y(int value)
{
    if (_angle_y != value)
    {
        _angle_y = value;
        updateGL();
    }
}

void GLWidget::set_angle_z(int value)
{
    if (_angle_z != value)
    {
        _angle_z = value;
        updateGL();
    }
}

void GLWidget::set_zoom_factor(double value)
{
    if (_zoom != value)
    {
        _zoom = value;
        updateGL();
    }
}

void GLWidget::set_trans_x(double value)
{
    if (_trans_x != value)
    {
        _trans_x = value;
        updateGL();
    }
}

void GLWidget::set_trans_y(double value)
{
    if (_trans_y != value)
    {
        _trans_y = value;
        updateGL();
    }
}

void GLWidget::set_trans_z(double value)
{
    if (_trans_z != value)
    {
        _trans_z = value;
        updateGL();
    }
}

void GLWidget::loadPointCloud()
{
    QString filename = QFileDialog::getOpenFileName(this,
        "Load point cloud", "", "Point cloud files (*.txt)");

    ifstream ifs(filename.toAscii().constData());
    ifs >> _point_cloud;
    repaint();
}

void GLWidget::slowConvexHull()
{
    _polygon = _point_cloud.SlowConvexHull_DirectedExtremeEdges();
    _polygon->UpdateVertexBufferObject();
    repaint();
}
