#ifndef DYNDIS_MAX_FLOW_HPP
#define DYNDIS_MAX_FLOW_HPP

#include <iostream>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

#define DENSITY_THRESHOLD 0.1

using namespace boost;

template <class Graph>
typename property_traits<
  typename property_map<Graph, edge_capacity_t>::const_type
>::value_type
dyndis_max_flow
  (Graph& g,
   typename graph_traits<Graph>::vertex_descriptor src,
   typename graph_traits<Graph>::vertex_descriptor sink)
{
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef typename graph_traits<Graph>::out_edge_iterator out_edge_iterator;
	typedef typename graph_traits<Graph>::edge_descriptor edge_descriptor;

	typename graph_traits<Graph>::vertices_size_type numNodes = num_vertices(g);
	
	typename graph_traits<Graph>::degree_size_type numEdges = 0;

	vertex_iterator vi, v_end;
	for(tie(vi,v_end) = vertices(g); vi != v_end; ++vi) {
		numEdges += out_degree(*vi, g);
	}

	numEdges /= 2; //this is done because the read_dimacs component added reverse edges to the graph

	double density = (double)numEdges / (numNodes * numNodes);
	
	std::cout << "numNodes = " << numNodes << "\tnumEdges = " << numEdges << "\tdensity = " << density << std::endl;

	if(density > DENSITY_THRESHOLD) {
		std::cout << "High density graph... using PR" << std::endl;
		return push_relabel_max_flow(g, src, sink);
	}
	else {
		std::cout << "Low density graph... using EK" << std::endl;
		return edmonds_karp_max_flow(g, src, sink);
	}
}

#endif //DYNDIS_MAX_FOW_HPP
