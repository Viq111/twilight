#include "MoveManager.h"

MoveManager::MoveManager(std::shared_ptr<Node> r, int n) : cores(n), root(r), socket(nullptr)
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