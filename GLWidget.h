#pragma once

#include "Core/Polygons2.h"
#include <QGLWidget>
#include <QGLFormat>
#include <vector>
#include "Core/PointClouds2.h"
#include "Core/TriangulatedMeshes3.h"
#include "Core/Lights.h"
#include "Core/PointCloud3.h"

class GLWidget: public QGLWidget
{
    Q_OBJECT

private:

    // variables defining the projection matrix
    float       _aspect;            // aspect ratio of the rendering window
    float       _fovy;              // field of view in direction y
    float       _z_near, _z_far;    // distance of near and far clipping planes

    // variables defining the model-view matrix
    float       _eye[3], _center[3], _up[3];

    // variables needed by transformations
    int         _angle_x, _angle_y, _angle_z;
    double      _zoom;
    double      _trans_x, _trans_y, _trans_z;

    bool    _show_point_cloud;
    cg::PointCloud2 _point_cloud;
    cg::Polygon2*   _polygon;


    bool    _show_model;
    cg::TriangulatedMesh3 *_mesh;
    cg::DirectionalLight   *_dl;

    bool _show_polygon;
    cg::Polygon2 *_polygon_2;

    bool _show_convex_hull;
    cg::PointCloud3 *_point_cloud_3;

public:
    // special and default constructor
    GLWidget(QWidget* parent = 0);
    ~GLWidget();
    // redeclared virtual functions
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
signals:
    void show_volume(double value);

public slots:
    // public event handling methods/slots
    void set_angle_x(int value);
    void set_angle_y(int value);
    void set_angle_z(int value);

    void set_zoom_factor(double value);

    void set_trans_x(double value);
    void set_trans_y(double value);
    void set_trans_z(double value);

    void loadPointCloud();
    void slowConvexHull();
    void quick_hull();
    void fast_hull();


    void load_model();

    void load_polygon();

    void load_3d();
    void convex_3d();

    void step_convex();
};
