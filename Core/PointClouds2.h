#pragma once

#include "DCoordinates2.h"
#include "Polygons2.h"
#include <iostream>
#include <vector>
#include <list>

namespace cg
{
    class PointCloud2
    {
        // input/output to/from stream
        friend std::ostream& operator <<(std::ostream& lhs, const PointCloud2& rhs);
        friend std::istream& operator >>(std::istream& lhs, PointCloud2& rhs);

    public:
        class Vertex2: public DCoordinate2
        {
        public:
            Vertex2() : DCoordinate2() {}
            Vertex2(GLdouble x, GLdouble y) { _data[0] = x; _data[1] = y; }


            // for lexicographical ordering
            bool operator <(const Vertex2& rhs) const
            {
                return (_data[0] < rhs._data[0] || (_data[0] == rhs._data[0] && _data[1] < rhs._data[1]));
            }

            bool operator!=(const Vertex2 &v) { return _data[0] != v._data[0] && _data[1] != v._data[1]; }
        };

        // const and non const vertex iterators
        typedef std::vector<Vertex2>::iterator        vertex_iterator;
        typedef std::vector<Vertex2>::const_iterator  const_vertex_iterator;

    protected:
        GLenum                  _usage_flag;
        GLuint                  _vbo_point_cloud;
        std::vector<Vertex2>    _points;

    public:
        // special/default constructor
        PointCloud2(GLuint point_count = 0, GLenum usage_flag = GL_STATIC_DRAW);

        // copy constructor
        PointCloud2(const PointCloud2& pc);

        // assignment operator
        PointCloud2& operator =(const PointCloud2& rhs);

        // vertex buffer handling methods
        GLvoid    DeleteVertexBufferObjectOfPoints();
        GLboolean RenderPoints() const;
        GLboolean UpdateVertexBufferObjectOfPoints();

        // particle handling methods/operators
        Vertex2  operator[](GLuint index) const;
        Vertex2& operator[](GLuint index);

        void Clear();

        // generate convex hull by means of naive and realistic algorithms
        Polygon2* SlowConvexHull_DirectedExtremeEdges(GLenum usage_flag_of_convex_hull = GL_STATIC_DRAW) const;
        Polygon2* QuickHull(GLenum usage_flag_of_convex_hull = GL_STATIC_DRAW) const;
        Polygon2* FastConvexHull(GLboolean sort_points_in_place = GL_TRUE, GLenum usage_flag_of_convex_hull = GL_STATIC_DRAW);

        // destructor
        virtual ~PointCloud2();

    private:
        std::vector<Vertex2> QuickHull(Vertex2 i, Vertex2 j, std::vector<Vertex2> points) const;

        int MaxDistance(Vertex2 iv, Vertex2 jv, std::vector<Vertex2>& points) const;

        std::vector<Vertex2> RightOf(Vertex2 iv, Vertex2 jv, std::vector<Vertex2> &points) const;

        float angle(const DCoordinate2 &a, const DCoordinate2 &b, const DCoordinate2 &c) const;
        bool RightTurn(Vertex2 a, Vertex2 b, Vertex2 c);
        Vertex2 last(std::list<Vertex2> &vlist, int count);
    };
}
