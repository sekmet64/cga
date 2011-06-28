#include "dcedgelist.h"
#include "Core/PointCloud3.h"

using namespace cg;
using namespace std;

DCEdgeList::DCEdgeList()
{
}

void DCEdgeList::Add(Vertex3 *p1, Vertex3 *p2, Vertex3 *p3, Vertex3 *p4)
{
    _vertices.push_back(p1);
    _vertices.push_back(p2);
    _vertices.push_back(p3);
    _vertices.push_back(p4);

    _half_edges.push_back(HalfEdge(p1));
    _half_edges.push_back(HalfEdge(p2));
    _half_edges.push_back(HalfEdge(p3));

    _half_edges.push_back(HalfEdge(p2));
    _half_edges.push_back(HalfEdge(p4));
    _half_edges.push_back(HalfEdge(p3));

    _half_edges.push_back(HalfEdge(p2));
    _half_edges.push_back(HalfEdge(p1));
    _half_edges.push_back(HalfEdge(p4));

    _half_edges.push_back(HalfEdge(p1));
    _half_edges.push_back(HalfEdge(p3));
    _half_edges.push_back(HalfEdge(p4));

    half_edge_iterator heit = _half_edges.begin();

    HalfEdge *he12 = &*heit; ++heit;
    HalfEdge *he23 = &*heit; ++heit;
    HalfEdge *he31 = &*heit; ++heit;

    HalfEdge *he24 = &*heit; ++heit;
    HalfEdge *he43 = &*heit; ++heit;
    HalfEdge *he32 = &*heit; ++heit;

    HalfEdge *he21 = &*heit; ++heit;
    HalfEdge *he14 = &*heit; ++heit;
    HalfEdge *he42 = &*heit; ++heit;

    HalfEdge *he13 = &*heit; ++heit;
    HalfEdge *he34 = &*heit; ++heit;
    HalfEdge *he41 = &*heit;

    he12->SetPrev(he31);
    he23->SetPrev(he12);
    he31->SetPrev(he23);
    he12->SetNext(he23);
    he23->SetNext(he31);
    he31->SetNext(he12);

    he24->SetPrev(he32);
    he43->SetPrev(he24);
    he32->SetPrev(he43);
    he24->SetNext(he43);
    he43->SetNext(he32);
    he32->SetNext(he24);

    he21->SetPrev(he42);
    he14->SetPrev(he21);
    he42->SetPrev(he14);
    he21->SetNext(he14);
    he14->SetNext(he42);
    he42->SetNext(he21);

    he13->SetPrev(he41);
    he34->SetPrev(he13);
    he41->SetPrev(he34);
    he13->SetNext(he34);
    he34->SetNext(he41);
    he41->SetNext(he13);

    _facets.push_back(Facet(he12));
    _facets.push_back(Facet(he24));
    _facets.push_back(Facet(he21));
    _facets.push_back(Facet(he13));

    facet_iterator fit = _facets.begin();
    Facet *f123 = &*fit; ++fit;
    Facet *f243 = &*fit; ++fit;
    Facet *f214 = &*fit; ++fit;
    Facet *f134 = &*fit;

    he12->SetIncidentFacet(f123);
    he23->SetIncidentFacet(f123);
    he31->SetIncidentFacet(f123);

    he24->SetIncidentFacet(f243);
    he43->SetIncidentFacet(f243);
    he32->SetIncidentFacet(f243);

    he21->SetIncidentFacet(f214);
    he14->SetIncidentFacet(f214);
    he42->SetIncidentFacet(f214);

    he13->SetIncidentFacet(f134);
    he34->SetIncidentFacet(f134);
    he41->SetIncidentFacet(f134);

//    p1->SetIncidentEdge(he12);
//    p2->SetIncidentEdge(he23);
//    p3->SetIncidentEdge(he31);
//    p4->SetIncidentEdge(he43);

    he12->SetTwin(he21);
    he13->SetTwin(he31);
    he14->SetTwin(he41);

    he21->SetTwin(he12);
    he23->SetTwin(he32);
    he24->SetTwin(he42);

    he31->SetTwin(he13);
    he32->SetTwin(he23);
    he34->SetTwin(he43);

    he41->SetTwin(he14);
    he42->SetTwin(he24);
    he43->SetTwin(he34);
}

