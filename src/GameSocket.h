#ifndef INCLUDE_GAMESOCKET
#define INCLUDE_GAMESOCKET

#include <string>
#include <memory>
#include "GameState.h"

typedef GameState(*PlayCallback)(GameState); // Define a callback which take the new GameState and return the played moved (GameState)

class GameSocket
{
public:
	GameSocket(PlayCallback callback = nullptr);
	void setCallback(PlayCallback callback); // Set a new callback
	void connect(std::string ip, int port, std::string name); // Connect to a server and set the name
	std::pair<unsigned int, unsigned int> getBoardSize();
	bool isAlive(); // Return whether the connection with the server is still alive
	void join(); // Wait for thread to end
	void close(); // Close the socket
protected:
	std::string myName;
	GameState currentGameState;
	PlayCallback callback;
};

#endif