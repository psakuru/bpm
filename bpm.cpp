
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include "read_bipartite.hpp"
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "bipartite_matching.hpp"


#if 1 

int main()
{
  using namespace boost;

  typedef adjacency_list_traits < vecS, vecS, directedS > Traits;

  typedef adjacency_list < listS, vecS, directedS,
    no_property,
    property < edge_index_t, size_t > > Graph;

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

  std::vector<default_color_type> color(num_vertices(g));
  std::vector<Traits::edge_descriptor> pred(num_vertices(g));
/*  long flow = bipartite_matching_edmonds_karp
  	(g, s, t, capacity, residual_capacity, rev, &color[0], &pred[0]);*/

  property_map<Graph, vertex_index_t>::type indexMap = get(vertex_index, g);
  long flow = bipartite_matching_push_relabel
  	(g, s, t, capacity, residual_capacity, rev, indexMap);
  
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

#else //working interior property map

int main()
{
  using namespace boost;

  typedef adjacency_list_traits < vecS, vecS, directedS > Traits;

  typedef adjacency_list < listS, vecS, directedS,
    no_property,
    property < edge_capacity_t, long,
    property < edge_residual_capacity_t, long,
    property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;


  Graph g;

  property_map < Graph, edge_capacity_t >::type
    capacity = get(edge_capacity, g);
  property_map < Graph, edge_reverse_t >::type rev = get(edge_reverse, g);
  property_map < Graph, edge_residual_capacity_t >::type
    residual_capacity = get(edge_residual_capacity, g);

  Traits::vertex_descriptor s, t;
  read_dimacs_bipartite(g, capacity, rev, s, t);

  std::vector<default_color_type> color(num_vertices(g));
  std::vector<Traits::edge_descriptor> pred(num_vertices(g));
  long flow = bipartite_matching_edmonds_karp
  	(g, s, t, capacity, residual_capacity, rev, &color[0], &pred[0]);

/*  long flow = bipartite_matching_push_relabel
  	(g, s, t, capacity, residual_capacity, rev, &color[0], &pred[0]);*/
  
  std::cout << "c  The total flow:" << std::endl;
  std::cout << "s " << flow << std::endl << std::endl;

  std::cout << "c flow values:" << std::endl;
  graph_traits < Graph >::vertex_iterator u_iter, u_end;
  graph_traits < Graph >::out_edge_iterator ei, e_end;
  for (boost::tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter)
    for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei)
      if (capacity[*ei] > 0)
        std::cout << "f " << *u_iter << " " << target(*ei, g) << " "
          << (capacity[*ei] - residual_capacity[*ei]) << std::endl;

  return EXIT_SUCCESS;
}

#endif
