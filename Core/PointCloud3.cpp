#include "PointCloud3.h"
#include <fstream>
#include "TriangulatedMeshes3.h"
#include <algorithm>
#include "Core/EdgeList/halfedge.h"
#include "Core/Materials.h"

using namespace std;
using namespace cg;

PointCloud3::PointCloud3() : _convex_hull_done(false)
{
}

void PointCloud3::LoadFromOFF(const string &file_name)
{
    TriangulatedMesh3 mesh;
    mesh.LoadFromOFF(file_name, GL_TRUE);
    vector<DCoordinate3> v = mesh.GetVertices();

    for (vector<DCoordinate3>::const_iterator it = v.begin(); it != v.end();
         ++it)
    {
        const DCoordinate3 *p = &*it;
        _points.push_back(Vertex3(p->x(), p->y(), p->z()));

    }
}

void PointCloud3::LoadTest()
{
    _points.push_back(Vertex3(-1.3, -1.0, 4.3));
    _points.push_back(Vertex3(-4.3, -8.1, 2.1));
    _points.push_back(Vertex3(-1.0, 8.2, -6.6));
    _points.push_back(Vertex3(5, -3.9, -5.0));
    _points.push_back(Vertex3(2.5, -2.0, -5.4));
    _points.push_back(Vertex3(3.9, 7.7, 1.6));
    _points.push_back(Vertex3(5.3, 4.0, -1.4));
    _points.push_back(Vertex3(6.8, 5.6, -7.6));
    _points.push_back(Vertex3(-5.2, 2.9, 2.0));
    _points.push_back(Vertex3(-3.0, -5.9, 7.6));
    _points.push_back(Vertex3(2.4, 2.4, 4.0));
    _points.push_back(Vertex3(9.7, 2.5, 6.4));
    _points.push_back(Vertex3(4.7, 3.7, 5.0));
    _points.push_back(Vertex3(-2.7, 1.4, -6.7));
    _points.push_back(Vertex3(0.6, -7.0, -1.8));
    _points.push_back(Vertex3(-3.8, -9.0, 7.9));
    _points.push_back(Vertex3(3.7, 4.6, -8.0));
    _points.push_back(Vertex3(1.8, -6.8, 2.0));
    _points.push_back(Vertex3(-9.5, -9.2, 8.0));
    _points.push_back(Vertex3(-8.3, 3.1, 3.0));
    _points.push_back(Vertex3(-7.1, 4.2, 2.1));
    _points.push_back(Vertex3(1.8, 4.1, -4.6));
    _points.push_back(Vertex3(4.0, 4.9, -2.5));
    _points.push_back(Vertex3(6.0, -8.7, 3.2));
    _points.push_back(Vertex3(-8.6, -8.0, -6.4));
    _points.push_back(Vertex3(1.0, 4.3, -3.2));
    _points.push_back(Vertex3(2.7, -2.9, -2.9));
}



void PointCloud3::ConvexHull()
{
    // convex hull using randomized iterative algorithm

    // four initial points
    point_iterator it = _points.begin();

    Vertex3 *p1 = &*it; ++it;
    Vertex3 *p2 = &*it; ++it;
    Vertex3 *p3 = &*it; ++it;
    Vertex3 *p4 = &*it; ++it;


    _edges.Add(p1, p2, p3, p4);

    //_points.erase(_points.begin(), it);

    // randomize points
    //random_shuffle(it, _points.end());

    // initialize conflict graph
    _edges.InitializeConflictGraph(_points);

    for (point_iterator pit = it; pit != _points.end(); ++pit)
    {
        if (_edges.IsOutside(&*pit))
        {
            // Delete visibe facets, leave the horizon _edges and return them
            list<Facet*> old_facets;
            list<HalfEdge*> horizon_edges = _edges.DeleteVisibleFacets(&*pit, old_facets);

            list<Facet*>::iterator fit = old_facets.begin();

            // Construct a facet with the point and the horizon edge
            for (list<HalfEdge*>::iterator hit = horizon_edges.begin();
                 hit != horizon_edges.end(); ++hit)
            {
                _edges.CreateFacet(*hit, &*pit);

                // todo if coplanar

                // determine conflicts for the new facet
                _edges.SetConflicts(*fit,
                    (*hit)->GetTwin()->GetIncidentFacet(),
                    (*hit)->GetIncidentFacet());

                ++fit;

            }

            // Set up pointers of twin edges
            _edges.SetUpTwins(horizon_edges);

            _edges.RemoveFromConflicts(&*pit);
        }
    }

    _convex_hull_done = true;

}

