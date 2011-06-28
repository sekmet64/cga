#pragma once
#include "Core/DCoordinates3.h"
#include "halfedge.h"

namespace cg
{
    class HalfEdge;

    class Vertex3 : public DCoordinate3
    {
    public:
        Vertex3(double x, double y, double z);
        Vertex3();

        //void SetIncidentEdge(HalfEdge *incident_edge);

    private:
        //HalfEdge *_incident_edge;
    };
}
