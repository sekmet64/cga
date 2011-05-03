#include "Polygons2.h"
#include <cmath>

using namespace cg;
using namespace std;

Polygon2::Vertex2::Vertex2() : DCoordinate2()
{
}

Polygon2::Vertex2::Vertex2(GLdouble x, GLdouble y) : DCoordinate2(x, y)
{
}

// special/default constructor
Polygon2::Polygon2(GLenum usage_flag) : _usage_flag(usage_flag), _vbo_polygon(0)
{
}

// copy constructor
Polygon2::Polygon2(const Polygon2& poly) : _usage_flag(poly._usage_flag),
    _vbo_polygon(0), _vertices(poly._vertices)
{
    if (poly._vbo_polygon)
        UpdateVertexBufferObject(_usage_flag);
}

// assignment operator
Polygon2& Polygon2::operator =(const Polygon2& rhs)
{
    if (this != &rhs)
    {
        DeleteVertexBufferObject();

        _usage_flag = rhs._usage_flag;
        _vertices = rhs._vertices;

        if (rhs._vbo_polygon)
            UpdateVertexBufferObject(_usage_flag);
    }
    return *this;
}

// get iterator either to non const, or to const vertex
Polygon2::vertex_iterator Polygon2::operator [](GLuint index)
{
    if (index >= _vertices.size())
        return _vertices.end();

    vertex_iterator it = _vertices.begin();

    for (GLuint i = 0; i < index; ++i)
        ++it;

    return it;
}

Polygon2::const_vertex_iterator Polygon2::operator [](GLuint index) const
{
    if (index >= _vertices.size())
        return _vertices.end();

    const_vertex_iterator it = _vertices.begin();

    for (GLuint i = 0; i < index; ++i)
        ++it;

    return it;
}

// inserts a vertex before a position specified by a vertex iterator
// return value: is an iterator pointing to the position where the new
//               element was inserted into the list
Polygon2::vertex_iterator Polygon2::InsertVertex(vertex_iterator before_position,
                                       const Vertex2& v)
{
    return _vertices.insert(before_position, v);
}

// erases an existing vertex specified by a vertex iterator
// return value: a bidirectional iterator that designates the first element
//               remaining beyond any elements removed, or a pointer to the
//               end of the list if no such element exists
Polygon2::vertex_iterator Polygon2::EraseVertex(vertex_iterator at_position)
{
    return _vertices.erase(at_position);
}

// vertex buffer object handling methods
GLvoid Polygon2::DeleteVertexBufferObject()
{
    if (_vbo_polygon)
    {
        glDeleteBuffers(1, &_vbo_polygon);
        _vbo_polygon = 0;
    }
}

GLboolean Polygon2::Render(GLenum render_mode) const
{
    if (!_vbo_polygon)
        return GL_FALSE;
    if (render_mode != GL_LINE_LOOP && render_mode != GL_POINTS)
        return GL_FALSE;

    glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_polygon);
        glVertexPointer(2, GL_FLOAT, 0, (const GLvoid*)0);

        glDrawArrays(render_mode, 0, _vertices.size());

        glBindBuffer(GL_VERTEX_ARRAY, 0);
    glDisableClientState(GL_VERTEX_ARRAY);

    return GL_TRUE;
}
GLboolean Polygon2::UpdateVertexBufferObject(GLenum usage_flag)
{
    if (usage_flag != GL_STREAM_DRAW && usage_flag != GL_STREAM_READ &&
        usage_flag != GL_STREAM_COPY && usage_flag != GL_DYNAMIC_DRAW &&
        usage_flag != GL_DYNAMIC_READ && usage_flag != GL_DYNAMIC_COPY &&
        usage_flag != GL_STATIC_DRAW && usage_flag != GL_STATIC_READ &&
        usage_flag != GL_STATIC_COPY)
        return GL_FALSE;

    DeleteVertexBufferObject();

    glGenBuffers(1, &_vbo_polygon);
    if (!_vbo_polygon)
        return GL_FALSE;

    glBindBuffer(GL_ARRAY_BUFFER, _vbo_polygon);

    GLuint byte_size_of_vertices = 2 * _vertices.size() * sizeof(GL_FLOAT);
    glBufferData(GL_ARRAY_BUFFER, byte_size_of_vertices, 0, usage_flag);
    GLfloat *coordinate = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    for (const_vertex_iterator it = _vertices.begin(); it != _vertices.end();
        ++it)
    {
        for (GLuint c = 0; c < 2; ++c)
        {
            *coordinate = (GLfloat)(*it)[c];
            ++coordinate;
        }
    }

    if (!glUnmapBuffer(GL_ARRAY_BUFFER))
        DeleteVertexBufferObject();

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return GL_TRUE;
}

// calculates the signed area of the simple polygon
GLdouble Polygon2::CalculateSignedArea() const
{
    GLdouble area = 0;

    for (const_vertex_iterator second = _vertices.begin(), first = second++;
        second != _vertices.end(); first++, second++)
    {
        area += (*first)^(*second);
    }
    // triangle from last vertex and first
    area += (*_vertices.rbegin())^(*_vertices.begin());
    return abs(area / 2.0);
}

// destructor
Polygon2::~Polygon2()
{
    DeleteVertexBufferObject();
    _vertices.clear();
}

std::ostream& cg::operator <<(std::ostream& lhs, const Polygon2& rhs)
{
    lhs << rhs._vertices.size();
    for (Polygon2::const_vertex_iterator it = rhs._vertices.begin();
        it != rhs._vertices.end(); ++it)
    {
        lhs << *it;
    }
    return lhs;
}

std::istream& cg::operator >>(std::istream& lhs, Polygon2& rhs)
{
    int n;
    lhs >> n;
    for (int i = 0; i < n; i++)
    {
        Polygon2::Vertex2 v;
        lhs >> v;
        rhs._vertices.push_back(v);
    }
    return lhs;
}

void Polygon2::push_back(const GLdouble x, const GLdouble y)
{
    Vertex2 v(x, y);
    _vertices.push_back(v);
}
