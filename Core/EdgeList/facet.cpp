#include "facet.h"

using namespace cg;

Facet::Facet()
{
}

Facet::Facet(HalfEdge *outer_component) :
    _outer_component(outer_component)
{
}

Facet::~Facet()
{
}

HalfEdge* Facet::GetOuterComponent()
{
    return _outer_component;
}