bool CanSee(Vertex3 *v, Facet *f)
{
    HalfEdge *he1 = f->GetOuterComponent();
    HalfEdge *he2 = he1->GetNext();
    HalfEdge *he3 = he2->GetNext();

    Vertex3 *v1 = he1->GetOrigin();
    Vertex3 *v2 = he2->GetOrigin();
    Vertex3 *v3 = he3->GetOrigin();

    double A = v1->y() * (v2->z() - v3->z()) + v2->y() * (v3->z() - v1->z()) +
        v3->y() * (v1->z() - v2->z());
    double B = v1->z() * (v2->x() - v3->x()) + v2->z() * (v3->x() - v1->x()) +
        v3->z() * (v1->x() - v2->x());
    double C = v1->x() * (v2->y() - v3->y()) + v2->x() * (v3->y() - v1->y()) +
        v3->x() * (v1->y() - v2->y());
    double D = -(v1->x() * (v2->y() * v3->z() - v3->y() * v2->z()) +
        v2->x() * (v3->y() * v1->z() - v1->y() * v3->z()) +
        v3->x() * (v1->y() * v2->z() - v2->y() * v1->z()));

    double s = A * v->x() + B * v->y() + C * v->z() + D;

    return s > 0;
}

void DCEdgeList::InitializeConflictGraph(vector<Vertex3> &points)
{

    for (PointCloud3::point_iterator pit = points.begin(); pit != points.end(); ++pit)
    {
        for (facet_iterator fit = _facets.begin(); fit != _facets.end(); ++fit)
        {
            if (CanSee(&*pit, &*fit))
                _conflict_graph[&*pit].insert(&*fit);
        }
    }
}

map<Vertex3*, set<Facet*> >& DCEdgeList::GetConflictGraph()
{
    return _conflict_graph;
}

bool DCEdgeList::IsOutside(Vertex3 *v)
{
    return !_conflict_graph[v].empty();
}

list<HalfEdge*> DCEdgeList::DeleteVisibleFacets(Vertex3 *v, list<Facet*> &old_facets)
{
    list<HalfEdge*> horizon_edges;

    set<Facet*>& facets_to_delete = _conflict_graph[v];

    facet_iterator fit = _facets.begin();
    while (fit != _facets.end())
    {
        if (facets_to_delete.count(&*fit))
        {
            list<HalfEdge*> edges = DeleteCycle((*fit).GetOuterComponent(), v, old_facets);
            horizon_edges.splice(horizon_edges.end(), edges);


            fit = _facets.erase(fit);
        }
        else
        {
            ++fit;
        }
    }

    return horizon_edges;
}

list<HalfEdge*> DCEdgeList::DeleteCycle(HalfEdge *he, Vertex3 *v, list<Facet*> &old_facets)
{
    set<HalfEdge*> hes_to_delete;

    list<HalfEdge*> horizon_edges;

    HalfEdge *edge = he;
    while (edge->GetNext() != he)
    {
        if (_conflict_graph[v].count(he->GetTwin()->GetIncidentFacet()))
        {
            hes_to_delete.insert(edge);
        }
        else
        {
            horizon_edges.push_back(edge);
            old_facets.push_back(edge->GetIncidentFacet());
        }

        edge = edge->GetNext();
    }
    if (_conflict_graph[v].count(he->GetTwin()->GetIncidentFacet()))
    {
        hes_to_delete.insert(edge);
    }
    else
    {
        horizon_edges.push_back(edge);
        old_facets.push_back(edge->GetIncidentFacet());
    }

    half_edge_iterator heit = _half_edges.begin();

    while (heit != _half_edges.end())
    {
        if (hes_to_delete.count(&*heit))
        {
            heit = _half_edges.erase(heit);
        }
        else
        {
            ++heit;
        }
    }

    return horizon_edges;
}

