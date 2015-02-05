#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

// ToDo: Include socket
//#include "MoveManager.h"

int main(int argc, char* argv[])
{
	// Parse the arguments
	std::vector<std::string> params(argv, argv + argc);

	/*
	// First create a connetion to the server
	std::unique_ptr<Socket> socket = std::make_unique<Socket>();
	// Then create the root of the tree
	//std::shared_ptr<Node> root = std::make_shared<Node>();

	// Create the MoveManager with the root node
	unsigned int nbCores = std::thread::hardware_concurrency();
	if (nbCores == 0)	// Couldn't determine the number of threads, use 2 by default
	{
		std::cerr << "[WARNING] Cannot find the real number of cores, using default 2" << std::endl;
		nbCores = 2;
	}
	std::cout << "Number of cores detected: " << nbCores << std::endl;
	MoveManager mm(root, nbCores);

	// Currently only benchmark
	mm.benchmark();
	*/

	return 0;
}