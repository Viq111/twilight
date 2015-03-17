#include <iostream>
#include "GameSocket.h"
#include "standardFunctions.h" // For make_unique

int main(int argc, char* argv[])
{

	// Parse the arguments
	std::vector<std::string> params(argv, argv + argc);

	if (params.size() != 4)
	{
		std::cout << "Usage: " << params.at(0) << " <ip> <port> <name>" << std::endl;
		return -1;
	}

    std::string ip = params.at(1);
	int port = std::stoi(params.at(2));
    std::string name = params.at(3);

	GameSocket socket = GameSocket(ip, port, name);
	socket.connect();

    return 0;
}