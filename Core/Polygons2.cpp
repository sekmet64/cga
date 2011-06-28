#include "Polygons2.h"
#include <cmath>
#include <set>
#include <map>


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

class comp
{
public:
    bool operator()(const Polygon2::const_vertex_iterator &lhs, const Polygon2::const_vertex_iterator &rhs)
    {
        return *lhs < *rhs;
    }
};

typedef list<pair<Polygon2::Vertex2, Polygon2::Vertex2> >::const_iterator const_edge_iterator;

class edge_comp
{
public:
    bool operator()(const const_edge_iterator &lhs, const const_edge_iterator &rhs)
    {
        return *lhs < *rhs;
    }
};

double InteriorAngle(const Polygon2::Vertex2 &a, const Polygon2::Vertex2 &b, const Polygon2::Vertex2 &c)
{

    double bc = sqrt(pow(b.x() - c.x(), 2) + pow(b.y() - c.y(), 2));
    double ab = sqrt(pow(a.x() - b.x(), 2) + pow(a.y() - b.y(), 2));
    double ac = sqrt(pow(a.x() - c.x(), 2) + pow(a.y() - c.y(), 2));
    return acos((bc * bc - ab * ab - ac * ac) / (2 * ab * ac));
}

bool IsLeftOf(const_edge_iterator *lhs,
              const_edge_iterator *rhs)
{
    return true;
}

bool IsLeftOf(Polygon2::Vertex2 v1, Polygon2::Vertex2 v2)
{
    return (v2.x() < v1.x() || (v2.x() == v1.x() && v2.y() < v1.y()));
}

bool IsLeftOf(const_edge_iterator *lhs, Polygon2::Vertex2 rhs)
{
    return true;
}

