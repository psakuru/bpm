#include <boost/random.hpp>
#include <boost/nondet_random.hpp>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <set>
#include <fstream>
#include <string>
#include <sstream>

using namespace boost;

int main(int argc, char**argv)
{
	if(argc != 3) {
		std::cout << "Usage: randomplay numNodes numEdges" << std::endl;
		return -1;
	}

	int numNodes = atoi(argv[1]);
	int numEdges = atoi(argv[2]);

	if(numEdges > (numNodes*numNodes)/4) {
		std::cout << "Graph impossible as edges exceed max" << std::endl;
		return -1;
	}
	if(numNodes & 0x01) {
		std::cout << "Please use even number of nodes for now" << std::endl;
		return -1;
	}
	std::cout << "Generating bipartite graph with " << numNodes << " nodes and " << numEdges << " edges" << std::endl;

	mt19937 gen;
	boost::uniform_int<> range1(1, numNodes/2);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > next1(gen, range1);

	boost::uniform_int<> range2(numNodes/2+1, numNodes);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > next2(gen, range2);

	std::ostringstream fileName;
	fileName << "bipartite_" << numNodes << "_" << numEdges << ".dat";

	std::ofstream outFile(fileName.str().c_str());

	outFile << "p\tmax\t" << numNodes << "\t" << numEdges << std::endl;

	std::set<std::pair<int, int> > edgeSet;
	std::pair<int, int> edge;

	gen.seed(time(0));   // this is optional, but wise to do

	for (int i = 0; i < numEdges; )
	{
		edge.first = next1();
		edge.second = next2();

		if(edgeSet.count(edge) == 0) {
			edgeSet.insert(edge);
			++i;
			outFile << "a\t" << edge.first << "\t" << edge.second << std::endl;
		}
	}
	outFile.close();
}

