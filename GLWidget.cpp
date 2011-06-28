#include "GLWidget.h"
#include <iostream>
#include <fstream>
#include "MainWindow.h"
#include "Core/Polygons2.h"
#include <QFileDialog>
#include "Core/Materials.h"

using namespace cg;
using namespace std;

//--------------------------------
// special and default constructor
//--------------------------------
GLWidget::GLWidget(QWidget *parent): QGLWidget(QGLFormat(
        QGL::Rgba | QGL::DepthBuffer | QGL::DoubleBuffer),
        parent),
    _show_point_cloud(true),
    _polygon(0),
    _show_model(false),
    _mesh(0),
    _show_polygon(false),
    _polygon_2(0),
    _show_convex_hull(false),
    _point_cloud_3(0)

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

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // enabling depth test
    glEnable(GL_DEPTH_TEST);

    // setting the color of background
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // initial values of transformation parameters
    _zoom = 1.0;
    _angle_x = _angle_y = _angle_z = 0.0;
    _trans_x = _trans_y = _trans_z = 0.0;

    glewInit();



    _dl = new DirectionalLight(GL_LIGHT0, HCoordinate3(0.0,0.0,1.0,0.0),
        Color4(0.4,0.4,0.4,1.0),
        Color4(0.8,0.8,0.8,1.0),
        Color4(1.0,1.0,1.0,1.0));





//    glEnable(GL_POINT_SMOOTH);
//    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_LINE_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);


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

        if (_show_point_cloud)
        {
            _point_cloud.RenderPoints();

            if (_polygon != 0)
            {
                _polygon->Render();
            }
        }

        if (_show_model)
        {
            glEnable(GL_LIGHTING);
            glEnable(GL_NORMALIZE);

            MatFBEmerald.Apply();
            _dl->Enable();
            _mesh->Render();
            _dl->Disable();

            glDisable(GL_LIGHTING);
            glDisable(GL_NORMALIZE);
        }

        if (_show_polygon)
        {
            _polygon_2->Render();
        }

        if (_show_convex_hull)
        {
            _point_cloud_3->Render();
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
    _point_cloud.Clear();
    if (_polygon != NULL)
        delete _polygon;
    ifs >> _point_cloud;
    _show_point_cloud = true;
    _show_model = false;
    _show_polygon = false;
    _show_convex_hull = false;
    repaint();
}

void GLWidget::slowConvexHull()
{
    _polygon = _point_cloud.SlowConvexHull_DirectedExtremeEdges();
    _polygon->UpdateVertexBufferObject();
    repaint();
}

void GLWidget::quick_hull()
{
    _polygon = _point_cloud.QuickHull();
    _polygon->UpdateVertexBufferObject();
    repaint();
}

void GLWidget::fast_hull()
{
    _polygon = _point_cloud.FastConvexHull();
    _polygon->UpdateVertexBufferObject();
    repaint();
}


void GLWidget::load_model()
{
    QString filename = QFileDialog::getOpenFileName(this,
        "Load model", "", "Object File Format files (*.off)");

    if (_mesh != 0)
    {
        delete _mesh;
    }

    _mesh = new TriangulatedMesh3();
    _mesh->LoadFromOFF(filename.toAscii().constData(), GL_TRUE);
    _mesh->UpdateVertexBufferObjects();
    emit show_volume(_mesh->CalculateSignedVolume());
    _show_model = true;
    _show_point_cloud = false;
    _show_polygon = false;
    _show_convex_hull = false;
    repaint();

}

void GLWidget::load_polygon()
{
    QString filename = QFileDialog::getOpenFileName(this,
        "Load polygon", "", "Text files (*.txt)");

    if (!_polygon_2)
    {
        delete _polygon_2;
    }

    _polygon_2 = new Polygon2;
    ifstream ifs(filename.toAscii().constData());
    ifs >> *_polygon_2;
    _polygon_2->UpdateVertexBufferObject();
    _show_model = false;
    _show_point_cloud = false;
    _show_polygon = true;
    _show_convex_hull = false;
    repaint();
}

void GLWidget::load_3d()
{
    QString filename = QFileDialog::getOpenFileName(this,
        "Load model", "", "Object File Format files (*.off)");
    if (!_point_cloud_3)
        _point_cloud_3 = new PointCloud3;

    _point_cloud_3->LoadFromOFF(filename.toAscii().constData());

    _show_model = false;
    _show_point_cloud = false;
    _show_polygon = false;
    _show_convex_hull = true;
    repaint();
}

void GLWidget::convex_3d()
{
    _point_cloud_3->ConvexHull();
    repaint();
}

void GLWidget::step_convex()
{
    _point_cloud_3->SteppedConvexHull();
    repaint();
}
