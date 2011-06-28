#include "vertex3.h"

using namespace cg;

Vertex3::Vertex3()
{
}

Vertex3::Vertex3(double x, double y, double z) :
    DCoordinate3(x, y, z)//,
    //_incident_edge(0)
{
}

//void Vertex3::SetIncidentEdge(HalfEdge *incident_edge)
//{
//    _incident_edge = incident_edge;
//}