void DCEdgeList::CreateFacet(HalfEdge *he, Vertex3 *v)
{
    //Vertex3 *v1 = he->GetOrigin();
    Vertex3 *v2 = he->GetDestination();
    Vertex3 *v3 = v;

    _facets.push_back(Facet(he));
    Facet *f123 = &_facets.back();

    _half_edges.push_back(HalfEdge(v2));
    _half_edges.push_back(HalfEdge(v3));

    half_edge_iterator it = _half_edges.end();
    --it;
    HalfEdge *he31 = &*it; --it;
    HalfEdge *he23 = &*it;
    HalfEdge *he12 = he;

    he12->SetNext(he23);
    he23->SetPrev(he12);

    he23->SetNext(he31);
    he31->SetPrev(he23);

    he31->SetNext(he12);
    he12->SetPrev(he31);

    he12->SetIncidentFacet(f123);
    he23->SetIncidentFacet(f123);
    he31->SetIncidentFacet(f123);
}

void DCEdgeList::SetUpTwins(list<HalfEdge*> &horizon_edges)
{
    list<HalfEdge*>::iterator it1 = horizon_edges.begin();
    list<HalfEdge*>::iterator it2 = it1; ++it2;

    while (it2 != horizon_edges.end())
    {
        HalfEdge* he1 = (*it1)->GetNext();
        HalfEdge* he2 = (*it2)->GetPrev();

        he1->SetTwin(he2);
        he2->SetTwin(he1);

        ++it1; ++it2;
    }

    // last item
    HalfEdge* he1 = (*it1)->GetNext();
    HalfEdge* he2 = (*horizon_edges.begin())->GetPrev();

    he1->SetTwin(he2);
    he2->SetTwin(he1);
}

void DCEdgeList::SetConflicts(Facet *f1, Facet *f2, Facet *f)
{
    list<Vertex3*> points;

    for (map<Vertex3*, set<Facet*> >::iterator it = _conflict_graph.begin();
         it != _conflict_graph.end(); ++it)
    {
        if ((*it).second.count(f1) || (*it).second.count(f2))
            points.push_back((*it).first);
    }

    for (list<Vertex3*>::iterator it = points.begin(); it != points.end();
         ++it)
    {
        if (CanSee((*it), f))
        {
            _conflict_graph[(*it)].insert(f);
        }
    }
}

void DCEdgeList::RemoveFromConflicts(Vertex3 *v)
{
    _conflict_graph.erase(v);
}

list<HalfEdge>& DCEdgeList::GetHalfEdgeList()
{
    return _half_edges;
}


set<Facet*> DCEdgeList::VisibleFacets(Vertex3 *p)
{
    set<Facet*> visible_facets;
    for (facet_iterator fit = _facets.begin(); fit != _facets.end(); ++fit)
    {
        if (CanSee(p, &*fit))
        {
            visible_facets.insert(&*fit);
        }
    }
    return visible_facets;
}

set<HalfEdge*> DCEdgeList::HorizonEdges(set<Facet*> &visible_facets)
{
    set<HalfEdge*> horizon_edges;
    for (set<Facet*>::iterator it = visible_facets.begin();
         it != visible_facets.end(); ++it)
    {
        HalfEdge *he1 = (*it)->GetOuterComponent();
        HalfEdge *he2 = he1->GetNext();
        HalfEdge *he3 = he1->GetNext()->GetNext();

        if (visible_facets.count(he1->GetTwin()->GetIncidentFacet()) == 0)
        {
            horizon_edges.insert(he1);
        }
        if (visible_facets.count(he2->GetTwin()->GetIncidentFacet()) == 0)
        {
            horizon_edges.insert(he2);
        }
        if (visible_facets.count(he3->GetTwin()->GetIncidentFacet()) == 0)
        {
            horizon_edges.insert(he3);
        }
    }
    return horizon_edges;
}

std::list<Facet>& DCEdgeList::GetFacets()
{
    return _facets;
}
