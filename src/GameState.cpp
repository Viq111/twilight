#include "GameState.h"
#include "standardFunctions.h"

// Default board size
int GameState::n = 0;
int GameState::m = 0;
int GameState::opsFactor = 60;

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

void GameState::moveGroup (int index, Direction dir, int count)
{
    Group group = allies[index];
    //allies.erase(allies.begin()+index);
    allies[index].count -= count;
    int x = group.x;
    int y = group.y;
    switch (dir) {
        case Right: x += 1; break;
        case Left:  x -= 1; break;
        case Up:    y += 1; break;
        case Down:  y -= 1; break;
        case UpRight:   x += 1; y += 1; break;
        case UpLeft:    x -= 1; y += 1; break;
        case DownRight: x += 1; y -= 1; break;
        case DownLeft:  x -= 1; y -= 1; break;
    }
    Group group2 = {x, y, count};
    allies.push_back(group2);

    //if (count < group.count) {
    //    group.count -= count;
    //    allies.push_back(group);
    //}
}


void GameState::tryDirection(std::shared_ptr<GameState> head, Direction dir, int index, std::shared_ptr<std::vector<std::shared_ptr<GameState>>> children, int popMax){
    for (int pop = 1; pop <= popMax; pop++) {
        std::shared_ptr<GameState> state = std::make_shared<GameState>(*head);
        state->moveGroup(index, dir, pop);
        if(index){
            getChildrenRecursive(index-1, state, children);
        } else {
            children->push_back(state);
        }
    }
}


void GameState::getChildrenRecursive(int index, std::shared_ptr<GameState> head, std::shared_ptr<std::vector<std::shared_ptr<GameState>>> children)
{
    // la variable index correspondant au groupe d'indice le plus grand qui n'a pas encore bougé
    // on regarde donc les possibilités de ce groupe et on itère récursivement avec un index plus petit
    Group g = allies[index];
    int pop = g.count;
    bool right =    (g.x == GameState::n -1);
    bool left =     (g.x == 0);
    bool top =      (g.y == GameState::m -1);
    bool bottom =   (g.y == 0);
    
    // cas où on choisit de ne pas bouger le groupe
    if (index) {
        getChildrenRecursive(index-1, head, children);
    } else {
        children->push_back(head);
    }

    if (!right) {
        // si le groupe n'est pas à l'extrème droite de la grille, on gère les cas où un certain nombre vont à droite :
        tryDirection(head, Right, index, children, pop);
        if (!bottom) {
            tryDirection(head, DownRight, index, children, pop);
        }
        if (!top) {
            tryDirection(head, UpRight, index, children, pop);
        }
    }
    if (!left) {
        tryDirection(head, Left, index, children, pop);
        if (!bottom) {
            tryDirection(head, DownLeft, index, children, pop);
        }
        if (!top) {
            tryDirection(head, UpLeft, index, children, pop);
        }
    }
    if (!top) {
        tryDirection(head, Up, index, children, pop);
    }
    if (!bottom) {
        tryDirection(head, Down, index, children, pop);
    }
}

std::shared_ptr<std::vector<std::shared_ptr<GameState>>> GameState::getChildren() {
    auto children = std::make_shared<std::vector<std::shared_ptr<GameState>>>();
    getChildrenRecursive(allies.size()-1, std::shared_ptr<GameState>(this), children);
    return children;
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
                }
            }

            for (Group group : enemies) {
                if (group.x == x && group.y == y) {
                    std::cout << "E ";
                    std::cout << std::to_string(group.count) << " ";
                    empty = false;
                }
            }

            if (empty) {
                std::cout << "  .  ";
            }
            //std::cout << "\t";
        }
        std::cout << std::endl;
    }
}

int GameState::racePopulation(std::vector<Group> race)
{
    int count;
    for (Group group : race)
    {
        count += group.count;
    }
    return count;
}
