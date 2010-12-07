#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include "bipartite_matching.hpp"
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/bipartite.hpp>
#include <vector>
#include <cstdio>

#include <boost/graph/graph_traits.hpp>


using namespace boost;


int main()
{

  typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;

  typedef adjacency_list < listS, vecS, undirectedS,
    no_property,
    property < edge_index_t, size_t > > Graph;

  Graph g;

  typedef graph_traits<Graph>::vertices_size_type vertices_size_type;
  typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  typedef graph_traits<Graph>::edge_descriptor edge_descriptor;

  std::vector<vertex_descriptor> verts; //stores vertex descriptors for each added node

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
        if (!in1) {
          std::cout << "unable to add edge (" << head << "," << tail << ")"
                    << std::endl;
          return -1;
        }
      }
      break;

    } /* end of switch */
  } /* end of input loop */

	bipartite_matching_edmonds_karp(g, n, m);

  return EXIT_SUCCESS;
}
