#pragma once

#include "DCoordinates2.h"
#include <iostream>
#include <list>
#include <queue>
#include <utility>

namespace cg
{
    class Polygon2
    {
        // input/output to/from stream
        friend std::ostream& operator <<(std::ostream& lhs, const Polygon2& rhs);
        friend std::istream& operator >>(std::istream& lhs, Polygon2& rhs);

    public:
        class Vertex2: public DCoordinate2
        {

        public:
            Vertex2();
            Vertex2(GLdouble x, GLdouble y);

            bool operator<(const Vertex2 &vertex) const
            {
                return (y() < vertex.y() || (y() == vertex.y() && x() < vertex.x()));
            }

            bool operator==(const Vertex2 &rhs) const
            {
                return x() == rhs.x() && y() == rhs.y();
            }
        };

        typedef std::list<Vertex2>::iterator        vertex_iterator;
        typedef std::list<Vertex2>::const_iterator  const_vertex_iterator;

    protected:
        GLuint              _usage_flag;
        GLuint              _vbo_polygon;
        std::list<Vertex2>  _vertices;

    public:
        // special/default constructor
        Polygon2(GLenum usage_flag = GL_STATIC_DRAW);

        // copy constructor
        Polygon2(const Polygon2& poly);

        // assignment operator
        Polygon2& operator =(const Polygon2& rhs);

        // get iterator either to non const, or to const vertex
        vertex_iterator       operator [](GLuint index);
        const_vertex_iterator operator [](GLuint index) const;

        // inserts a vertex before a position specified by a vertex iterator
        // return value: is an iterator pointing to the position where the new
        //               element was inserted into the list
        vertex_iterator InsertVertex(vertex_iterator before_position, const Vertex2& v);

        // erases an existing vertex specified by a vertex iterator
        // return value: a bidirectional iterator that designates the first element
        //               remaining beyond any elements removed, or a pointer to the
        //               end of the list if no such element exists
        vertex_iterator EraseVertex(vertex_iterator at_position);

        void push_back(const GLdouble x, const GLdouble y);

        // vertex buffer object handling methods
        GLvoid    DeleteVertexBufferObject();
        GLboolean Render(GLenum render_mode = GL_LINE_LOOP) const;
        GLboolean UpdateVertexBufferObject(GLenum usage_flag = GL_STATIC_DRAW);

        // calculates the signed area of the simple polygon
        GLdouble CalculateSignedArea() const;

        void MakeMonotone();

        // destructor
        virtual ~Polygon2();
    };
}
