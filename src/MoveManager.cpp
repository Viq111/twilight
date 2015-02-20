#include "MoveManager.h"

MoveManager::MoveManager(int n) : cores(n), socket(nullptr)
{
}

MoveManager::MoveManager(std::shared_ptr<Node> r, int n) : cores(n), socket(nullptr), root(r)
{
}

void MoveManager::setSocket(std::unique_ptr<GameSocket> s)
{
	socket = std::move(s);
}
void MoveManager::benchmark()
{
	std::cout << "To Implement" << std::endl;
}

void MoveManager::mainloop()
{
	// ToDo
}

// Worker thread part
bool MoveManager::stillWork() // Still work to do ?
{
	return true;
}

std::shared_ptr<Node> MoveManager::callback(std::shared_ptr<Node> initialGameState)
{
	// ToDo: Implement
	std::cout << "Server has a new GameState!" << std::endl;
	return initialGameState;
}