void PointCloud3::SteppedConvexHull()
{

    if (!_convex_hull_done)
    {
        // convex hull using randomized iterative algorithm

        // four initial points
        point_iterator it = _points.begin();

        Vertex3 *p1 = &*it; ++it;
        Vertex3 *p2 = &*it; ++it;
        Vertex3 *p3 = &*it; ++it;
        Vertex3 *p4 = &*it; ++it;


        _edges.Add(p1, p2, p3, p4);

        //_points.erase(_points.begin(), it);

        // randomize points
        //random_shuffle(it, _points.end());

        // initialize conflict graph
        //_edges.InitializeConflictGraph(_points);

        _pit = _points.begin() + 4;
        _convex_hull_done = true;
    }
    else
    {
        if (_pit != _points.end())
        {
//            if (_edges.IsOutside(&*_pit))
//            {
//                // Delete visibe facets, leave the horizon _edges and return them
//                list<Facet*> old_facets;
//                _horizon_edges = _edges.DeleteVisibleFacets(&*_pit, old_facets);

//                list<Facet*>::iterator fit = old_facets.begin();

//                // Construct a facet with the point and the horizon edge
//                for (list<HalfEdge*>::iterator hit = _horizon_edges.begin();
//                     hit != _horizon_edges.end(); ++hit)
//                {
//                    _edges.CreateFacet(*hit, &*_pit);

//                    // todo if coplanar

//                    // determine conflicts for the new facet
//                    _edges.SetConflicts(*fit,
//                        (*hit)->GetTwin()->GetIncidentFacet(),
//                        (*hit)->GetIncidentFacet());

//                    ++fit;

//                }

//                // Set up pointers of twin edges
//                _edges.SetUpTwins(_horizon_edges);

//                _edges.RemoveFromConflicts(&*_pit);
                _visible_facets = _edges.VisibleFacets(&*_pit);
                _horizon_edges = _edges.HorizonEdges(_visible_facets);

                for (set<HalfEdge*>::iterator it = _horizon_edges.begin();
                     it != _horizon_edges.end(); ++it)
                {
                    _edges.CreateFacet(*it, &*_pit);
                }

            //}
            ++_pit;
        }
    }
}

void PointCloud3::Render()
{
    if (_convex_hull_done)
    {
        list<HalfEdge>& edges = _edges.GetHalfEdgeList();


        glBegin(GL_LINES);

            // convex hull


            for (list<HalfEdge>::iterator it = edges.begin(); it != edges.end(); ++it)
            {
                HalfEdge* he = &*it;
                if (_horizon_edges.count(he) != 0 || _horizon_edges.count(he->GetTwin()))
                {
                    glColor3f(0.0f, 0.0f, 1.0f);
                }
                else
                {
                    glColor3f(0.3f, 0.3f, 0.3f);
                }

                Vertex3* a = (*it).GetOrigin();
                Vertex3* b = (*it).GetDestination();

                glVertex3f(a->x(), a->y(), a->z());
                glVertex3f(b->x(), b->y(), b->z());
            }

            // current point


            glColor3f(0.0f, 0.0f, 1.0f);

            Vertex3 *a = &*_pit;
            GLfloat cross_size = 0.1f;

            glVertex3f(a->x() - cross_size, a->y(), a->z());
            glVertex3f(a->x() + cross_size, a->y(), a->z());

            glVertex3f(a->x(), a->y() - cross_size, a->z());
            glVertex3f(a->x(), a->y() + cross_size, a->z());

            glVertex3f(a->x(), a->y(), a->z() - cross_size);
            glVertex3f(a->x(), a->y(), a->z() + cross_size);
        glEnd();


//        // points in conflict with a any face

//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glBegin(GL_POINTS);


//            map<Vertex3*, set<Facet*> > &conflict_graph = _edges.GetConflictGraph();

//            glColor4f(1.0f, 0.0f, 0.0f, 0.3f);

//            for (map<Vertex3*, set<Facet*> >::iterator it = conflict_graph.begin();
//                 it != conflict_graph.end(); ++it)
//            {
//                Vertex3 *v = (*it).first;
//                glVertex3f(v->x(), v->y(), v->z());
//            }



//        glEnd();
//        glDisable(GL_BLEND);

//            glColor3f(0.5f, 0.5f, 0.0f);

//            for (list<HalfEdge*>::iterator theit = _horizon_edges.begin(); theit != _horizon_edges.end(); ++theit)
//            {
//                Vertex3 *a = (*theit)->GetOrigin();
//                Vertex3 *b = (*theit)->GetDestination();
//                glVertex3f(a->x(), a->y(), a->z());
//                glVertex3f(b->x(), b->y(), b->z());
//            }

        // visible facets
        glBegin(GL_TRIANGLES);
//        for (set<Facet*>::iterator it = _visible_facets.begin();
//             it != _visible_facets.end(); ++it)
//        {

//                glColor3f(1.0f, 0.0f, 0.0f);
//                HalfEdge *outer = (*it)->GetOuterComponent();
//                Vertex3 *v1 = outer->GetOrigin();
//                Vertex3 *v2 = outer->GetNext()->GetOrigin();
//                Vertex3 *v3 = outer->GetNext()->GetNext()->GetOrigin();

//                glVertex3f(v1->x(), v1->y(), v1->z());
//                glVertex3f(v2->x(), v2->y(), v2->z());
//                glVertex3f(v3->x(), v3->y(), v3->z());


//        }

        list<Facet> &facets = _edges.GetFacets();
        for (list<Facet>::iterator it = facets.begin(); it != facets.end();
             ++it)
        {
            if (_visible_facets.count(&*it) == 0)
            {
                glColor3f(0.7f, 0.7f, 0.7f);
            }
            else
            {
                glColor3f(1.0f, 0.0f, 0.0f);
            }
            HalfEdge *outer = (*it).GetOuterComponent();
            Vertex3 *v1 = outer->GetOrigin();
            Vertex3 *v2 = outer->GetNext()->GetOrigin();
            Vertex3 *v3 = outer->GetNext()->GetNext()->GetOrigin();

            glVertex3f(v1->x(), v1->y(), v1->z());
            glVertex3f(v2->x(), v2->y(), v2->z());
            glVertex3f(v3->x(), v3->y(), v3->z());
        }
        glEnd();




    }
    else
    {
        glBegin(GL_POINTS);

            glColor3f(0.0f, 1.0f, 0.0f);

            for (point_iterator it = _points.begin(); it != _points.end(); ++it)
                glVertex3f((*it).x(), (*it).y(), (*it).z());

        glEnd();
    }
}
