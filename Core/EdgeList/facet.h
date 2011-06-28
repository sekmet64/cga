#pragma once
#include "halfedge.h"

namespace cg
{
    class HalfEdge;

    class Facet
    {
    public:
        Facet();
        Facet(HalfEdge *outer_component);
        ~Facet();

        HalfEdge* GetOuterComponent();

    private:
        HalfEdge *_outer_component;
    };
}
