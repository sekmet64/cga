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
        _edges.InitializeConflictGraph(_points);

        _pit = _points.begin() + 4;
        _convex_hull_done = true;
    }
    else
    {
        if (_pit != _points.end())
        {

            if (_edges.IsOutside(&*_pit))
            {
                // Delete visibe facets, leave the horizon _edges and return them
                list<Facet*> old_facets;
                _horizon_edges = _edges.DeleteVisibleFacets(&*_pit, old_facets);

                list<Facet*>::iterator fit = old_facets.begin();

                // Construct a facet with the point and the horizon edge
                for (list<HalfEdge*>::iterator hit = _horizon_edges.begin();
                     hit != _horizon_edges.end(); ++hit)
                {
                    _edges.CreateFacet(*hit, &*_pit);

                    // todo if coplanar

                    // determine conflicts for the new facet
                    _edges.SetConflicts(*fit,
                        (*hit)->GetTwin()->GetIncidentFacet(),
                        (*hit)->GetIncidentFacet());

                    ++fit;

                }

                // Set up pointers of twin edges
                _edges.SetUpTwins(_horizon_edges);

                _edges.RemoveFromConflicts(&*_pit);
            }
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
            glColor3f(0.0f, 1.0f, 0.0f);

            for (list<HalfEdge>::iterator it = edges.begin(); it != edges.end(); ++it)
            {
                Vertex3* a = (*it).GetOrigin();
                Vertex3* b = (*it).GetDestination();

                glVertex3f(a->x(), a->y(), a->z());
                glVertex3f(b->x(), b->y(), b->z());
            }

            // current point

            glColor3f(0.0f, 0.0f, 1.0f);

            Vertex3 *a = &*_pit;
            GLfloat cross_size = 0.01f;

            glVertex3f(a->x() - cross_size, a->y(), a->z());
            glVertex3f(a->x() + cross_size, a->y(), a->z());

            glVertex3f(a->x(), a->y() - cross_size, a->z());
            glVertex3f(a->x(), a->y() + cross_size, a->z());

            glVertex3f(a->x(), a->y(), a->z() - cross_size);
            glVertex3f(a->x(), a->y(), a->z() + cross_size);
        glEnd();


        // points in conflict with a any face

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_POINTS);


            map<Vertex3*, set<Facet*> > &conflict_graph = _edges.GetConflictGraph();

            glColor4f(1.0f, 0.0f, 0.0f, 0.3f);

            for (map<Vertex3*, set<Facet*> >::iterator it = conflict_graph.begin();
                 it != conflict_graph.end(); ++it)
            {
                Vertex3 *v = (*it).first;
                glVertex3f(v->x(), v->y(), v->z());
            }



        glEnd();
        glDisable(GL_BLEND);

//            glColor3f(0.5f, 0.5f, 0.0f);

//            for (list<HalfEdge*>::iterator theit = _horizon_edges.begin(); theit != _horizon_edges.end(); ++theit)
//            {
//                Vertex3 *a = (*theit)->GetOrigin();
//                Vertex3 *b = (*theit)->GetDestination();
//                glVertex3f(a->x(), a->y(), a->z());
//                glVertex3f(b->x(), b->y(), b->z());
//            }


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
