
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "bipartite_matching.hpp"
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/bipartite.hpp>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <boost/graph/graph_traits.hpp>


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

  typedef property_map<Graph, vertex_index_t>::type IndexMap;
  typedef property_map<Graph, edge_index_t>::type EdgeMap;
  typedef std::vector<default_color_type> PartitionMap;
  PartitionMap *partitionMap; 

  typedef graph_traits<Graph>::vertices_size_type vertices_size_type;
  typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  typedef graph_traits<Graph>::edge_descriptor edge_descriptor;

  std::vector<vertex_descriptor> verts; //stores vertex descriptors for each added node

  vertex_descriptor src, sink;

  //arrays to fill in after memory allocation
  unsigned int* pCapacity;
  unsigned int* pResidualCapacity;
  edge_descriptor* pReverse;

  //giving edge ids to real and fake edges and their reverses
  //all reverse edges get original_edge_id + 1
  //so all original edges are even, reverse edges are odd
  unsigned int realEdgeID; 

  long m, n,                    /*  number of edges and nodes */
    i, head, tail;

  std::string in_line;          /* for reading input line */
  char pr_type[4];              /* for reading type of the problem */

  std::istream& in = std::cin;

  while (std::getline(in, in_line)) {

    switch (in_line[0]) {
    case 'c':                  /* skip lines with comments */
    case '\n':                 /* skip empty lines   */
    case '\0':                 /* skip empty lines at the end of file */
      break;

    case 'p':                  /* problem description      */
	{
      std::sscanf ( in_line.c_str(), "%*c %3s %ld %ld", pr_type, &n, &m );
      {
        for (long vi = 0; vi < n; ++vi)
          verts.push_back(add_vertex(g));
      }

      // add fake source and target 
	  std::cout << "Received vertex size: " << n << std::endl;
	  n += 2;
      verts.push_back(add_vertex(g)); //source at index n-2
	  verts.push_back(add_vertex(g)); //target at index n-1

	  // initialize src and sink
	  src = verts[n-2];
	  sink = verts[n-1];

	  std::cout << "src: " << src << "\t" << "sink: " << sink << std::endl;

	  //create dynamic memory for capacity and reverse
	  //n-2 fake edges(one for each of the n-2 real nodes) and m bipartite edges
	  //double that for reverses
	  pCapacity = new unsigned int[2*(n-2 + m)]; 
	  pResidualCapacity = new unsigned int[2*(n-2 + m)];
	  pReverse = new edge_descriptor[2*(n-2 + m)];

	  //now initialize the edge IDs
	  realEdgeID = 0;
	}
      break;

    case 'a':                    /* arc description */

          std::sscanf ( in_line.c_str(),"%*c %ld %ld",
                        &tail, &head);

      --tail; // index from 0, not 1
      --head;

      {
        edge_descriptor e1, e2;
        bool in1, in2;
        boost::tie(e1, in1) = add_edge(verts[tail], verts[head], realEdgeID++, g); 
        boost::tie(e2, in2) = add_edge(verts[head], verts[tail], realEdgeID++, g); 
        if (!in1 || !in2) {
          std::cerr << "unable to add edge (" << head << "," << tail << ")"
                    << std::endl;
          return -1;
        }
        pCapacity[realEdgeID-2 /*e1*/ ] = 1;
        pCapacity[realEdgeID-1 /*e2*/ ] = 0;
        pReverse[realEdgeID-2 /*e1*/ ] = e2; 
        pReverse[realEdgeID-1 /*e2*/ ] = e1; 

      }
      break;

    } /* end of switch */
  }     /* end of input loop */

	//post-processing
	{
		//now check for bipartiteness and colorize
		typedef property_map<Graph, vertex_index_t>::type IndexMap;

		IndexMap index_map = get(vertex_index, g);
		partitionMap = new PartitionMap(num_vertices(g)); //its a vector!!!
		
		typedef iterator_property_map<PartitionMap::iterator, IndexMap> IteratorPartitionMap;
		IteratorPartitionMap iteratorPartitionMap((partitionMap)->begin(), index_map);

		bool really = is_bipartite(g, index_map, iteratorPartitionMap);
		std::cout << (really?"true":"false") << std::endl;

		graph_traits<Graph>::vertex_iterator vertex_iter, vertex_end;
		for(tie (vertex_iter, vertex_end) = vertices (g); vertex_iter != vertex_end; ++vertex_iter)
    	{   
			std::cout << "Vertex " << *vertex_iter << " has color " << (get(iteratorPartitionMap, *vertex_iter) == 
					color_traits<default_color_type>::white() ? "white" : "black") << std::endl;
		}   

		//now add edges from and to src and target
		for(tie (vertex_iter, vertex_end) = vertices (g); vertex_iter != vertex_end; ++vertex_iter)
    	{   
			if(*vertex_iter == src || *vertex_iter == sink)		continue;

			if(get(iteratorPartitionMap, *vertex_iter) == color_traits<default_color_type>::white())
			{
				//add edge from src to node
				edge_descriptor e1, e2;
				bool in1, in2;
				boost::tie(e1, in1) = add_edge(verts[src], verts[*vertex_iter], realEdgeID++, g); 
				boost::tie(e2, in2) = add_edge(verts[*vertex_iter], verts[src], realEdgeID++, g); 
				pCapacity[realEdgeID-2 /*e1*/ ] = 1;
				pCapacity[realEdgeID-1 /*e2*/ ] = 0;
				pReverse[realEdgeID-2 /*e1*/ ] = e2; 
				pReverse[realEdgeID-1 /*e2*/ ] = e1; 

			}
			else
			{
				//add edge from node to target
				edge_descriptor e1, e2;
				bool in1, in2;
				boost::tie(e1, in1) = add_edge(verts[*vertex_iter], verts[sink], realEdgeID++, g); 
				boost::tie(e2, in2) = add_edge(verts[sink], verts[*vertex_iter], realEdgeID++, g); 
				pCapacity[realEdgeID-2 /*e1*/ ] = 1;
				pCapacity[realEdgeID-1 /*e2*/ ] = 0;
				pReverse[realEdgeID-2 /*e1*/ ] = e2; 
				pReverse[realEdgeID-1 /*e2*/ ] = e1; 
			}
		}//for   
	}//post-processing


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
    
  make_directed_t md(g, iteratorPartitionMap/*clown *partition_map*/, capacity, src, sink);
  filtered_graph<Graph, make_directed_t> fg(g, md);
  std::vector<default_color_type> color(num_vertices(g));
  std::vector<Traits::edge_descriptor> pred(num_vertices(g));

  long flow = bipartite_matching_edmonds_karp
  	(fg, src, sink, capacity, residual_capacity, rev, &color[0], &pred[0]);

#if 0 

  long flow = bipartite_matching_push_relabel
  	(fg, src, sink, capacity, residual_capacity, rev, index_map);

#endif 

  std::cout << "Matching Number:" << std::endl;
  std::cout << flow << std::endl << std::endl;

  std::cout << "Matching:" << std::endl;
  graph_traits < Graph >::vertex_iterator u_iter, u_end;
  graph_traits < Graph >::out_edge_iterator ei, e_end;
  for (boost::tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter)
    for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei) {
      if ((capacity[*ei] - residual_capacity[*ei] == 1) && (*u_iter != src) && (target(*ei, g) != sink))
        std::cout << *u_iter << " " << target(*ei, g) << std::endl;
	}


  //now clean up
  delete partitionMap;
  delete[] pCapacity;
  delete[] pResidualCapacity;
  delete[] pReverse;

  return EXIT_SUCCESS;
}
