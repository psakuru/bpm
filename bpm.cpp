
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include "read_bipartite.hpp"
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "bipartite_matching.hpp"
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/bipartite.hpp>

using namespace boost;

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

int main()
{

  typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;

  typedef adjacency_list < listS, vecS, undirectedS,
    no_property,
    property < edge_index_t, size_t > > Graph;

  typedef graph_traits<Graph>::edge_descriptor edge_descriptor;

  Graph g;

  unsigned int *pCapacity, *pResidualCapacity; //send pointers to read() which dynamically allocates memory
  edge_descriptor *pReverse;

  Traits::vertex_descriptor s, t;
  typedef property_map<Graph, vertex_index_t>::type IndexMap;
  typedef std::vector<default_color_type> PartitionMap;
  PartitionMap *partitionMap; 

  read_dimacs_bipartite(g, &pCapacity, &pReverse, &pResidualCapacity, &partitionMap, s, t); //passing residual capacity for allocation

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


 //create a filtered graph
  typedef iterator_property_map<PartitionMap::iterator, IndexMap> IteratorPartitionMap;
  IndexMap indexMap = get(vertex_index, g);
  IteratorPartitionMap iteratorPartitionMap(partitionMap->begin(), indexMap);

  typedef make_directed<Graph, IteratorPartitionMap/*clown PartitionMap*/, iterator_property_map<unsigned int*, EdgeID_Map>, Traits::vertex_descriptor> make_directed_t;
    
  make_directed_t md(g, iteratorPartitionMap/*clown *partition_map*/, capacity, s, t);
  filtered_graph<Graph, make_directed_t> fg(g, md);
  std::vector<default_color_type> color(num_vertices(g));
  std::vector<Traits::edge_descriptor> pred(num_vertices(g));

  long flow = bipartite_matching_edmonds_karp
  	(fg, s, t, capacity, residual_capacity, rev, &color[0], &pred[0]);

#if 0 

  long flow = bipartite_matching_push_relabel
  	(fg, s, t, capacity, residual_capacity, rev, index_map);

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
  delete partitionMap;
  delete[] pCapacity;
  delete[] pResidualCapacity;
  delete[] pReverse;

  return EXIT_SUCCESS;
}
