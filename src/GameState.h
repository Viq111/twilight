//
//  GameState.h
//  dataStructures
//
//  Created by Damien VIELPEAU on 06/02/2015.
//  Copyright (c) 2015 Damien VIELPEAU. All rights reserved.
//

#ifndef INCLUDE_GAMESTATE
#define INCLUDE_GAMESTATE

#include <cstdlib>
#include <vector>
#include <iostream>
#include <ctime>
#include <memory>
#include <algorithm>
#include <string>
#include <queue>

// Represent a group of creatures
struct Group {
    int x;
    int y;
    int count;
    bool hasAlreadyMoved;
};

enum Direction {
    Right,
    Left,
    Up,
    Down,
    UpRight,
    UpLeft,
    DownRight,
    DownLeft
};

struct Move {
    Direction dir;
    int count;
};

struct GroupEvolution {
    Group group;
    std::vector<Move> moves;
};

// Represent a current board
class GameState {

public:
	// Size is static because it should not change during the game
    static int n;
	static int m;
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>> possibleEvolution(std::shared_ptr<Group> group);
    // Multiplication factor used in all scoring operations
    static int opsFactor;

    // Weights of the different parts of the score
    static int populationCountsWeight;
    static int humanProximityWeight;
    static int enemyProximityWeight;

    static void setBoardSize(int n, int m);

	// Constructor
	GameState(); // Create an empty game for Benchmarking
	GameState(	std::vector<Group> allies,
				std::vector<Group> humans,
				std::vector<Group> enemies);

    GameState(std::vector<Group> allies,
        std::vector<Group> humans,
        std::vector<Group> enemies,
        int alliesCount,
        int humansCount,
        int enemiesCount);

    // Number of _groups_ for each race
	int alliesCount;
	int humansCount;
	int enemiesCount;

    // Listes of groups of each race
    std::vector<Group> allies;
    const std::vector<Group> humans;
    std::vector<Group> enemies;

    // Utility method to sum the populations of groups
    static int racePopulation(std::vector<Group> groupsOfRace);

    // Generate children for alpha_beta algo
    std::shared_ptr<std::vector<std::shared_ptr<GameState>>> getChildren();

    // How to access this state from his parents in alpha-beta tree
    std::vector<Move> accessiblesViaMoves;

    // Calculate score
    int getScore();

    // For visualisation/debug purpose
    void print();

private:
    // Useful to avoid impossible moves
    std::vector<std::vector<bool>> departurePositions;
    std::vector<std::vector<bool>> arrivalPositions;

    // Compute all the possible evolution of a given group
    // its a list of moves : for instance two guys go right, three go down
    // [WARNING] THE "NO ONE MOVES" POSSIBILITY MUST NOT BE CONSIDERED
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<Move>>>>> possibleEvolutions(std::shared_ptr<Group> group);
    

    // Apply a list of evolutions to a given copied GameState to create a new one
    GameState* applyGroupEvolutions(std::vector<std::shared_ptr<GroupEvolution>> evolutions, GameState* intial);

    // Utility method for the distance between 2 groups 
    int distance(const Group& group1, const Group& group2);
};

#endif
