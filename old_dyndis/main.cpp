#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include "dyndis_max_flow.hpp"
#include <vector>
#include <cstdio>
#include <cstdlib>

#include <boost/graph/graph_traits.hpp>


using namespace boost;


int main(int argc, char** argv)
{
	if(argc != 2) {
		std::cout << "Usage: mf algoType <graphFile" << std::endl;
		return -1;
	}

  typedef adjacency_list_traits < vecS, vecS, directedS > Traits;

  typedef adjacency_list < listS, vecS, directedS,
    no_property,
    property < edge_index_t, size_t > > Graph;

  Graph g;

  typedef graph_traits<Graph>::vertices_size_type vertices_size_type;
  typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  typedef graph_traits<Graph>::edge_descriptor edge_descriptor;

  //giving edge ids to real and fake edges and their reverses
  //all reverse edges get original_edge_id + 1
  //so all original edges are even, reverse edges are odd
  unsigned int realEdgeID; 

  long m, n,                    /*  number of edges and nodes */
    i, head, tail, tempCap;

  std::string in_line;          /* for reading input line */
  char pr_type[4];              /* for reading type of the problem */
  char nd;

  std::istream& in = std::cin;

  vertex_descriptor src, sink;

  //capacity map
  unsigned int *pCapacity;

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
		  add_vertex(g);
      }

	  //now initialize the edge IDs
	  realEdgeID = 0;

	  //create a capacity map considering reverse and fake edges
	  pCapacity = new unsigned int[2*m]; 
	}
      break;

	case 'n':                    /* source(s) description */

      /* reading source  or sink */
      std::sscanf ( in_line.c_str(),"%*c %ld %c", &i, &nd );
      --i; // index from 0

      switch (nd) {
      case 's':  /* source line */

        src = i;
        break;

      case 't':  /* sink line */

        sink = i;
        break;

      }
    break;


    case 'a':                    /* arc description */

          std::sscanf ( in_line.c_str(),"%*c %ld %ld %ld",
                        &tail, &head, &tempCap);

      --tail; // index from 0, not 1
      --head;

      {
        edge_descriptor e1, e2;
        bool in1, in2;
        boost::tie(e1, in1) = add_edge(tail, head, realEdgeID++, g);
        if (!in1) {
          std::cout << "unable to add edge (" << head << "," << tail << ")"
                    << std::endl;
          return -1;
        }
		pCapacity[realEdgeID] = tempCap;
      }
      break;

    } /* end of switch */
  } /* end of input loop */

	
  AlgoTag algo_tag = (atoi(argv[1]) == 0)? edmonds_karp : push_relabel;

	unsigned int* pMatching = (unsigned int*)calloc(m, sizeof(unsigned int));
	typedef property_map<Graph, edge_index_t>::type EdgeID_Map;
	EdgeID_Map edge_id_map = get(edge_index, g);

	iterator_property_map
	  <unsigned int*, EdgeID_Map>
	    mat(pMatching, edge_id_map);

	iterator_property_map
	  <unsigned int*, EdgeID_Map>
	    cap(pCapacity, edge_id_map);

	unsigned int flow = dyndis_max_flow(g, mat, cap, n, m, src, sink, algo_tag);

	std::cout << "Matching: " << flow << ": " << std::endl;
	for(unsigned int i=0; i<m; ++i) {
		std::cout << pMatching[i] << std::endl;
	}

	free(pMatching);
	delete[] pCapacity;

  return EXIT_SUCCESS;
}
