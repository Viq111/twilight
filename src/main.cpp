#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

#include "GameSocket.h"
#include "MoveManager.h"
#include "standardFunctions.h" // For make_unique

int getNbCores()
{
	unsigned int nbCores = std::thread::hardware_concurrency();
	if (nbCores == 0)	// Couldn't determine the number of threads, use 2 by default
	{
		std::cerr << "[WARNING] Cannot find the real number of cores, using default 2" << std::endl;
		nbCores = 2;
	}
	return nbCores;
}

int main(int argc, char* argv[])
{
	// Parse the arguments
	std::vector<std::string> params(argv, argv + argc);

	// Get cores
	int nbCores = getNbCores();

	// If params is benchmark, do it
	if (((params.size() == 2) && params[1] == "benchmark") || params.size() == 1)
	{
		std::cout << "Benchmarking ..." << std::endl;
		GameState::setBoardSize(10, 15); // Set default board size
		std::shared_ptr<Node> root = std::make_shared<Node>(std::make_unique<GameState>()); // Empty Node
		MoveManager mm(root, nbCores);
		mm.benchmark();
		return 0;
	}
	// Check number of arguments
	if (params.size() != 3)
	{
		std::cout << "Usage: " << params.at(0) << " <ip> <port>" << std::endl;
		return -1;
	}
	// Number of arguments seems good, get ip and port
	std::string ip = params.at(1);
	int port = std::stoi(params.at(2));

	// First create a connetion to the server
	std::unique_ptr<GameSocket> socket = make_unique<GameSocket>();
	socket->connect(ip, port, "Edward");

	// Create the MoveManager with the root node
	MoveManager mm(nbCores);
	mm.setSocket(std::move(socket));

	mm.mainloop();
	return 0;
}