#include "GameState.h"

// Default board size
unsigned int GameState::n = 10;
unsigned int GameState::m = 15;

void GameState::setBoardSize(unsigned int n_, unsigned int m_)
{
	n = n_;
	m = m_;
}

GameState::GameState() : alliesCount(0), humansCount(0), enemiescount(0) {}
GameState::GameState(
	std::vector<Group> allies,
	std::vector<Group> humans,
	std::vector<Group> enemies)
	: alliesCount(0), humansCount(0), enemiescount(0)
{

}