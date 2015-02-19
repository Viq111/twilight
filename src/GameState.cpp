#include "GameState.h"

GameState::GameState() : alliesCount(0), humansCount(0), enemiescount(0) {}
GameState::GameState(
	std::vector<Group> allies,
	std::vector<Group> humans,
	std::vector<Group> enemies)
	: alliesCount(0), humansCount(0), enemiescount(0)
{

}