#include "GameSocket.h"

GameSocket::GameSocket() : myName("Edward") {}

void GameSocket::connect(std::string ip, int port, std::string name)
{
	// ToDo: Connect to the server and set the name
}

GameState GameSocket::getNewGameState()
{
	// ToDo: Block until the server send a UPD
	// Compute the new gameState
	// And return a copy of the GameState
	std::vector<Group> none;
	return GameState(none, none, none);
}