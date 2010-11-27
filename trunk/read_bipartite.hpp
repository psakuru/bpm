#ifndef BOOST_GRAPH_READ_DIMACS_HPP
#define BOOST_GRAPH_READ_DIMACS_HPP

#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <boost/graph/graph_traits.hpp>

namespace boost {

template <class Graph>
int read_dimacs_bipartite(Graph& g,
                         unsigned int** ppCapacity,
                         typename graph_traits<Graph>::edge_descriptor** ppReverse,
                         unsigned int** ppResidualCapacity,
                         typename graph_traits<Graph>::vertex_descriptor& src,
                         typename graph_traits<Graph>::vertex_descriptor& sink,
                         std::istream& in = std::cin)
{
  //  const int MAXLINE = 100;      /* max line length in the input file */
  const int ARC_FIELDS = 2;     /* no of fields in arc line  */
  const int NODE_FIELDS = 2;    /* no of fields in node line  */
  const int P_FIELDS = 3;       /* no of fields in problem line */
  const char* PROBLEM_TYPE = "max"; /* name of problem type*/

  typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
  typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  typedef typename graph_traits<Graph>::edge_descriptor edge_descriptor;

  std::vector<vertex_descriptor> verts; //stores vertex descriptors for each added node
  std::vector<bool> marked; //used to mark nodes when fake edges are added

  //arrays to fill in after memory allocation
  unsigned int* pCapacity;
  unsigned int* pResidualCapacity;
  edge_descriptor* pReverse;

  //giving edge ids to real and fake edges and their reverses
  //all reverse edges get original_edge_id + 1
  //so all original edges are even, reverse edges are odd
  unsigned int realEdgeID, fakeEdgeID; 

  long m, n,                    /*  number of edges and nodes */
    i, head, tail;

  long no_lines=0,              /* no of current input line */
    no_plines=0,                /* no of problem-lines */
    no_nslines=0,               /* no of node-source-lines */
    no_nklines=0,               /* no of node-source-lines */
    no_alines=0;                /* no of arc-lines */

  std::string in_line;          /* for reading input line */
  char pr_type[4];              /* for reading type of the problem */
  char nd;                      /* source (s) or sink (t) */

  int k,                        /* temporary */
    err_no;                     /* no of detected error */

  const int EN1   = 0;
  const int EN2   = 1;
  const int EN3   = 2;
  const int EN4   = 3;
  const int EN8   = 7;
  const int EN9   = 8;
  const int EN11  = 9;
  const int EN12 = 10;
  const int EN14 = 12;
  const int EN16 = 13;
  const int EN15 = 14;
  const int EN17 = 15;
  const int EN18 = 16;
  const int EN21 = 17;
  const int EN19 = 18;
  const int EN20 = 19;
  const int EN22 = 20;

  static char *err_message[] =
  {
    /* 0*/    (char*)"more than one problem line.",
    /* 1*/    (char*)"wrong number of parameters in the problem line.",
    /* 2*/    (char*)"it is not a Max Flow problem line.",
    /* 3*/    (char*)"bad value of a parameter in the problem line.",
    /* 4*/    (char*)"can't obtain enough memory to solve this problem.",
    /* 5*/    (char*)"more than one line with the problem name.",
    /* 6*/    (char*)"can't read problem name.",
    /* 7*/    (char*)"problem description must be before node description.",
    /* 8*/    (char*)"this parser doesn't support multiply sources and sinks.",
    /* 9*/    (char*)"wrong number of parameters in the node line.",
    /*10*/    (char*)"wrong value of parameters in the node line.",
    /*11*/    (char*)" ",
    /*12*/    (char*)"source and sink descriptions must be before arc descriptions.",
    /*13*/    (char*)"too many arcs in the input.",
    /*14*/    (char*)"wrong number of parameters in the arc line.",
    /*15*/    (char*)"wrong value of parameters in the arc line.",
    /*16*/    (char*)"unknown line type in the input.",
    /*17*/    (char*)"reading error.",
    /*18*/    (char*)"not enough arcs in the input.",
    /*19*/    (char*)"source or sink doesn't have incident arcs.",
    /*20*/    (char*)"can't read anything from the input file."
  };
  /* --------------------------------------------------------------- */

  
  while (std::getline(in, in_line)) {
    ++no_lines;

    switch (in_line[0]) {
    case 'c':                  /* skip lines with comments */
    case '\n':                 /* skip empty lines   */
    case '\0':                 /* skip empty lines at the end of file */
      break;

    case 'p':                  /* problem description      */
	{
      if ( no_plines > 0 )
        /* more than one problem line */
        { err_no = EN1 ; goto error; }

      no_plines = 1;

      if (
          /* reading problem line: type of problem, no of nodes, no of arcs */
          std::sscanf ( in_line.c_str(), "%*c %3s %ld %ld", pr_type, &n, &m )
          != P_FIELDS
          )
        /*wrong number of parameters in the problem line*/
        { err_no = EN2; goto error; }

      if ( std::strcmp ( pr_type, PROBLEM_TYPE ) )
        /*wrong problem type*/
        { err_no = EN3; goto error; }

      if ( n <= 0  || m <= 0 )
        /*wrong value of no of arcs or nodes*/
        { err_no = EN4; goto error; }

      {
        for (long vi = 0; vi < n; ++vi)
          verts.push_back(add_vertex(g));
      }

      // add fake source and target 
	  n += 2;
      verts.push_back(add_vertex(g)); //source at index n-2
	  verts.push_back(add_vertex(g)); //target at index n-1

	  // initialize the 'marked' bool vector now that you know the size
	  std::vector<bool> _marked(n, false);
	  marked = _marked;

	  // initialize src and sink
	  src = verts[n-2];
	  sink = verts[n-1];

	  std::cout << "src: " << src << "\t" << "sink: " << sink << std::endl;

	  //create dynamic memory for capacity and reverse
	  //n-2 fake edges(one for each of the n-2 real nodes) and m bipartite edges
	  //double that for reverses
	  *ppCapacity = new unsigned int[2*(n-2 + m)]; 
	  *ppResidualCapacity = new unsigned int[2*(n-2 + m)];
	  *ppReverse = new edge_descriptor[2*(n-2 + m)];

	  //now define arrays to be used
	  pCapacity = *ppCapacity;
	  pResidualCapacity = *ppResidualCapacity;
	  pReverse = *ppReverse;

	  //now initialize the edge IDs
	  realEdgeID = 0; //range 0 ~ 2m-1
	  fakeEdgeID = 2*m; //range 2m ~ beyond{2(n-2+m)-1}
	}
      break;

    case 'a':                    /* arc description */
      if ( no_alines >= m )
        /*too many arcs on input*/
        { err_no = EN16; goto error; }

      if (
          /* reading an arc description */
          std::sscanf ( in_line.c_str(),"%*c %ld %ld",
                        &tail, &head)
          != ARC_FIELDS
          )
        /* arc description is not correct */
        { err_no = EN15; goto error; }

      --tail; // index from 0, not 1
      --head;
      if ( tail < 0  ||  tail > n  ||
           head < 0  ||  head > n
           )
        /* wrong value of nodes */
        { err_no = EN17; goto error; }

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

		// add the fake edge from source to tail and its reverse
		if(marked[tail] == false)
		{
			boost::tie(e1, in1) = add_edge(verts[n-2], verts[tail], fakeEdgeID++, g); 
			boost::tie(e2, in2) = add_edge(verts[tail], verts[n-2], fakeEdgeID++, g);
			if (!in1 || !in2) {
			  std::cerr << "unable to add edge (" << n-2 << "," << tail << ")"
						<< std::endl;
			  return -1;
			}
			pCapacity[fakeEdgeID-2 /*e1*/ ] = 1;
			pCapacity[fakeEdgeID-1 /*e2*/ ] = 0;
			pReverse[fakeEdgeID-2 /*e1*/ ] = e2;
			pReverse[fakeEdgeID-1 /*e2*/ ] = e1;
			
			marked[tail] = true;
		}
        
		// add the fake edge from head to target and its reverses
		if(marked[head] == false)
		{
			boost::tie(e1, in1) = add_edge(verts[head], verts[n-1], fakeEdgeID++, g);
			boost::tie(e2, in2) = add_edge(verts[n-1], verts[head], fakeEdgeID++, g);
			if (!in1 || !in2) {
			  std::cerr << "unable to add edge (" << head << "," << n-1 << ")"
						<< std::endl;
			  return -1;
			}
			pCapacity[fakeEdgeID-2 /*e1*/ ] = 1;
			pCapacity[fakeEdgeID-1 /*e2*/ ] = 0;
			pReverse[fakeEdgeID-2 /*e1*/ ] = e2;
			pReverse[fakeEdgeID-1 /*e2*/ ] = e1;

			marked[head] = true;
		}

      }
      ++no_alines;
      break;

    default:
      err_no = EN18; goto error;

    } /* end of switch */
  }     /* end of input loop */

  if ( in.eof() == 0 ) /* reading error */
    { err_no=EN21; goto error; }

  if ( no_lines == 0 ) /* empty input */
    { err_no = EN22; goto error; }

  if ( no_alines < m ) /* not enough arcs */
    { err_no = EN19; goto error; }

  if ( out_degree(src, g) == 0 || out_degree(sink, g) == 0  )
    /* no arc goes out of the source */
    { err_no = EN20; goto error; }

  /* Thanks God! all is done */
  return (0);

  /* ---------------------------------- */
 error:  /* error found reading input */

  std::printf ( "\nline %ld of input - %s\n",
                no_lines, err_message[err_no] );

  std::exit (1);
  return (0); /* to avoid warning */
}

} // namespace boost

#endif // BOOST_GRAPH_READ_DIMACS_HPP
