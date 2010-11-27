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

} //namespace boost

#endif //BIPARTITE_MATCHING_HPP
