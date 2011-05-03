#include "GLWidget.h"
#include <iostream>
#include <fstream>
#include "MainWindow.h"
#include "Core/Polygons2.h"

using namespace cg;

//--------------------------------
// special and default constructor
//--------------------------------
GLWidget::GLWidget(QWidget *parent): QGLWidget(QGLFormat(
        QGL::Rgba | QGL::DepthBuffer | QGL::DoubleBuffer),
        parent)
{
//    // read poly from file
//    std::ifstream input;
//    input.open("poly.txt");
//    input >> _polygon;
//    input.close();
    _vertex_count = 10000;
    GLdouble alpha = 0.0;

    GLdouble delta = 6.2831 / _vertex_count;
    GLdouble radius = 1.0;

    Polygon2::vertex_iterator it = _polygon[0];
    Polygon2::Vertex2 v;
    for (GLuint i = 0; i < _vertex_count; ++i)
    {
        v[0] = radius * cos(alpha);
        v[1] = radius + sin(alpha);
        it = _polygon.InsertVertex(it, v);
        alpha += delta;
    }



    ((MainWindow*)parent)->setArea(QString::number(_polygon.CalculateSignedArea()));
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

//    _vertex_count = 10000;
//    for (int i = 0; i < _vertex_count; ++i)
//    {
//        _x.push_back(-1.0 + 2.0 *  rand() / RAND_MAX);
//        _y.push_back(-1.0 + 2.0 *  rand() / RAND_MAX);
//        _z.push_back(-1.0 + 2.0 *  rand() / RAND_MAX);
//        _r.push_back( (float) rand() / RAND_MAX);
//        _g.push_back( (float) rand() / RAND_MAX);
//        _b.push_back( (float) rand() / RAND_MAX);
//    }

//    _dl_triangles = 0;

//    _dl_triangles = glGenLists(1);

//    if (!_dl_triangles)
//    {
//        // error
//    }

//    glNewList(_dl_triangles, GL_COMPILE);
//        glBegin(GL_LINES);
//            for (int i=0; i < _vertex_count; ++i) {
//                glColor3f(_r[i], _g[i], _b[i]);
//                glVertex3f(_x[i], _y[i], _z[i]);
//            }
//        glEnd();
//    glEndList();



    glewInit();
    _polygon.UpdateVertexBufferObject(GL_STATIC_DRAW);
}


GLWidget::~GLWidget()
{
if (_dl_triangles)
    glDeleteLists(_dl_triangles, 1);
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

//        // render something
//        glBegin(GL_TRIANGLES);
//            glColor3f(1.0, 0.0, 0.0);
//            glVertex3f(1.0, 0.0, 0.0);

//            glColor3f(0.0, 1.0, 0.0);
//            glVertex3f(0.0, 1.0, 0.0);

//            glColor3f(0.0, 0.0, 1.0);
//            glVertex3f(0.0, 0.0, 1.0);
//        glEnd();

//        glBegin(GL_TRIANGLES);
//            for (int i=0; i < _vertex_count; ++i) {
//                glColor3f(_r[i], _g[i], _b[i]);
//                glVertex3f(_x[i], _y[i], _z[i]);
//            }
//        glEnd();

//        if (_dl_triangles)
//        {
//            glCallList(_dl_triangles);
//        }
        _polygon.Render();

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
