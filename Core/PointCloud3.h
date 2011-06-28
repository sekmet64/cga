#pragma once
#include <vector>
#include "DCoordinates3.h"
#include <string>

#include "EdgeList/vertex3.h"
#include "Core/EdgeList/dcedgelist.h"

namespace cg
{
    class PointCloud3
    {
    public:

        PointCloud3();

        void LoadFromOFF(const std::string &file_name);

        void ConvexHull();

        void SteppedConvexHull();

        typedef std::vector<Vertex3>::iterator point_iterator;

        void Render();

    private:
        std::vector<Vertex3> _points;

        DCEdgeList _edges;

        bool _convex_hull_done;

        point_iterator _pit;
        std::list<HalfEdge*> _horizon_edges;
    };
}
