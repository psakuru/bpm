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

	if(numEdges > numNodes * numNodes) {
		std::cout << "Graph impossible as edges exceed max" << std::endl;
		return -1;
	}
	std::cout << "Generating graph with " << numNodes << " nodes and " << numEdges << " edges" << std::endl;

	mt19937 gen;
	boost::uniform_int<> range(1, numNodes);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > next(gen, range);

	boost::uniform_int<> range2(1, 5);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > next2(gen, range2);

	std::ostringstream fileName;
	fileName << "eg_" << numNodes << "_" << numEdges << ".dat";

	std::ofstream outFile(fileName.str().c_str());

	outFile << "p\tmax\t" << numNodes << "\t" << numEdges << std::endl;

	std::set<std::pair<int, int> > edgeSet;
	std::pair<int, int> edge;

	gen.seed(time(0));   // this is optional, but wise to do

	int first, second;

	for (int i = -1; i < numEdges; )
	{
		first = next();
		second = next();

		if(first == second) continue;

		edge.first = first;
		edge.second = second;

		if(edgeSet.count(edge) != 0) continue;

		if(i == -1) {
			outFile << "n\t" << first << "\ts" << std::endl;
			outFile << "n\t" << second << "\tt" << std::endl;
			++i;
			continue;
		}

		edgeSet.insert(edge);
		++i;
		outFile << "a\t" << edge.first << "\t" << edge.second << "\t" << next2() << std::endl;
	}
	outFile.close();
}