void Polygon2::MakeMonotone()
{
    // make edge list from polygon
    list<pair<Vertex2, Vertex2> > edge_list;


    class SearchTree
    {
    public:

        class Node
        {
        public:
            Node(const_edge_iterator *it) : _value(it), _left(0), _right(0)
            {
            }

            const_edge_iterator* Value()
            {
                return _value;
            }

            void AddLeft(const_edge_iterator *edge)
            {
                if (!_left)
                    _left = new Node(edge);
                else
                    if (IsLeftOf(_left->Value(), edge))
                        _left->AddLeft(edge);
                    else
                        _left->AddRight(edge);
            }

            void AddRight(const_edge_iterator *edge)
            {
                if (!_right)
                    _right = new Node(edge);
                else
                    if (IsLeftOf(_right->Value(), edge))
                        _right->AddLeft(edge);
                    else
                        _right->AddRight(edge);
            }

            Node* Left()
            {
                return _left;
            }

            Node* Right()
            {
                return _right;
            }

            void SetValue(const_edge_iterator* value)
            {
                _value = value;
            }

        private:
            const_edge_iterator *_value;
            Node *_left;
            Node *_right;
        };

        SearchTree()
        {
            _root = 0;
        }

        void Insert(const_edge_iterator *edge)
        {
            if (!_root)
                _root = new Node(edge);
            else
                if (**_root->Value() < **edge)
                    _root->AddLeft(edge);
                else
                    _root->AddRight(edge);
        }

        const_edge_iterator* Search(const_vertex_iterator *it)
        {
            if ((**_root->Value()).first == **it)
            {
                if (IsLeftOf((**_root->Value()).second, **it))
                {
                    return _root->Value();
                }
            }
            else
            {
                if ((**_root->Value()).second == **it)
                {
                    if (IsLeftOf((**_root->Value()).first, **it))
                    {
                        return _root->Value();
                    }
                }
                else
                {
                    if (IsLeftOf(_root->Value(), **it))
                    {
                       return SearchR(_root, **it);
                    }
                }
            }

            //return const_edge_iterator;
        }

        const_edge_iterator* SearchR(Node *node, Vertex2 v)
        {
            if ((**node->Value()).first == v)
            {
                if (IsLeftOf((**node->Value()).second, v))
                {
                    return node->Value();
                }
            }
            else
            {
                if ((**node->Value()).second == v)
                {
                    if (IsLeftOf((**node->Value()).first, v))
                    {
                        return node->Value();
                    }
                }
                else
                {
                    if (IsLeftOf(node->Value(), v))
                    {
                       return SearchR(node, v);
                    }
                }
            }

        }

        void Delete(const_edge_iterator *edge)
        {
            DeleteR(_root, edge);
        }

        void DeleteR(Node *node, const_edge_iterator *edge)
        {
            if (*node->Value() == *edge)
            {
                Node *left = node->Left();
                Node *right = node->Right();
                // TODO bst delete
            }
        }

    private:
        Node* _root;
    };



    map<const_vertex_iterator, const_edge_iterator, comp> edge_of;

    const_vertex_iterator it1 = _vertices.begin();
    const_vertex_iterator it2 = _vertices.begin();
    ++it2;
    while (it2 != _vertices.end())
    {
        edge_list.push_back(make_pair((*it1), (*it2)));
        const_edge_iterator last = edge_list.end();
        --last;
        edge_of[it1] = last;
        ++it1;
        ++it2;
    }

    edge_list.push_back(make_pair(_vertices.back(), _vertices.front()));
    const_edge_iterator last = edge_list.end();
    --last;
    edge_of[it1] = last;

    // priority queue
    priority_queue<const_vertex_iterator, vector<const_vertex_iterator>,
            comp> queue;

    for (const_vertex_iterator it = _vertices.begin(); it != _vertices.end();
         ++it)
    {
        queue.push(it);
    }

    SearchTree search_tree;


    map<const_edge_iterator, const_vertex_iterator, edge_comp> helper;

    while (!queue.empty())
    {
        const_vertex_iterator top = queue.top();
        queue.pop();

        const_vertex_iterator prev = top;
        if (prev == _vertices.begin())
            prev = _vertices.end();
        --prev;

        const_vertex_iterator next = top;
        const_vertex_iterator last = _vertices.end();
        --last;
        if (next == last)
            next = _vertices.begin();
        else
            ++next;

        if ((*prev).y() < (*top).y() && (*next).y() < (*top).y())
        {
            if (InteriorAngle((*prev), (*top), (*next)) < 180.0)
            {
                // start vertex
                search_tree.Insert(&edge_of[top]);
                helper[edge_of[top]] = top;
            }
            else
            {
                // split vertex
                const_edge_iterator* left = search_tree.Search(&top);
                edge_list.push_back(make_pair(*top, *helper[*left]));
                helper[*left] = top;
                search_tree.Insert(&edge_of[top]);
                helper[edge_of[top]] = top;
            }
        }
        else
        {
            if ((*prev).y() > (*top).y() && (*next).y() > (*top).y())
            {
                if (InteriorAngle((*prev), (*top), (*next)) < 180.0)
                {
                    // end vertex
                    const_vertex_iterator peh = helper[edge_of[prev]];
                    const_vertex_iterator prev_peh = peh;
                    if (prev_peh == _vertices.begin())
                        prev_peh = _vertices.end();
                    --prev_peh;

                    const_vertex_iterator next_peh = peh;
                    if (next_peh == last)
                        next_peh = _vertices.begin();
                    else
                        ++next_peh;

                    if ((*prev_peh).y() > (*peh).y() &&
                        (*next_peh).y() > (*peh).y() &&
                        InteriorAngle((*prev_peh), (*peh), (*next_peh)) > 180.0)
                    {
                        // peh is merge vertex
                        edge_list.push_back(make_pair(*top, *helper[edge_of[prev]]));
                    }

                    search_tree.Delete(&edge_of[prev]);
                }
                else
                {
                    // merge vertex
                    const_vertex_iterator peh = helper[edge_of[prev]];
                    const_vertex_iterator prev_peh = peh;
                    if (prev_peh == _vertices.begin())
                        prev_peh = _vertices.end();
                    --prev_peh;

                    const_vertex_iterator next_peh = peh;
                    if (next_peh == last)
                        next_peh = _vertices.begin();
                    else
                        ++next_peh;

                    if ((*prev_peh).y() > (*peh).y() &&
                        (*next_peh).y() > (*peh).y() &&
                        InteriorAngle((*prev_peh), (*peh), (*next_peh)) > 180.0)
                    {
                        // peh is merge vertex
                        edge_list.push_back(make_pair(*top, *helper[edge_of[prev]]));
                    }

                    search_tree.Delete(&edge_of[prev]);

                    const_edge_iterator* left = search_tree.Search(&top);

                    const_vertex_iterator eh = helper[*left];
                    const_vertex_iterator prev_eh = eh;
                    if (prev_eh == _vertices.begin())
                        prev_eh = _vertices.end();
                    --prev_eh;

                    const_vertex_iterator next_eh = eh;
                    if (next_eh == last)
                        next_eh = _vertices.begin();
                    else
                        ++next_eh;

                    if ((*prev_eh).y() > (*eh).y() &&
                        (*next_eh).y() > (*eh).y() &&
                        InteriorAngle((*prev_eh), (*eh), (*next_eh)) > 180.0)
                    {
                        // merge vertex
                        edge_list.push_back(make_pair(*top, *helper[*left]));
                    }

                    helper[*left] = top;


                }
            }
            else
            {
                // regular vertex
                bool interior_right = false;
                for (const_vertex_iterator it = _vertices.begin(); it != _vertices.end();
                     ++it)
                {
                    if ((*it).x() > (*top).x())
                    {
                        interior_right = true;
                        break;
                    }
                }

                if (interior_right)
                {
                    const_vertex_iterator peh = helper[edge_of[prev]];
                    const_vertex_iterator prev_peh = peh;
                    if (prev_peh == _vertices.begin())
                        prev_peh = _vertices.end();
                    --prev_peh;

                    const_vertex_iterator next_peh = peh;
                    if (next_peh == last)
                        next_peh = _vertices.begin();
                    else
                        ++next_peh;

                    if ((*prev_peh).y() > (*peh).y() &&
                        (*next_peh).y() > (*peh).y() &&
                        InteriorAngle((*prev_peh), (*peh), (*next_peh)) > 180.0)
                    {
                        // peh is merge vertex
                        edge_list.push_back(make_pair(*top, *helper[edge_of[prev]]));
                    }

                    search_tree.Delete(&edge_of[prev]);

                    search_tree.Insert(&edge_of[top]);
                    helper[edge_of[prev]] = top;
                }
                else
                {
                    const_edge_iterator* left = search_tree.Search(&top);

                    const_vertex_iterator eh = helper[*left];
                    const_vertex_iterator prev_eh = eh;
                    if (prev_eh == _vertices.begin())
                        prev_eh = _vertices.end();
                    --prev_eh;

                    const_vertex_iterator next_eh = eh;
                    if (next_eh == last)
                        next_eh = _vertices.begin();
                    else
                        ++next_eh;

                    if ((*prev_eh).y() > (*eh).y() &&
                        (*next_eh).y() > (*eh).y() &&
                        InteriorAngle((*prev_eh), (*eh), (*next_eh)) > 180.0)
                    {
                        // merge vertex
                        edge_list.push_back(make_pair(*top, *helper[*left]));
                    }
                }


            }
        }
    }
}
