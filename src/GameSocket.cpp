#include "GameSocket.h"

GameSocket::GameSocket(PlayCallback c) : myName("Edward"), callback(c) {}

void GameSocket::connect(std::string ip, int port, std::string name)
{
	// ToDo: Connect to the server and set the name
	// Create a thread to listen for answer and fire callback
	// When fiering callback, the return Node is the move
	// Compare to old Node and check where we moved
	// If callback is nullptr, do nothing (or maybe print)
}

void GameSocket::setCallback(PlayCallback c)
{
	callback = c;
}

bool GameSocket::isAlive()
{
	// ToDo: Check if the thread or connection is still alive
	return true;
}

void GameSocket::join()
{
	// ToDo: Wait the thread
}

void GameSocket::close()
{
	// ToDo: Close the socket and stop the thread
}
