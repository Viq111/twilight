#ifndef INCLUDE_GAMESOCKET
#define INCLUDE_GAMESOCKET

#include <vector>
#include <string>
#include <memory>
#include "GameState.h"

class GameSocket
{
public:
	GameSocket();
	void connect(std::string ip, int port, std::string name); // Connect to a server and set the name
	GameState getNewGameState(); // Block and return the new State of the game
protected:
	std::string myName;
	GameState currentGameState;
};

#endif