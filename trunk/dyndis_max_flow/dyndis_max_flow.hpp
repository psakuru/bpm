#ifndef DYNDIS_MAX_FLOW_HPP
#define DYNDIS_MAX_FLOW_HPP

#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <assert.h>
#include <ctime>

namespace boost {

#define DENSITY_THRESHOLD 0.5 //TODO: determine empirically

typedef enum { edmonds_karp, push_relabel } AlgoTag;


  //Algorithm specified by user
  template <class Graph, class MatchingEdgeMap, class CapacityEdgeMap>
  unsigned int
  dyndis_max_flow
    (Graph& g,
	MatchingEdgeMap& mat,
	CapacityEdgeMap& cap,
	unsigned int n,
	unsigned int m,
	typename graph_traits<Graph>::vertex_descriptor src,
	typename graph_traits<Graph>::vertex_descriptor sink,
	AlgoTag algo_tag)
  {

	typedef typename property_map<Graph, vertex_index_t>::type IndexMap;
	typedef typename property_map<Graph, edge_index_t>::type EdgeID_Map;

	typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
	typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef typename graph_traits<Graph>::edge_descriptor edge_descriptor;
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename graph_traits<Graph>::out_edge_iterator out_edge_iterator;
	typedef typename graph_traits<Graph>::degree_size_type degree_size_type;

	//arrays to fill in after memory allocation
	unsigned int* pResidualCapacity;
	edge_descriptor* pReverse;


	EdgeID_Map edge_id_map = get(edge_index, g);
	IndexMap index_map = get(vertex_index, g);

	
	std::cout << "src: " << src << "\t" << "sink: " << sink << std::endl;

	//create dynamic memory for capacity and reverse
	//n-2 fake edges(one for each of the n-2 real nodes) and m bipartite edges
	//double that for reverses
	pResidualCapacity = new unsigned int[2*m];
	pReverse = new edge_descriptor[2*m];

	//define realEdgeID
	unsigned int realEdgeID = m; //added m edges from input already

	edge_descriptor e1;
	bool in1;


	//add reverse edges to existing directed edges, assign capacity and reverse edge map values
	vertex_iterator vertex_iter, vertex_end;
	for(tie (vertex_iter, vertex_end) = vertices (g); vertex_iter != vertex_end; ++vertex_iter)
	{   
		out_edge_iterator ei, e_end;
		for (boost::tie(ei, e_end) = out_edges(*vertex_iter, g); ei != e_end; ++ei) {

			unsigned int edgeID = get(edge_id_map, *ei);
			boost::tie(e1, in1) = add_edge(target(*ei, g), source(*ei, g), realEdgeID, g); 

			//pCapacity[edgeID] = 1; //user-defined
			//pCapacity[realEdgeID] = 0;
			put(cap, e1, 0);
			pReverse[edgeID] = e1;
			pReverse[realEdgeID] = *ei;

			++realEdgeID;
		}
	}//for   

	//create maps out of arrays
	iterator_property_map
	  <unsigned int*, EdgeID_Map>
	    residual_capacity(pResidualCapacity, edge_id_map);

	iterator_property_map
	  <edge_descriptor*, EdgeID_Map>
	    rev(pReverse, edge_id_map);

	std::vector<default_color_type> color(num_vertices(g));
	std::vector<edge_descriptor> pred(num_vertices(g));


	long flow = -1;
	time_t start, end;
	double diff;

	if(algo_tag == edmonds_karp) {

		time(&start);
		//for(int i=0; i < 1000; ++i)
		flow = edmonds_karp_max_flow
		  (g, src, sink, cap, residual_capacity, rev, &color[0], &pred[0]);

		time(&end);
		diff = difftime(end, start);
		std::cout << "Time taken = " << diff << std::endl;

		std::cout << "EK flow is: " << flow << std::endl;
	}
	else if(algo_tag == push_relabel) {

		time(&start);
		//for(int i=0; i < 1000; ++i)
		flow = push_relabel_max_flow
		  (g, src, sink, cap, residual_capacity, rev, index_map);

		time(&end);
		diff = difftime(end, start);
		std::cout << "Time taken = " << diff << std::endl;


		std::cout << "PR flow is: " << flow << std::endl;
	}
	else {
		std::cout << "Invalid algorithm tag specified... Exiting!!!" << std::endl;
		return 0;
	}


	vertex_iterator u_iter, u_end;
	out_edge_iterator ei, e_end;
	unsigned int matched_edge_count = 0;
	for (boost::tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter) {

		for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei) {

			if (get(cap, *ei) - residual_capacity[*ei] == 1) {

				put(mat, *ei, 1);
				++matched_edge_count;
			}
		}
	}

	assert(flow == matched_edge_count);

	//now clean up
	delete[] pResidualCapacity;
	delete[] pReverse;

	return flow;
  }

  /*//Algorithm decided based on density
  template <class Graph, class MatchingEdgeMap>
  unsigned int
  bipartite_matching
    (Graph& g,
	MatchingEdgeMap& mat,
	unsigned int n,
	unsigned int m)
  {
	  float density = 2.0 * m / (n*(n-1));  // 2E / V(V-1)

	  AlgoTag algo_tag;
	  if(density > DENSITY_THRESHOLD)
		  algo_tag = push_relabel;
	  else
		  algo_tag = edmonds_karp;

	  return bipartite_matching(g, mat, n, m, algo_tag);

  }*/ //TODO

} //namespace boost

#endif //DYNDIS_MAX_FLOW
