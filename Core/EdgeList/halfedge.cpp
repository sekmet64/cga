#include "halfedge.h"

using namespace cg;

HalfEdge::HalfEdge()
{
}

HalfEdge::HalfEdge(Vertex3 *origin) :
    _prev(0),
    _next(0),
    _incident_facet(0),
    _origin(origin)
{
}

void HalfEdge::SetPrev(HalfEdge *prev)
{
    _prev = prev;
}

void HalfEdge::SetNext(HalfEdge *next)
{
    _next = next;
}

void HalfEdge::SetIncidentFacet(Facet *incident_facet)
{
    _incident_facet = incident_facet;
}

HalfEdge* HalfEdge::GetPrev()
{
    return _prev;
}

HalfEdge* HalfEdge::GetNext()
{
    return _next;
}

Vertex3* HalfEdge::GetOrigin()
{
    return _origin;
}

void HalfEdge::SetTwin(HalfEdge *twin)
{
    _twin = twin;
}

HalfEdge* HalfEdge::GetTwin()
{
    return _twin;
}

Facet* HalfEdge::GetIncidentFacet()
{
    return _incident_facet;
}

Vertex3* HalfEdge::GetDestination()
{
    return _twin->GetOrigin();
}
