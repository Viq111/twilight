#include "GameState.h"

// Default board size
unsigned int GameState::n = 10;
unsigned int GameState::m = 15;
int GameState::opsFactor = 60;

void GameState::setBoardSize(unsigned int n_, unsigned int m_)
{
	n = n_;
	m = m_;
}

GameState::GameState() : alliesCount(0), humansCount(0), enemiesCount(0) {}
GameState::GameState(
	std::vector<Group> allies,
	std::vector<Group> humans,
	std::vector<Group> enemies)
	: alliesCount(0),
	  humansCount(0),
	  enemiesCount(0),
	  allies(allies),
	  humans(humans),
	  enemies(enemies)
{

}

// TODO: take into account random battles, and add coefficients
int GameState::getScore() {
	int score = 0;

	// Number of allies and enemies
	score += opsFactor * (alliesCount - enemiesCount) / (alliesCount + enemiesCount);

	// Proximity to human groups
	for (auto&& human: humans) {
		for (auto&& ally: allies) {
			if (human.count <= ally.count) {
				score += opsFactor * human.count / distance(ally, human);
			}
		}
		for (auto&& enemy: enemies) {
			if (human.count <= enemy.count) {
				score += -opsFactor * enemy.count / distance(enemy, human);
			}
		}
	}

	// Distance between allied and enemy groups
	for (auto&& ally: allies) {
		for (auto&& enemy: enemies) {
			if (3 * enemy.count <= 2 * ally.count) {
				score += opsFactor * enemy.count / distance(enemy, ally);
			}
			else if (3 * ally.count <= 2 * enemy.count) {
				score += -opsFactor * ally.count / distance(enemy, ally);
			}
		}
	}

	return score;
}

int GameState::distance(const Group& group1, const Group& group2) {
	return std::max(std::abs(group2.x - group1.x), std::abs(group2.y - group1.y));
}
