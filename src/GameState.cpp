#include "GameState.h"
#include "standardFunctions.h"
#include <math.h>
#include <map>
#include <utility>

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

GameState::GameState(){} // for benchmarking purpose

GameState::GameState(
	std::vector<Group> allies,
	std::vector<Group> humans,
	std::vector<Group> enemies)
	: allies(allies),
	  humans(humans),
	  enemies(enemies)
{
    if (!GameState::n || !GameState::m) {
        throw "GameState_board_size_not_initialized";
    }
}

// TODO: add coefficients (by machine learning or manual benchmarking)
int GameState::getScore() {

    int alliesCount = racePopulation(allies);
    int enemiesCount = racePopulation(enemies);

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

std::shared_ptr<std::vector<std::shared_ptr<GameState>>> GameState::getChildren(bool itsAlliesTurn)
{
    std::shared_ptr<std::vector<std::shared_ptr<GameState>>> children;

    std::vector<Group> currentRace = itsAlliesTurn ? allies : enemies;
    std::vector<std::vector<std::shared_ptr<GroupEvolution>>> pEvol = possibleEvolutions(currentRace);
    for (std::vector<std::shared_ptr<GroupEvolution>> raceEvol : pEvol)
    {
        children->push_back(applyGroupEvolutions(raceEvol, this));
    }
    return children;
}


std::vector<std::shared_ptr<GroupEvolution>> convert(Group group, std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>> thing)
{
    auto result = std::vector<std::shared_ptr<GroupEvolution>>();

    for (std::shared_ptr<std::vector<std::shared_ptr<Move>>> movesThing : *thing)
    {
        std::shared_ptr<GroupEvolution> cur = std::make_shared<GroupEvolution>(GroupEvolution());
        cur->group = group;
        std::vector<Move> moves;
        for (std::shared_ptr<Move> move : *movesThing)
        {
            moves.push_back(*move);
        }
        cur->moves = moves;
        result.push_back(cur);
    }

    return result;
}

std::vector<std::vector<std::shared_ptr<GroupEvolution>>> GameState::possibleEvolutions(std::vector<Group> race)
{
    std::vector<std::vector<std::shared_ptr<GroupEvolution>>> results;

    for (int i = 0; i < race.size(); i++){
        const int resultSize = results.size();
        auto pEvolutions = possibleEvolutions(std::make_shared<Group>(race[i]));

        // particular case "no one moved yet" (so we have to move). "this" is the root of it.
        for (int k = 0; k < pEvolutions->size(); k++){
            //results->push_back(applyEvolutions(*this, (*pEvolutions)[k])));
            std::vector<std::shared_ptr<GroupEvolution>> newOne = {convert(race[i],pEvolutions)[k]};
            results.push_back(newOne);
        }

        // general case
        for (int j = 0; j < resultSize; j++){
            std::vector<std::shared_ptr<GroupEvolution>> root = results.at(j);
            for (int k = 0; k < pEvolutions->size(); k++){
                //results->push_back(applyEvolutions(*currentState, (*pEvolutions)[k])));
                root.push_back(convert(race[i],pEvolutions)[k]);
                results.push_back(root);
            }
        }
    }

    return results;
}

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

            if (empty){
                for (Group group : enemies) {
                    if (group.x == x && group.y == y) {
                        std::cout << "E ";
                        std::cout << std::to_string(group.count) << " ";
                        empty = false;
                        break;
                    }
                }

                if (empty){
                    for (Group group : humans) {
                        if (group.x == x && group.y == y) {
                            std::cout << "H ";
                            std::cout << std::to_string(group.count) << " ";
                            empty = false;
                            break;
                        }
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

std::shared_ptr<GameState> GameState::applyGroupEvolutions(std::vector<std::shared_ptr<GroupEvolution>> evolutions, GameState* intial)
{
    std::shared_ptr<GameState> updatedState = std::make_shared<GameState>(*intial);//create a shared_ptr to a copy of inital GameState

    updatedState->operationOfGeneration = evolutions;
    updatedState->arrivalPositions = std::vector<bool>(n*(m+1),false); 

    for (std::shared_ptr<GroupEvolution> gEvol : evolutions) {
        for (Move move : gEvol->moves) {
            Group group = gEvol->group;
            // la groupEvolution n'est appliquée que si le depart n'est pas une arrivée
            if (!arrivalPositions[gEvol->group.x + n*gEvol->group.y]){
                switch (move.dir) {
                    case Right : gEvol->group.x++; break;
                    case Left: gEvol->group.x--; break;
                    case Up: gEvol->group.y++; break;
                    case Down: gEvol->group.y--; break;
                    case UpRight: gEvol->group.x++; gEvol->group.y++; break;
                    case UpLeft: gEvol->group.x--; gEvol->group.y++; break;
                    case DownRight: gEvol->group.x++; gEvol->group.y--; break;
                    case DownLeft: gEvol->group.x--; gEvol->group.y--; break;
                }
                arrivalPositions[gEvol->group.x + n*gEvol->group.y] = true;
            }
        }   
    }
    updatedState->resolve();
    return updatedState;
}

std::vector<std::shared_ptr<GroupEvolution>> GameState::getOperationOfGeneration()
{
    return operationOfGeneration;
};

void GameState::resolve()
{
    //allies fusion   
    for (int i = 0; i < allies.size(); i++)
    {
        for (int j = (i + 1); j < allies.size(); j++)
        {
            if (allies[i].x == allies[j].x && allies[i].y == allies[j].y)
            {
                allies[i].count += allies[j].count;
                allies.erase(allies.begin() + j);
                j--;
            }
        }
    }
    //enemies fusion
    for (int i = 0; i < enemies.size(); i++)
    {
        for (int j = (i + 1); j < enemies.size(); j++){
            if (enemies[i].x == enemies[j].x && enemies[i].y == enemies[j].y){
                enemies[i].count += enemies[j].count;
                enemies.erase(enemies.begin() + j);
                j--;
            }
        }
    }
    //allies-enemies battles
    for (int i = 0; i < allies.size(); i++){
        for (int j = 0; j < enemies.size(); j++){
            if (allies[i].x == enemies[j].x && allies[i].y == enemies[j].y){
                if (allies[i].count == enemies[j].count){
                    //the two groups disapear
                    allies.erase(allies.begin() + i);
                    i--;
                    enemies.erase(enemies.begin() + j);
                    j--;
                }
                else if (allies[i].count >= (1.5 * enemies[j].count)){
                    //allies automaticly wined :) !!
                    enemies.erase(enemies.begin() + j);
                    j--;
                }
                else if (enemies[j].count >= (1.5 * allies[i].count)){
                    //enemy automaticly wined :( ...
                    allies.erase(allies.begin() + i);
                    i--;
                }
                else {
                    //the allies are more likely to win
                    int averageSurvivorsMultByTot = ((allies[i].count + 1) * allies[i].count) - ((enemies[j].count + 1) * enemies[j].count);
                    if (averageSurvivorsMultByTot > 0){
                        //allies wined :) !!
                        allies[i].count = (averageSurvivorsMultByTot / (allies[i].count + enemies[j].count));
                        enemies.erase(enemies.begin() + j);
                        j--;
                    }
                    else if (averageSurvivorsMultByTot < 0){
                        //enemy wined :( ... That is only an hypothetic case, anyway...
                        enemies[j].count = -(averageSurvivorsMultByTot / (allies[i].count + enemies[j].count));
                        allies.erase(allies.begin() + i);
                        i--;
                    }
                    else{
                        //it shouldn't happend
                        //the two groups disapear, as if they had the same number of people
                        allies.erase(allies.begin() + i);
                        i--;
                        enemies.erase(enemies.begin() + j);
                        j--;
                    }
                }
            }
        }
    }

    //humans battles...
    for (int i = 0; i < humans.size(); i++){
        //...against allies
        for (int j = 0; j < allies.size(); j++){
            if (humans[i].x == allies[j].x && humans[i].y == allies[j].y){
                if (allies[j].count >= humans[i].count){
                    allies[j].count += humans[i].count;
                    humans.erase(humans.begin() + i);
                    i--;
                }
                else{
                    int averageSurvivorsMultByTot = ((humans[i].count + 1) * humans[i].count) - ((allies[j].count + 1) * allies[j].count);
                    humans[i].count = averageSurvivorsMultByTot / (humans[i].count + allies[j].count);
                    allies.erase(allies.begin() + j);
                    j--;
                }
            }
        }
        //... and against enemies
        for (int j = 0; j < enemies.size(); j++){
            if (humans[i].x == enemies[j].x && humans[i].y == enemies[j].y){
                if (enemies[j].count >= humans[i].count){
                    enemies[j].count += humans[i].count;
                    humans.erase(humans.begin() + i);
                    i--;
                }
                else{
                    int averageSurvivorsMultByTot = ((humans[i].count + 1) * humans[i].count) - ((enemies[j].count + 1) * enemies[j].count);
                    humans[i].count = averageSurvivorsMultByTot / (humans[i].count + enemies[j].count);
                    enemies.erase(enemies.begin() + j);
                    j--;
                }
            }
        }
    }

    //A more efficient, but more complex code for the beginning of this function.
    //It may be used later to enhance this function
    /*
    std::vector<std::vector<std::pair<int, Group *>>> allGroups(n*m);
    std::vector<int> globalConflictualPositions;

    //record allied conflicts
    std::vector<Group *> newAllies;
    std::vector<int> alliesConflictualPositions;
    std::vector<std::vector<std::pair<int, Group *>>> alliesGroups(n*m);
    for (int i = 0; i < allies.size(); i++){
    int vectorPosistion = allies[i].x + n*allies[i].y;
    if (alliesGroups[vectorPosistion].size() == 1){
    //there is already someone there and this is the first time we see it => this is a NEW conflict
    alliesConflictualPositions.push_back(vectorPosistion);
    }
    alliesGroups[vectorPosistion].push_back(std::make_pair(i, &(allies[i])));
    }
    //resolve allies conflicts
    for (int i = 0; i < alliesConflictualPositions.size(); i++){
    std::vector<std::pair<int, Group *>> toMerge = alliesGroups[alliesConflictualPositions[i]];
    Group* newGroup = toMerge[0].second;
    for (int j = 1; j < toMerge.size(); j++){
    newGroup->count += toMerge[j].second->count;
    }
    newAllies.push_back(newGroup);
    allGroups[toMerge[0].first].push_back(toMerge[0]);
    }
    //add those whithout conflict
    for (int i = 0; i < allies.size(); i++){
    if (alliesGroups[allies[i].x + n*allies[i].y].size() == 1){
    newAllies.push_back(alliesGroups[allies[i].x + n*allies[i].y][0].second);
    allGroups[allies[i].x + n*allies[i].y].push_back(std::make_pair(allies[i].x + n*allies[i].y, &(allies[i])));
    }
    }

    //record enemis conflicts
    std::vector<Group *> newEnemis;
    std::vector<int> enemisConflictualPositions;
    std::vector<std::vector<std::pair<int, Group *>>> enemisGroups(n*m);
    for (int i = 0; i < enemies.size(); i++){
    int vectorPosistion = enemies[i].x + n*enemies[i].y;
    if (enemisGroups[vectorPosistion].size() == 1){
    //there is already someone there and this is the first time we see it => this is a NEW conflict
    enemisConflictualPositions.push_back(vectorPosistion);
    }
    enemisGroups[vectorPosistion].push_back(std::make_pair(i, &(enemies[i])));
    }
    //resolve enemis conflicts and record some globalConflicts
    for (int i = 0; i < enemisConflictualPositions.size(); i++){
    std::vector<std::pair<int, Group *>> toMerge = enemisGroups[enemisConflictualPositions[i]];
    Group* newGroup = toMerge[0].second;
    for (int j = 1; j < toMerge.size(); j++){
    newGroup->count += toMerge[j].second->count;
    }
    newEnemis.push_back(newGroup);
    if (allGroups[toMerge[0].first].size() == 1){
    globalConflictualPositions.push_back(toMerge[0].first);
    }
    allGroups[toMerge[0].first].push_back(toMerge[0]);
    }
    //add those whithout conflict and record the other allied-enemis globalConflicts, only conflicts with humans will remain
    for (int i = 0; i < enemies.size(); i++){
    if (enemisGroups[enemies[i].x + n*enemies[i].y].size() == 1){
    newEnemis.push_back(enemisGroups[enemies[i].x + n*enemies[i].y][0].second);
    if (allGroups[enemies[i].x + n*enemies[i].y].size() == 1){
    globalConflictualPositions.push_back(enemies[i].x + n*enemies[i].y);
    }
    allGroups[enemies[i].x + n*enemies[i].y].push_back(std::make_pair(enemies[i].x + n*enemies[i].y, &(enemies[i])));
    }
    }

    //resolve conflicts between allies and enemis

    for (int i = 0; i < globalConflictualPositions.size(); i++){
    std::vector<std::pair<int, Group *>> toMerge = allGroups[globalConflictualPositions[i]];
    Group* newGroup = toMerge[0].second;
    for (int j = 1; j < toMerge.size(); j++){
    newGroup->count += toMerge[j].second->count;
    }
    newAllies.push_back(newGroup);
    allGroups[toMerge[0].first].push_back(toMerge[0]);
    }
    */
}

