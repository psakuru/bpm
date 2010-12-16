#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

int main(int argc, char**argv)
{
	if(argc != 2) {
		std::cout << "Usage: lame_randomplay numNodes" << std::endl;
		return -1;
	}

	int numNodes = atoi(argv[1]);

	if(numNodes & 0x01) {
		std::cout << "Please use even number of nodes for now" << std::endl;
		return -1;
	}
	std::cout << "Generating bipartite graph with " << numNodes << " nodes" << std::endl;

	std::ostringstream fileName;
	fileName << "lame_bipartite_" << numNodes << ".dat";

	std::ofstream outFile(fileName.str().c_str());

	outFile << "p\tmax\t" << numNodes << "\t" << numNodes/2 << std::endl;

	for (int i = 1, j = numNodes/2+1; i <= numNodes/2; ++i, ++j)
	{
		outFile << "a\t" << i << "\t" << j << std::endl;
	}
	outFile.close();
}

