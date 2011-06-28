#pragma once
#include "facet.h"
#include "vertex3.h"

namespace cg
{
    class Facet;
    class Vertex3;

    class HalfEdge
    {
    public:
        HalfEdge();
        HalfEdge(Vertex3 *origin);

        void SetPrev(HalfEdge *prev);
        void SetNext(HalfEdge *next);
        void SetIncidentFacet(Facet *incident_facet);

        HalfEdge* GetPrev();
        HalfEdge* GetNext();
        Vertex3* GetOrigin();
        Vertex3* GetDestination();
        HalfEdge* GetTwin();
        Facet* GetIncidentFacet();

        void SetTwin(HalfEdge *twin);

    private:
        HalfEdge *_prev;
        HalfEdge *_next;

        Facet *_incident_facet;

        Vertex3 *_origin;

        HalfEdge *_twin;
    };
}


