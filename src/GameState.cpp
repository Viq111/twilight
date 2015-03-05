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

std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>> GameState::possibleEvolution(std::shared_ptr<Group> group){
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>> possibleMoves = std::make_shared<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>>();

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

    //implementation 1 : a group should not split in more than two sub-groups
        //all the group moves
    for (int i = 0; i < availableMoves.size(); i++){
        std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMove = std::make_shared<std::vector<std::shared_ptr<Move>>>();
        std::shared_ptr<Move> theMove = std::make_shared<Move>();
        theMove->count = group->count;
        theMove->dir = availableMoves[i];
        currentMove->push_back(theMove);
        possibleMoves->push_back(currentMove);
    }

        //all the moves with a split in two different sized moving groups
    for (int i = 0; i < ((group->count - 1) / 2); i++){
        for (int j = 0; j < availableMoves.size(); j++){
            for (int k = 0; k < availableMoves.size(); k++){
                if (j != k){
                    std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMoves = std::make_shared<std::vector<std::shared_ptr<Move>>>();
                    std::shared_ptr<Move> firstMove = std::make_shared<Move>();
                    firstMove->count = i;
                    firstMove->dir = availableMoves[j];
                    currentMoves->push_back(firstMove);
                    std::shared_ptr<Move> secondMove = std::make_shared<Move>();
                    secondMove->count = (group->count - i);
                    secondMove->dir = availableMoves[k];
                    currentMoves->push_back(secondMove);
                    possibleMoves->push_back(currentMoves);
                }
            }
        }
    }
        //add possibility no moves for one of the two groups
    for (int i = 0; i < group->count; i++){
        for (int j = 0; j < availableMoves.size(); j++){
            std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMove = std::make_shared<std::vector<std::shared_ptr<Move>>>();
            std::shared_ptr<Move> theMove = std::make_shared<Move>();
            theMove->count = i;
            theMove->dir = availableMoves[j];
            currentMove->push_back(theMove);
            possibleMoves->push_back(currentMove);
        }
    }
        //all the split equally moves if there is an even number of people in the group (if done in the general case, it will be done two times, because of the case symmetry)
    if ((group->count % 2) == 0){
        int subGroupCount = (group->count) / 2;
        for (int i = 0; i < availableMoves.size() - 1; i++){
            for (int j = i; j < availableMoves.size(); j++){
                if (i != j){
                    std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMoves = std::make_shared<std::vector<std::shared_ptr<Move>>>();
                    std::shared_ptr<Move> firstMove = std::make_shared<Move>();
                    firstMove->count = subGroupCount;
                    firstMove->dir = availableMoves[i];
                    currentMoves->push_back(firstMove);
                    std::shared_ptr<Move> secondMove = std::make_shared<Move>();
                    secondMove->count = subGroupCount;
                    secondMove->dir = availableMoves[j];
                    currentMoves->push_back(secondMove);
                    possibleMoves->push_back(currentMoves);
                }
            }
        }
    }
    //end of implementation 1

    return possibleMoves;
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
