#pragma once
#include <vector>
#include "vertex3.h"
#include "facet.h"
#include "halfedge.h"
#include <list>
#include <set>
#include <map>

namespace cg
{
    class DCEdgeList
    {
    public:
        DCEdgeList();

        void Add(Vertex3 *p1, Vertex3 *p2, Vertex3 *p3, Vertex3 *p4);
        void InitializeConflictGraph(std::vector<Vertex3> &points);

        std::map<Vertex3*, std::set<Facet*> >& GetConflictGraph();

        bool IsOutside(Vertex3 *v);

        std::list<HalfEdge*> DeleteVisibleFacets(Vertex3 *v, std::list<Facet*> &old_facets);

        std::list<HalfEdge*> DeleteCycle(HalfEdge *he, Vertex3 *v, std::list<Facet*> &old_facets);

        void CreateFacet(HalfEdge *he, Vertex3 *v);

        void SetUpTwins(std::list<HalfEdge*>& horizon_edges);

        void SetConflicts(Facet *f1, Facet *f2, Facet *f);

        void RemoveFromConflicts(Vertex3 *v);

        std::list<HalfEdge>& GetHalfEdgeList();

    private:
        std::vector<Vertex3*> _vertices;
        std::list<Facet> _facets;
        std::list<HalfEdge> _half_edges;

        std::map<Vertex3*, std::set<Facet*> > _conflict_graph;

        typedef std::vector<Vertex3*>::iterator vertex_iterator;
        typedef std::vector<Vertex3*>::const_iterator const_vertex_iterator;
        typedef std::list<HalfEdge>::iterator half_edge_iterator;
        typedef std::list<Facet>::iterator facet_iterator;
    };
}
