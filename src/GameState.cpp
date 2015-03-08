#include "GameState.h"
#include "standardFunctions.h"
#include <math.h>

// Default board size
int GameState::n = 10;
int GameState::m = 10;
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
    int score = -(opsFactor * allies.size());

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
        std::vector<Move> possibleMoves = possibleEvolutions(std::make_shared<Group>(allies[i]));

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

std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>> GameState::possibleEvolutions(std::shared_ptr<Group> group){
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>> possibleMoves = std::make_shared<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>>();

    //find the available directions (not all directions are possible if the group is on a side of the map)
    std::vector<Direction> availableDirections;
    if (group->x == 1){
        if (group->y == 1){
            availableDirections = { Right, Up, UpRight};
        }
        else if (group->y == n){
            availableDirections = { Right, Down, DownRight};
        }
        else{
            availableDirections = { Right, Up, Down, UpRight, DownRight };
        }
    }
    else if (group->x == m){
        if (group->y == 1){
            availableDirections = { Left, Up, UpLeft };
        }
        else if (group->y == m){
            availableDirections = { Left, Down, DownLeft };
        }
        else{
            availableDirections = { Left, Up, Down,UpLeft, DownLeft };
        }
    }
    else{
        if (group->y == 1){
            availableDirections = { Right, Left, Up, UpRight, UpLeft};
        }
        else if (group->y == m){
            availableDirections = { Right, Left, Down, DownRight, DownLeft };
        }
        else{
            availableDirections = { Right, Left, Up, Down, UpRight, UpLeft, DownRight, DownLeft };
        }
    }

    /*//implementation 1 : a group should not split in more than two sub-groups
        //all the group moves
    for (int i = 0; i < availableDirections.size(); i++){
        std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMove = std::make_shared<std::vector<std::shared_ptr<Move>>>();
        std::shared_ptr<Move> theMove = std::make_shared<Move>();
        theMove->count = group->count;
        theMove->dir = availableDirections[i];
        currentMove->push_back(theMove);
        possibleMoves->push_back(currentMove);
    }

        //all the moves with a split in two different sized moving groups
    for (int i = 1; i <= ((group->count - 1) / 2); i++){
        for (int j = 0; j < availableDirections.size(); j++){
            for (int k = 0; k < availableDirections.size(); k++){

                if (j != k){
                    std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMoves = std::make_shared<std::vector<std::shared_ptr<Move>>>();
                    std::shared_ptr<Move> firstMove = std::make_shared<Move>();
                    firstMove->count = i;
                    firstMove->dir = availableDirections[j];
                    currentMoves->push_back(firstMove);
                    std::shared_ptr<Move> secondMove = std::make_shared<Move>();
                    secondMove->count = (group->count - i);
                    secondMove->dir = availableDirections[k];
                    currentMoves->push_back(secondMove);
                    possibleMoves->push_back(currentMoves);
                }
            }
        }
    }
        //add possibility no moves for one of the two groups
    for (int i = 1; i < group->count; i++){
        for (int j = 0; j < availableDirections.size(); j++){

            std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMove = std::make_shared<std::vector<std::shared_ptr<Move>>>();
            std::shared_ptr<Move> theMove = std::make_shared<Move>();
            theMove->count = i;
            theMove->dir = availableDirections[j];
            currentMove->push_back(theMove);
            possibleMoves->push_back(currentMove);
        }
    }
        //all the split equally moves if there is an even number of people in the group
    if (((group->count % 2) == 0) && (group->count > 1)){
        int subGroupCount = (group->count) / 2;
        for (int i = 0; i < availableDirections.size() - 1; i++){
            for (int j = i; j < availableDirections.size(); j++){
                if (i != j){
                    std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMoves = std::make_shared<std::vector<std::shared_ptr<Move>>>();
                    std::shared_ptr<Move> firstMove = std::make_shared<Move>();
                    firstMove->count = subGroupCount;
                    firstMove->dir = availableDirections[i];
                    currentMoves->push_back(firstMove);
                    std::shared_ptr<Move> secondMove = std::make_shared<Move>();
                    secondMove->count = subGroupCount;
                    secondMove->dir = availableDirections[j];
                    currentMoves->push_back(secondMove);
                    possibleMoves->push_back(currentMoves);
                }
            }
        }
    }
    *///end of implementation 1
    
    //implementation 2 : a group should not split in more than two sub-groups
        //determine the minimum interresting group size
    int minGroupSize;
    if (enemies.size() > 0){
        minGroupSize = ceil(1.5*(enemies[0].count));
        //std::cout << "minGroupSize : " << minGroupSize << std::endl;
        for (int i = 1; i < enemies.size(); i++){
            if (minGroupSize > ceil(1.5*(enemies[i].count))){
                minGroupSize = ceil(1.5*(enemies[i].count));
            }
        }
    }
    //std::cout << "minGroupSize : " << minGroupSize << std::endl;
    for (int i = 0; i < humans.size(); i++){
        if (minGroupSize > humans[i].count){
            minGroupSize = humans[i].count;
        }
    }
    //std::cout << "minGroupSize : " << minGroupSize << std::endl;
        //all the group moves : same as implementation 1
    for (int i = 0; i < availableDirections.size(); i++){
        std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMove = std::make_shared<std::vector<std::shared_ptr<Move>>>();
        std::shared_ptr<Move> theMove = std::make_shared<Move>();
        theMove->count = group->count;
        theMove->dir = availableDirections[i];
        currentMove->push_back(theMove);
        possibleMoves->push_back(currentMove);
    }

        //all the moves with a split in two different sized big enough moving groups
    for (int i = minGroupSize; i <= ((group->count - 1) / 2); i++){
        for (int j = 0; j < availableDirections.size(); j++){
            for (int k = 0; k < availableDirections.size(); k++){
                if (j != k){
                    std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMoves = std::make_shared<std::vector<std::shared_ptr<Move>>>();
                    std::shared_ptr<Move> firstMove = std::make_shared<Move>();
                    firstMove->count = i;
                    firstMove->dir = availableDirections[j];
                    currentMoves->push_back(firstMove);
                    std::shared_ptr<Move> secondMove = std::make_shared<Move>();
                    secondMove->count = (group->count - i);
                    secondMove->dir = availableDirections[k];
                    currentMoves->push_back(secondMove);
                    possibleMoves->push_back(currentMoves);
                }
            }
        }
    }
        //add possibility no moves for one of the two groups if big enough
    for (int i = minGroupSize; i <= (group->count - minGroupSize); i++){
        for (int j = 0; j < availableDirections.size(); j++){

            std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMove = std::make_shared<std::vector<std::shared_ptr<Move>>>();
            std::shared_ptr<Move> theMove = std::make_shared<Move>();
            theMove->count = i;
            theMove->dir = availableDirections[j];
            currentMove->push_back(theMove);
            possibleMoves->push_back(currentMove);
        }
    }
        //all the split equally moves if there is an even number of people in the group and if the subgroups would be big enough
    if (((group->count % 2) == 0) && (group->count >= (2 * minGroupSize))){
        int subGroupCount = (group->count) / 2;
        for (int i = 0; i < availableDirections.size() - 1; i++){
            for (int j = i; j < availableDirections.size(); j++){
                if (i != j){
                    std::shared_ptr<std::vector<std::shared_ptr<Move>>> currentMoves = std::make_shared<std::vector<std::shared_ptr<Move>>>();
                    std::shared_ptr<Move> firstMove = std::make_shared<Move>();
                    firstMove->count = subGroupCount;
                    firstMove->dir = availableDirections[i];
                    currentMoves->push_back(firstMove);
                    std::shared_ptr<Move> secondMove = std::make_shared<Move>();
                    secondMove->count = subGroupCount;
                    secondMove->dir = availableDirections[j];
                    currentMoves->push_back(secondMove);
                    possibleMoves->push_back(currentMoves);
                }
            }
        }
    }
    //end of implementation 2
    
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

GameState* GameState::applyGroupEvolutions (std::vector<std::shared_ptr<GroupEvolution>> evolutions, GameState* intial)
{
    GameState* updatedState = new GameState(*intial);

    updatedState->operationOfGeneration = evolutions;

    for (std::shared_ptr<GroupEvolution> gEvol : evolutions) {
        for (Move move : gEvol->moves) {
            int deltaX, deltaY;
            switch (move.dir) {
                case Right : deltaX = +1; deltaY = 0; break;
                case Left : deltaX = -1; deltaY = 0; break;
                case Up : deltaX = 0; deltaY = +1; break;
                case Down : deltaX = 0; deltaY = -1; break;
                case UpRight : deltaX = +1; deltaY = +1; break;
                case UpLeft : deltaX = -1; deltaY = +1; break;
                case DownRight : deltaX = +1; deltaY = -1; break;
                case DownLeft : deltaX = -1; deltaY = -1; break;
            }

            Group group = gEvol->group;

            int newX = group.x + deltaX;
            int newY = group.y + deltaY;

            group.x = newX;
            group.y = newY;
        }   
    }
    resolve(updatedState);
    return updatedState;
}

std::vector<std::shared_ptr<GroupEvolution>> GameState::getOperationOfGeneration()
{
    return operationOfGeneration;
};

void GameState::resolve(GameState* state)
{
    for (Group group : allies)
    {
        int x = group.x;
        int y = group.y;

        for (Group ally : allies)
        {
            if (ally.x == x && ally.y == y)
            {
                // TODO regroupement
            }
        }
        for (Group enemy : enemies)
        {
            if (enemy.x == x && enemy.y == y)
            {
                // TODO battle
            }
        }
        for (Group human : humans)
        {
            if (human.x == x && human.y == y)
            {
                // TODO meal
            }
        }
    }
}

