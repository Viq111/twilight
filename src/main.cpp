#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include "GameState.h"
#include "GameSocket.h"
#include "MoveManager.h"
#include "standardFunctions.h" // For make_unique
#include "MinMax.h"

int getNbCores()
{
	unsigned int nbCores = std::thread::hardware_concurrency();
	if (nbCores == 0)	// Couldn't determine the number of threads, use 2 by default
	{
		std::cerr << "[WARNING] Cannot find the real number of cores, using default 2" << std::endl;
		nbCores = 2;
	}
	return nbCores;
}

// tests for GameState
void testGameState();
int testGameStatePossibleEvolution();
int testGameStateWorkflow();

int main(int argc, char* argv[])
{

	// Parse the arguments
	std::vector<std::string> params(argv, argv + argc);

	// Get cores
	int nbCores = getNbCores();

	// If params is benchmark, do it
	if (((params.size() == 2) && params[1] == "benchmark") || params.size() == 1)
	{
		std::cout << "Benchmarking ..." << std::endl;
		GameState::setBoardSize(10, 15); // Set default board size
		std::shared_ptr<Node> root = std::make_shared<Node>(); // Empty Node
		MoveManager mm(root, nbCores);
		mm.benchmark();
		return 0;
	}
/*	if (((params.size() == 2) && params[1] == "testGameState"))
	{
		testGameState();
		return 0;
	}*/
	// Check number of arguments
	if (params.size() != 3)
	{
		std::cout << "Usage: " << params.at(0) << " <ip> <port>" << std::endl;
		return -1;
	}
	// Number of arguments seems good, get ip and port
	std::string ip = params.at(1);
	int port = std::stoi(params.at(2));

	// First create a connetion to the server
	std::unique_ptr<GameSocket> socket = make_unique<GameSocket>();
	socket->connect(ip, port, "Edward");

	// Create the MoveManager with the root node
	MoveManager mm(nbCores);
	mm.setSocket(std::move(socket));

	mm.mainloop();
	return 0;
}

/*
void testGameState()
{
    GameState::n = 10;
    GameState::m = 10;

    auto allies = std::vector<Group>();
    auto humans = std::vector<Group>();
    auto enemies = std::vector<Group>();

    std::cout << "* Deux exemples en 1d (la 2d fonctionne), montrant ce qui marche et ce qui déconne. *" << std::endl;

    std::cout << "* Premier exemple où cela fonctionne *" << std::endl;

    Group initialGroup1 = {3, 2, 10}; // {x, y, pop}
    Group initialGroup2 = {1, 9, 10}; // {x, y, pop}
    Group initialGroup3 = {3, 9, 10}; // {x, y, pop}
    allies.push_back(initialGroup1);
    allies.push_back(initialGroup2);
    allies.push_back(initialGroup3);
    GameState* initial = new GameState(allies, humans, enemies);

    std::cout << "Situation de départ : " << std::endl;
    initial->print();


    std::cout << "Enfants : " << std::endl;

    clock_t t;
    t = clock();
    auto children = initial->getChildren();
    t = clock() - t;
    //    for (GameState* child : *children) {
    //        child->print();
    //    }
    std::cout << "nb d'enfants : " << children->size() << std::endl;
    std::cout << "temps en ms : " << (((float)t)*1000)/CLOCKS_PER_SEC << std::endl;
}
*/

int testGameStatePossibleEvolution(){

    GameState* test = new GameState();
    std::shared_ptr<Group> g = std::make_shared<Group>();
    g->x = 5;
    g->y = 5;
    std::vector<Group> humans;
    for (int i = 1; i < 5; i++){        //min = 5
        Group h;
        h.x = 5;
        h.y = 5;
        h.count = (2 * i) + 1;
        humans.push_back(h);
    }
    std::vector<Group> ennemies;
    for (int i = 2; i < 5; i++){        //min = 5, ceil(1.5*min) = 8
        Group e;
        e.x = 5;
        e.y = 5;
        e.count = (2 * i) + 1;
        ennemies.push_back(e);
    }
    test->enemies = ennemies;
    test->humans = humans;

    std::cout << "before" << std::endl;
    for (int i = 2; i < 10; i++){
        g->count = i;
        std::cout << "for " << g->count << " persons in the group, there are " << (test->possibleEvolutions(g))->size() << " possible moves." << std::endl;
    }
    std::cout << "after" << std::endl;
    system("PAUSE");

    return 0;
}

int testGameStateWorkflow(){

    GameState::n = 5;
    GameState::m = 2;

    std::vector<Group> humans, enemies, allies;
    Group e, a1, a2, h;

    /*
    **  after initialization:
    **  |..|..|A2|..|..|
    **  |..|E1|A1|H |..|
    */

    e.x = 1;
    e.y = 0;
    e.count = 1;
    enemies.push_back(e);

    a1.x = 2;
    a1.y = 0;
    a1.count = 1;
    allies.push_back(a1);

    a2.x = 2;
    a2.y = 1;
    a2.count = 2;
    allies.push_back(a2);

    h.x = 3;
    h.y = 0;
    h.count = 1;
    humans.push_back(h);



    GameState* gState = new GameState(allies, humans, enemies);
    std::cout << gState->humans[0].count << std::endl;
    gState->print();

    return 0;
}
