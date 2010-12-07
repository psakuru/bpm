#ifndef BIPARTITE_MATCHING_HPP
#define BIPARTITE_MATCHING_HPP

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

template <class Graph, 
		class CapacityEdgeMap, class ResidualCapacityEdgeMap,
		class ReverseEdgeMap, class VertexIndexMap>
typename property_traits<CapacityEdgeMap>::value_type
bipartite_matching_push_relabel
  (Graph& g, 
   typename graph_traits<Graph>::vertex_descriptor src,
   typename graph_traits<Graph>::vertex_descriptor sink,
   CapacityEdgeMap cap, 
   ResidualCapacityEdgeMap res,
   ReverseEdgeMap rev, 
   VertexIndexMap indexMap) 
{
	return push_relabel_max_flow(g, src, sink, cap, res, rev, indexMap);
}


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

  //TODO: add MatchingEdgeMap
  template <class Graph>
  unsigned int
  bipartite_matching_edmonds_karp
    (Graph& g,
	unsigned int n,
	unsigned int m)
  {

	typedef typename property_map<Graph, vertex_index_t>::type IndexMap;
	typedef typename property_map<Graph, edge_index_t>::type EdgeID_Map;
	typedef std::vector<default_color_type> PartitionMap;
	PartitionMap *partitionMap; 

	typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
	typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef typename graph_traits<Graph>::edge_descriptor edge_descriptor;
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename graph_traits<Graph>::out_edge_iterator out_edge_iterator;
	typedef typename graph_traits<Graph>::degree_size_type degree_size_type;

	vertex_descriptor src, sink;

	//arrays to fill in after memory allocation
	unsigned int* pCapacity;
	unsigned int* pResidualCapacity;
	edge_descriptor* pReverse;


	EdgeID_Map edge_id_map = get(edge_index, g);
	IndexMap index_map = get(vertex_index, g);

	//now check for bipartiteness and colorize
	partitionMap = new PartitionMap(num_vertices(g)); //its a vector!!!
	
	typedef iterator_property_map<PartitionMap::iterator, IndexMap> IteratorPartitionMap;
	IteratorPartitionMap iteratorPartitionMap((partitionMap)->begin(), index_map);

	bool really = is_bipartite(g, index_map, iteratorPartitionMap);
	std::cout << (really?"true":"false") << std::endl;

	vertex_iterator vertex_iter, vertex_end;
	for(tie (vertex_iter, vertex_end) = vertices (g); vertex_iter != vertex_end; ++vertex_iter)
	{   
		std::cout << "Vertex " << *vertex_iter << " has color " << (get(iteratorPartitionMap, *vertex_iter) == 
				color_traits<default_color_type>::white() ? "white" : "black") << std::endl;
	}

	// add fake source and target 
	n += 2;
	add_vertex(g);
	add_vertex(g);

	// initialize src and sink
	src = n-2;
	sink = n-1;

	std::cout << "src: " << src << "\t" << "sink: " << sink << std::endl;

	//create dynamic memory for capacity and reverse
	//n-2 fake edges(one for each of the n-2 real nodes) and m bipartite edges
	//double that for reverses
	pCapacity = new unsigned int[2*(n-2 + m)]; 
	pResidualCapacity = new unsigned int[2*(n-2 + m)];
	pReverse = new edge_descriptor[2*(n-2 + m)];

	//define realEdgeID
	unsigned int realEdgeID = m;

	edge_descriptor e1;
	bool in1;


	//now add edges from and to src and target
	for(tie (vertex_iter, vertex_end) = vertices (g); vertex_iter != vertex_end; ++vertex_iter)
	{   
		if(*vertex_iter == src || *vertex_iter == sink)		continue;

		if(get(iteratorPartitionMap, *vertex_iter) == color_traits<default_color_type>::white())
		{

			//add reverse edges to real bipartite edges, assign capacity and reverse edge map values
			out_edge_iterator ei, e_end;
			degree_size_type outDegree = out_degree(*vertex_iter, g);
			unsigned int i=0;
			for (boost::tie(ei, e_end) = out_edges(*vertex_iter, g); ei != e_end && i < outDegree; ++ei, ++i) {

				unsigned int edgeID = get(edge_id_map, *ei);
				boost::tie(e1, in1) = add_edge(target(*ei, g), source(*ei, g), realEdgeID, g); 

				pCapacity[edgeID] = 1;
				pCapacity[realEdgeID] = 0;
				pReverse[edgeID] = e1;
				pReverse[realEdgeID] = *ei;

				++realEdgeID;
			}


			//add edge from src to node
			edge_descriptor e1, e2;
			bool in1, in2;
			boost::tie(e1, in1) = add_edge(src, *vertex_iter, realEdgeID++, g); 
			boost::tie(e2, in2) = add_edge(*vertex_iter, src, realEdgeID++, g); 
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
			boost::tie(e1, in1) = add_edge(*vertex_iter, sink, realEdgeID++, g); 
			boost::tie(e2, in2) = add_edge(sink, *vertex_iter, realEdgeID++, g); 
			pCapacity[realEdgeID-2 /*e1*/ ] = 1;
			pCapacity[realEdgeID-1 /*e2*/ ] = 0;
			pReverse[realEdgeID-2 /*e1*/ ] = e2; 
			pReverse[realEdgeID-1 /*e2*/ ] = e1; 
		}
	}//for   

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

  typedef make_directed<Graph, IteratorPartitionMap, iterator_property_map<unsigned int*, EdgeID_Map>, vertex_descriptor> make_directed_t;
    
  make_directed_t md(g, iteratorPartitionMap, capacity, src, sink);
  filtered_graph<Graph, make_directed_t> fg(g, md);
  std::vector<default_color_type> color(num_vertices(g));
  std::vector<edge_descriptor> pred(num_vertices(g));

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
  vertex_iterator u_iter, u_end;
  out_edge_iterator ei, e_end;
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

	  return 0;
  }


} //namespace boost

#endif //BIPARTITE_MATCHING_HPP
