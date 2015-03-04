#include "GameState.h"
#include "standardFunctions.h"

// Default board size
int GameState::n = 0;
int GameState::m = 0;
int GameState::opsFactor = 60;
int GameState::populationCountsWeight = 30;
int GameState::humanProximityWeight = 20;
int GameState::enemyProximityWeight = 20;

void GameState::setBoardSize(int n_, int m_)
{
	n = n_;
	m = m_;
}

GameState::GameState() : alliesCount(0), humansCount(0), enemiesCount(0) {}
GameState::GameState(
	std::vector<Group> allies,
	std::vector<Group> humans,
	std::vector<Group> enemies)
	: allies(allies),
	  humans(humans),
	  enemies(enemies),
	  alliesCount(racePopulation(allies)),
      humansCount(racePopulation(humans)),
      enemiesCount(racePopulation(enemies))
{
    if (!GameState::n || !GameState::m) {
        throw "GameState_board_size_not_initialized";
    }
}

// TODO: add coefficients (by machine learning or manual benchmarking)
int GameState::getScore() {
	int score = 0;

	// Number of allies and enemies
    score += (opsFactor * populationCountsWeight * (alliesCount - enemiesCount)) / (alliesCount + enemiesCount);

	// Proximity to human groups
	for (auto&& human: humans) {
		for (auto&& ally: allies) {
			if (human.count <= ally.count) {
                score += (opsFactor * humanProximityWeight * human.count) / distance(ally, human);
            }
		}
		for (auto&& enemy: enemies) {
			if (human.count <= enemy.count) {
                score += -(opsFactor * humanProximityWeight * enemy.count) / distance(enemy, human);
            }
		}
	}

	// Distance between allied and enemy groups
	for (auto&& ally: allies) {
		for (auto&& enemy: enemies) {
			if (3 * enemy.count <= 2 * ally.count) {
                score += (opsFactor * enemyProximityWeight * enemy.count) / distance(enemy, ally);
            }
			else if (3 * ally.count <= 2 * enemy.count) {
                score += -(opsFactor * enemyProximityWeight * ally.count) / distance(enemy, ally);
            }
		}
	}

	return score;
}

/*std::shared_ptr<std::vector<std::shared_ptr<GameState>>> GameState::getChildren(){
    std::shared_ptr<std::vector<std::shared_ptr<GameState>>> results;

    for (int i = 0; i < allies.size(); i++){
        const int resultSize = results->size();
        std::vector<Move> possibleMoves = possibleEvolution(std::make_shared<Group>(allies[i]));

        // particular case "no one moved yet" (so we have to move). "this" is the root of it.
        for (int k = 0; k < possibleMoves.size(); k++){
            results->push_back(applyEvolution(*this, possibleMoves[k]));
        }

        // general case
        for (int j = 0; j < resultSize; j++){
            std::shared_ptr<GameState> currentState = results->at(j);
            for (int k = 0; k < possibleMoves.size(); k++){
                results->push_back(applyEvolution(*currentState, possibleMoves[k]));
            }
        }
    }

    return results;
}*/

std::vector<Move> GameState::possibleEvolution(std::shared_ptr<Group> group){

    //find the available directions (not all directions are possible if the group is on a side of the map)
    std::vector<Direction> availableMoves;
    if (group->x == 1){
        if (group->y == 1){
            availableMoves = { Right, Up, UpRight};
        }
        else if (group->y == n){
            availableMoves = { Right, Down, DownRight};
        }
        else{
            availableMoves = { Right, Up, Down, UpRight, DownRight };
        }
    }
    else if (group->x == m){
        if (group->y == 1){
            availableMoves = { Left, Up, UpLeft };
        }
        else if (group->y == m){
            availableMoves = { Left, Down, DownLeft };
        }
        else{
            availableMoves = { Left, Up, Down,UpLeft, DownLeft };
        }
    }
    else{
        if (group->y == 1){
            availableMoves = { Right, Left, Up, UpRight, UpLeft};
        }
        else if (group->y == m){
            availableMoves = { Right, Left, Down, DownRight, DownLeft };
        }
        else{
            availableMoves = { Right, Left, Up, Down, UpRight, UpLeft, DownRight, DownLeft };
        }
    }


}

int GameState::distance(const Group& group1, const Group& group2) {
	return std::max(std::abs(group2.x - group1.x), std::abs(group2.y - group1.y));
}

void GameState::print(){
    std::cout << std::endl;

    for (int y = GameState::m-1; y >= 0; y--) {
        for (int x = 0; x < GameState::n; x++) {

            std::cout << "\t";
            bool empty = true;

            for (Group group : allies) {
                if (group.x == x && group.y == y) {
                    std::cout << "A ";
                    std::cout << std::to_string(group.count) << " ";
                    empty = false;
                    break;
                }
            }

            if (!empty){
                for (Group group : enemies) {
                    if (group.x == x && group.y == y) {
                        std::cout << "E ";
                        std::cout << std::to_string(group.count) << " ";
                        empty = false;
                        break;
                    }
                }
            }

            if (empty) {
                std::cout << ".";
            }
            //std::cout << "\t";
        }
        std::cout << std::endl;
    }
}

int GameState::racePopulation(std::vector<Group> race)
{
    int count = 0;
    for (Group group : race)
    {
        count += group.count;
    }
    return count;
}
