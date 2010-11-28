
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include "read_bipartite.hpp"
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "bipartite_matching.hpp"
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/one_bit_color_map.hpp>
#include <boost/graph/bipartite.hpp>

using namespace boost;

template<typename Graph, typename PartitionMap>
struct make_directed
{
	make_directed() {}

	make_directed(Graph& _g, PartitionMap& _partition_map):g(_g), partition_map(_partition_map) {}

	template<typename EdgeDescriptor>
	bool operator()(const EdgeDescriptor& edge) const {
		if(get(partition_map, source(edge, g)) == color_traits<default_color_type>::white())
			return true;

		return false;
	}

	PartitionMap& partition_map;
	Graph& g;
};

int main()
{
#if 1
  typedef adjacency_list_traits < vecS, vecS, directedS > Traits;

  typedef adjacency_list < listS, vecS, directedS,
    no_property,
    property < edge_index_t, size_t > > Graph;
#else
  typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;

  typedef adjacency_list < listS, vecS, undirectedS,
    no_property,
    property < edge_index_t, size_t > > Graph;
#endif

  typedef graph_traits<Graph>::edge_descriptor edge_descriptor;


  Graph g;

  unsigned int *pCapacity, *pResidualCapacity; //send pointers to read() which dynamically allocates memory
  edge_descriptor *pReverse;

  Traits::vertex_descriptor s, t;
  read_dimacs_bipartite(g, &pCapacity, &pReverse, &pResidualCapacity, s, t); //passing residual capacity for allocation

  //create maps out of arrays
  typedef property_map<Graph, edge_index_t>::type EdgeID_Map;
  EdgeID_Map edge_id = get(edge_index, g);

  iterator_property_map
    <unsigned int*, EdgeID_Map>
	  capacity(pCapacity, edge_id);

  iterator_property_map
    <unsigned int*, EdgeID_Map>
	  residual_capacity(pResidualCapacity, edge_id);

  iterator_property_map
    <edge_descriptor*, EdgeID_Map>
	  rev(pReverse, edge_id);


#if 0  //create a filtered graph
  typedef property_map<Graph, vertex_index_t>::type IndexMap;
  typedef one_bit_color_map<IndexMap> PartitionMap;

  IndexMap index_map = get(vertex_index, g);
  PartitionMap partition_map(num_vertices(g), index_map);
  is_bipartite(g, index_map, partition_map);

  make_directed<Graph, PartitionMap> md(g, partition_map);
  filtered_graph<Graph, make_directed> fg(g, md);

  long flow = bipartite_matching_edmonds_karp
  	(fg, s, t, capacity, residual_capacity, rev, &color[0], &pred[0]);
#endif

#if 1

  std::vector<default_color_type> color(num_vertices(g));
  std::vector<Traits::edge_descriptor> pred(num_vertices(g));
  long flow = bipartite_matching_edmonds_karp
  	(g, s, t, capacity, residual_capacity, rev, &color[0], &pred[0]);

#else

  long flow = bipartite_matching_push_relabel
  	(g, s, t, capacity, residual_capacity, rev, index_map);

#endif 

  std::cout << "Matching Number:" << std::endl;
  std::cout << flow << std::endl << std::endl;

  std::cout << "Matching:" << std::endl;
  graph_traits < Graph >::vertex_iterator u_iter, u_end;
  graph_traits < Graph >::out_edge_iterator ei, e_end;
  for (boost::tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter)
    for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei) {
      if ((capacity[*ei] - residual_capacity[*ei] == 1) && (*u_iter != s) && (target(*ei, g) != t))
        std::cout << *u_iter << " " << target(*ei, g) << std::endl;
	}


  //now clean up
  delete[] pCapacity;
  delete[] pResidualCapacity;
  delete[] pReverse;

  return EXIT_SUCCESS;
}
