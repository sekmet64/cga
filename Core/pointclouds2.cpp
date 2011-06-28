#include "PointClouds2.h"
#include <algorithm>
#include <cmath>

using namespace cg;
using namespace std;

ostream& cg::operator <<(ostream& lhs, const PointCloud2& rhs)
{
    lhs << rhs._usage_flag << endl;
    lhs << rhs._points.size() << endl;
    for (PointCloud2::const_vertex_iterator it = rhs._points.begin();
        it != rhs._points.end(); ++it)
    {
        lhs << *it;
    }
    return lhs;
}

istream& cg::operator >>(istream& lhs, PointCloud2& rhs)
{
    // cleanup
    rhs.DeleteVertexBufferObjectOfPoints();
    rhs._points.clear();

    int n;
    lhs >> rhs._usage_flag;
    lhs >> n;
    rhs._points.reserve(n);

    for (int i = 0; i < n; i++)
    {
        PointCloud2::Vertex2 v;
        lhs >> v;
        rhs._points.push_back(v);
    }

    rhs.UpdateVertexBufferObjectOfPoints();
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

void PointCloud2::Clear()
{
    _points.clear();
}

// generate convex hull by means of naive and realistic algorithms
Polygon2* PointCloud2::SlowConvexHull_DirectedExtremeEdges(
        GLenum usage_flag_of_convex_hull) const
{
    Polygon2* convex_hull = new Polygon2(usage_flag_of_convex_hull);

    unsigned i0 = 0;
    for (unsigned i = 1; i < _points.size(); ++i)
        if (_points[i].y() < _points[i0].y())
            i0 = i;

    unsigned i = i0;
    convex_hull->push_back(_points[i].x(), _points[i].y());
    int k;
    float alpha;
    double lowest;
    Vertex2 prev(_points[i][0], _points[i][1] - 1);
    do
    {
        lowest = 360.0;
        for (unsigned j = 0; j < _points.size(); ++j)
        {
            if (j != i)
            {
                DCoordinate2 a = static_cast<DCoordinate2>(prev);
                DCoordinate2 b = static_cast<DCoordinate2>(_points[i]);
                DCoordinate2 c = static_cast<DCoordinate2>(_points[j]);
                alpha = angle(a, b, c);

                if (alpha < lowest)
                {
                    lowest = alpha;
                    k = j;
                }
            }
        }

        prev = _points[i];
        convex_hull->push_back(_points[k].x(), _points[k].y());
        i = k;

    } while (i != i0);


    return convex_hull;
}

Polygon2* PointCloud2::QuickHull(GLenum usage_flag_of_convex_hull) const
{
    Polygon2 *quick_hull = new Polygon2(usage_flag_of_convex_hull);

    const Vertex2 *left_point = &_points[0];
    int ti = 0;

    for (unsigned i = 1; i < _points.size(); i++)
    {
        if (_points[i].x() > left_point->x())
        {
            left_point = &_points[i];
            ti = i;
        }
    }

    const Vertex2 *right_point = &_points[0];
    int tj = 0;

    for (unsigned i = 1; i < _points.size(); i++)
    {
        if (_points[i].x() < right_point->x())
        {
            right_point = &_points[i];
            tj = i;
        }
    }

    vector<Vertex2> hull;
    hull.push_back(_points[ti]);

    vector<Vertex2> hulla = QuickHull(_points[ti], _points[tj], _points);

    hull.insert(hull.end(), hulla.begin(), hulla.end());

    hull.push_back(_points[tj]);

    vector<Vertex2> hullb = QuickHull(_points[tj], _points[ti], _points);

    hull.insert(hull.end(), hullb.begin(), hullb.end());

    for (vector<Vertex2>::iterator it = hull.begin(); it != hull.end(); it++)
    {
        quick_hull->push_back((*it).x(), (*it).y());
    }

    return quick_hull;
}


vector<PointCloud2::Vertex2> PointCloud2::QuickHull(Vertex2 i, Vertex2 j, vector<Vertex2> points) const
{
    if (points.size() == 0)
    {
        vector<Vertex2> empty_vector;
        return empty_vector;
    }
    else
    {
        int k = MaxDistance(i, j, points);

        vector<Vertex2> a = RightOf(i, points[k], points);

        vector<Vertex2> b = RightOf(points[k], j, points);

        vector<Vertex2> result_a = QuickHull(i, points[k], a);
        vector<Vertex2> result_b = QuickHull(points[k], j, b);


        result_a.push_back(points[k]);
        result_a.insert(result_a.end(), result_b.begin(), result_b.end());
        return result_a;
    }
}

int PointCloud2::MaxDistance(Vertex2 iv, Vertex2 jv, vector<Vertex2> &points) const
{
    double max_distance = 0;

    int max_k = 0;

    for (unsigned k = 1; k < points.size(); k++)
    {

        if (points[k] != iv && points[k] != jv)
        {
            double L = sqrt( pow(jv.x() - iv.x(), 2) + pow(jv.y() - iv.y(), 2));
            double s = ((iv.y() - points[k].y()) * (jv.x() - iv.x()) -
                    (iv.x() - points[k].x()) * (jv.y() - iv.y())) / (L * L);

            if (s > 0)
            {
                double distance = fabs(s) * L;

                if (max_distance < distance)
                {
                    max_distance = distance;
                    max_k = k;
                }
            }
        }
    }
    return max_k;
}

vector<PointCloud2::Vertex2> PointCloud2::RightOf(Vertex2 iv, Vertex2 jv,
    vector<Vertex2> &points) const
{
    vector<Vertex2> right_points;

    for (unsigned k = 0; k < points.size(); k++)
    {
        if (points[k] != iv && points[k] != jv)
        {
            double L = sqrt( pow(jv.x() - iv.x(), 2) + pow(jv.y() - iv.y(), 2));
            double s = ((iv.y() - points[k].y()) * (jv.x() - iv.x()) -
                    (iv.x() - points[k].x()) * (jv.y() - iv.y())) / (L * L);

            if (s > 0)
            {
                right_points.push_back(points[k]);
            }
        }
    }

    return right_points;
}

Polygon2* PointCloud2::FastConvexHull(GLboolean sort_points_in_place, GLenum usage_flag_of_convex_hull)
{
    sort(_points.begin(), _points.end());
    list<Vertex2> upper;
    upper.push_back(_points[0]);
    upper.push_back(_points[1]);

    for (unsigned i = 2; i < _points.size(); i++)
    {
        upper.push_back(_points[i]);
        while (upper.size() > 2 && !RightTurn(last(upper, 2), last(upper, 1),
                                              upper.back()))
        {
            list<Vertex2>::iterator it = upper.end();
            advance(it, -2);
            upper.erase(it);
        }

    }
    list<Vertex2> lower;
    lower.push_back(_points[_points.size()-1]);
    lower.push_back(_points[_points.size()-2]);

    for (int i = _points.size() - 3; i >= 0; i--)
    {
        lower.push_back(_points[i]);
        while (lower.size() > 2 && !RightTurn(last(lower, 2), last(lower, 1),
                                              lower.back()))
        {
            list<Vertex2>::iterator it = lower.end();
            advance(it, -2);
            lower.erase(it);
        }
    }
    lower.erase(lower.begin());
    lower.erase(--lower.end());

    Polygon2 *result = new Polygon2(usage_flag_of_convex_hull);
    for (list<Vertex2>::iterator it = lower.begin(); it != lower.end(); it++)
    {
        result->push_back((*it).x(), (*it).y());
    }
    for (list<Vertex2>::iterator it = upper.begin(); it != upper.end(); it++)
    {
        result->push_back((*it).x(), (*it).y());
    }
    return result;

}

PointCloud2::Vertex2 PointCloud2::last(list<Vertex2> &vlist, int count)
{
    list<Vertex2>::reverse_iterator it = vlist.rbegin();
    advance(it, count);
    return *it;
}

bool PointCloud2::RightTurn(Vertex2 a, Vertex2 b, Vertex2 c)
{
    return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x()) > 0;
}

// destructor
PointCloud2::~PointCloud2()
{
    DeleteVertexBufferObjectOfPoints();
}

float PointCloud2::angle(const DCoordinate2 &a, const DCoordinate2 &b, const DCoordinate2 &c) const
{
    DCoordinate2 ab = a - b;
    DCoordinate2 cb = b - c;

    float dot = ab * cb;

    float ab_sqr = ab * ab;
    float cb_sqr = cb * cb;

    float cos_sqr = dot * dot / ab_sqr / cb_sqr;

    float cos2 = 2 * cos_sqr - 1;

    const float pi = 3.141592f;

    float alpha2 = (cos2 < -1) ? pi : (cos2 >= 1) ? 0 : acosf(cos2);

    float rslt = alpha2 / 2;
    float rs = rslt * 180 / pi;

    if (dot < 0)
        rs = 180 - rs;

    float det = ab ^ cb;
    if (det < 0)
        rs = -rs;

    return rs;
}
