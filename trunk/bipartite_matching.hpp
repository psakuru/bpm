#ifndef BIPARTITE_MATCHING_HPP
#define BIPARTITE_MATCHING_HPP

#include <boost/config.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

namespace boost {

template <class Graph, 
		class CapacityEdgeMap, class ResidualCapacityEdgeMap,
		class ReverseEdgeMap, class ColorMap, class PredEdgeMap>
typename property_traits<CapacityEdgeMap>::value_type
bipartite_matching_edmonds_karp
  (Graph& g, 
   typename graph_traits<Graph>::vertex_descriptor src,
   typename graph_traits<Graph>::vertex_descriptor sink,
   CapacityEdgeMap cap, 
   ResidualCapacityEdgeMap res,
   ReverseEdgeMap rev, 
   ColorMap color, 
   PredEdgeMap pred)
{
	return edmonds_karp_max_flow(g, src, sink, cap, res, rev, color, pred);
}

template <class Graph, 
		class CapacityEdgeMap, class ResidualCapacityEdgeMap,
		class ReverseEdgeMap, class VertexIndexMap>
typename property_traits<CapacityEdgeMap>::value_type
bipartite_matching_push_relabel
  (Graph& g, 
   typename graph_traits<Graph>::vertex_descriptor src,
   typename graph_traits<Graph>::vertex_descriptor sink,
   CapacityEdgeMap cap, 
   ResidualCapacityEdgeMap res,
   ReverseEdgeMap rev, 
   VertexIndexMap indexMap) 
{
	return push_relabel_max_flow(g, src, sink, cap, res, rev, indexMap);
}


template<typename Graph, typename PartitionMap, typename CapacityMap, typename Vertex>
struct make_directed
{
	make_directed() {}

	make_directed(Graph _g, PartitionMap _partition_map, CapacityMap _capacity_map, Vertex _src, Vertex _sink):
						g(_g), partition_map(_partition_map), capacity_map(_capacity_map), src(_src), sink(_sink) {}

	template<typename EdgeDescriptor>
	bool operator()(const EdgeDescriptor& edge) const {

		if(source(edge,g) == src && get(capacity_map, edge) == 1) //src
			return true;

		if(source(edge,g) == sink && get(capacity_map, edge) == 0) //sink
			return true;

		if(get(partition_map, source(edge, g)) == color_traits<default_color_type>::white()) { //white
			if(target(edge,g) == src) {
				if(get(capacity_map, edge) == 0)
					return true;
			}
			else {
				if(get(capacity_map, edge)==1)
					return true;
			}
		}
		else { //black
			if(target(edge,g) == sink) {
				if(get(capacity_map, edge) == 1)
					return true;
			}
			else {
				if(get(capacity_map, edge) == 0)
					return true;
			}
		}

		return false;
	}

	PartitionMap partition_map;
	CapacityMap capacity_map;
	Vertex src;
	Vertex sink;
	Graph g;
};

  //TODO: add MatchingEdgeMap
  template <class Graph>
  unsigned int
  bipartite_matching_edmonds_karp
    (Graph& g)
  {
	  return 0;
  }


} //namespace boost

#endif //BIPARTITE_MATCHING_HPP
