
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


int main()
{

  typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;

  typedef adjacency_list < listS, vecS, undirectedS,
    no_property,
    property < edge_index_t, size_t > > Graph;

  Graph g;

  typedef property_map<Graph, vertex_index_t>::type IndexMap;
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
        if (!in1/* || !in2*/) {
          std::cerr << "unable to add edge (" << head << "," << tail << ")"
                    << std::endl;
          return -1;
        }
      }
      break;

    } /* end of switch */
  }     /* end of input loop */

	//post-processing

	typedef property_map<Graph, edge_index_t>::type EdgeID_Map;
	EdgeID_Map edge_id_map = get(edge_index, g);

	IndexMap index_map = get(vertex_index, g);

	{
		//now check for bipartiteness and colorize
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

		// add fake source and target 
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

		edge_descriptor e1;
        bool in1;


		//now add edges from and to src and target
		for(tie (vertex_iter, vertex_end) = vertices (g); vertex_iter != vertex_end; ++vertex_iter)
    	{   
			if(*vertex_iter == src || *vertex_iter == sink)		continue;

			if(get(iteratorPartitionMap, *vertex_iter) == color_traits<default_color_type>::white())
			{
	
				//add reverse edges to real bipartite edges, assign capacity and reverse edge map values
				graph_traits < Graph >::out_edge_iterator ei, e_end;
				graph_traits<Graph>::degree_size_type outDegree = out_degree(*vertex_iter, g);
				unsigned int i=0;
				for (boost::tie(ei, e_end) = out_edges(*vertex_iter, g); ei != e_end && i < outDegree; ++ei, ++i) {

					unsigned int edgeID = get(edge_id_map, *ei);
        			boost::tie(e1, in1) = add_edge(verts[target(*ei, g)], verts[source(*ei, g)], realEdgeID, g); 

					pCapacity[edgeID] = 1;
					pCapacity[realEdgeID] = 0;
					pReverse[edgeID] = e1;
					pReverse[realEdgeID] = *ei;

					++realEdgeID;
				}


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
  iterator_property_map
    <unsigned int*, EdgeID_Map>
	  capacity(pCapacity, edge_id_map);

  iterator_property_map
    <unsigned int*, EdgeID_Map>
	  residual_capacity(pResidualCapacity, edge_id_map);

  iterator_property_map
    <edge_descriptor*, EdgeID_Map>
	  rev(pReverse, edge_id_map);


 //create a filtered graph
  typedef iterator_property_map<PartitionMap::iterator, IndexMap> IteratorPartitionMap;
  IndexMap indexMap = get(vertex_index, g);
  IteratorPartitionMap iteratorPartitionMap(partitionMap->begin(), indexMap);

  typedef make_directed<Graph, IteratorPartitionMap, iterator_property_map<unsigned int*, EdgeID_Map>, Traits::vertex_descriptor> make_directed_t;
    
  make_directed_t md(g, iteratorPartitionMap, capacity, src, sink);
  filtered_graph<Graph, make_directed_t> fg(g, md);
  std::vector<default_color_type> color(num_vertices(g));
  std::vector<Traits::edge_descriptor> pred(num_vertices(g));

#if 1

  long flow = bipartite_matching_edmonds_karp
  	(fg, src, sink, capacity, residual_capacity, rev, &color[0], &pred[0]);

#else  

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
