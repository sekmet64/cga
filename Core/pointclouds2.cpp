#include "PointClouds2.h"

using namespace cg;
using namespace std;

ostream& cg::operator <<(ostream& lhs, const PointCloud2& rhs)
{
    lhs << "??" << endl;
    lhs << rhs._points.size();
    for (PointCloud2::const_vertex_iterator it = rhs._points.begin();
        it != rhs._points.end(); ++it)
    {
        lhs << *it;
    }
    return lhs;
}

istream& cg::operator >>(istream& lhs, PointCloud2& rhs)
{
    int n;
    lhs >> n;//??
    lhs >> n;
    rhs._points.reserve(n);
    for (int i = 0; i < n; i++)
    {
        PointCloud2::Vertex2 v;
        lhs >> v;
        rhs._points.push_back(v);
    }
    return lhs;
}

// special/default constructor
PointCloud2::PointCloud2(GLuint point_count,
    GLenum usage_flag) :
        _usage_flag(usage_flag),
        _points(point_count)
{
}

// copy constructor
PointCloud2::PointCloud2(const PointCloud2& pc) : _usage_flag(pc._usage_flag),
    _points(pc._points)

{
    if (pc._vbo_point_cloud)
        UpdateVertexBufferObjectOfPoints();
}

// assignment operator
PointCloud2& PointCloud2::operator =(const PointCloud2& rhs)
{
    if (this != &rhs)
    {
        DeleteVertexBufferObjectOfPoints();

        _usage_flag = rhs._usage_flag;
        _points = rhs._points;

        if (rhs._vbo_point_cloud)
            UpdateVertexBufferObjectOfPoints();
    }
    return *this;
}

// vertex buffer handling methods
GLvoid    PointCloud2::DeleteVertexBufferObjectOfPoints()
{
    if (_vbo_point_cloud)
    {
        glDeleteBuffers(1, &_vbo_point_cloud);
        _vbo_point_cloud = 0;
    }
}

GLboolean PointCloud2::RenderPoints() const
{
    if (!_vbo_point_cloud)
        return GL_FALSE;

    glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_point_cloud);
        glVertexPointer(2, GL_FLOAT, 0, (const GLvoid*)0);

        glDrawArrays(GL_POINTS, 0, _points.size());

        glBindBuffer(GL_VERTEX_ARRAY, 0);
    glDisableClientState(GL_VERTEX_ARRAY);

    return GL_TRUE;
}

GLboolean PointCloud2::UpdateVertexBufferObjectOfPoints()
{
    DeleteVertexBufferObjectOfPoints();

    glGenBuffers(1, &_vbo_point_cloud);
    if (!_vbo_point_cloud)
        return GL_FALSE;

    glBindBuffer(GL_ARRAY_BUFFER, _vbo_point_cloud);

    GLuint byte_size_of_vertices = 2 * _points.size() * sizeof(GL_FLOAT);
    glBufferData(GL_ARRAY_BUFFER, byte_size_of_vertices, 0, GL_STATIC_DRAW);
    GLfloat *coordinate = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    for (const_vertex_iterator it = _points.begin(); it != _points.end();
        ++it)
    {
        for (GLuint c = 0; c < 2; ++c)
        {
            *coordinate = (GLfloat)(*it)[c];
            ++coordinate;
        }
    }

    if (!glUnmapBuffer(GL_ARRAY_BUFFER))
        DeleteVertexBufferObjectOfPoints();

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return GL_TRUE;
}

// particle handling methods/operators
PointCloud2::Vertex2  PointCloud2::operator[](GLuint index) const
{
    return _points[index];
}

PointCloud2::Vertex2& PointCloud2::operator[](GLuint index)
{
    return _points[index];
}

// generate convex hull by means of naive and realistic algorithms
Polygon2* PointCloud2::SlowConvexHull_DirectedExtremeEdges(
        GLenum usage_flag_of_convex_hull) const
{
    bool valid;
    Polygon2* convex_hull = new Polygon2(usage_flag_of_convex_hull);

    for (unsigned p = 0; p < _points.size(); p++)
    {
        for (unsigned q = 0; q < _points.size(); q++)
        {
            if (p != q)
            {
                valid = true;
                for (unsigned r = 0; r < _points.size(); r++)
                {
                    if (r != p && r != q)
                    {
                        if ((_points[q].x() - _points[p].x()) * (_points[r].y() -
                            _points[p].y()) - (_points[q].y() - _points[p].y()) *
                            (_points[r].x() - _points[p].x()))
                            valid = false;
                    }
                }
                if (valid)
                {
                    convex_hull->push_back(_points[p].x(), _points[p].y());
                    convex_hull->push_back(_points[q].x(), _points[q].y());
                }
            }
        }
    }
    return convex_hull;
}

Polygon2* PointCloud2::QuickHull(GLenum usage_flag_of_convex_hull) const
{
    return 0;
}

Polygon2* PointCloud2::FastConvexHull(GLboolean sort_points_in_place, GLenum usage_flag_of_convex_hull)
{
    return 0;
}

// destructor
PointCloud2::~PointCloud2()
{
    DeleteVertexBufferObjectOfPoints();
}
