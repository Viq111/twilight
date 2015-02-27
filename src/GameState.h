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

// Represent a group of creature with its position and size
struct Group {
    int x;
    int y;
    int count;
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

// Represent a current board
class GameState {
public:
	// Size is static because it should not change during the game
    static int n;
	static int m;
    // Multiplication factor used in all scoring operations
    static int opsFactor;
    static void setBoardSize(int n, int m);

	// Constructor
	GameState(); // Create an empty game for Benchmarking
	GameState(	std::vector<Group> allies,
				std::vector<Group> humans,
				std::vector<Group> enemies);

	const int alliesCount;
	const int humansCount;
	const int enemiesCount;
    // Utility method to sum the pop of groups
    static int racePopulation(std::vector<Group> race);
	std::vector<Group> allies;
    const std::vector<Group> humans;
    std::vector<Group> enemies;

    std::shared_ptr<std::vector<std::shared_ptr<GameState>>> getChildren();
    int getScore();
    void print();
private:
    void tryDirection(std::shared_ptr<GameState> head, Direction dir, int index, std::shared_ptr<std::vector<std::shared_ptr<GameState>>> children, int popMax);
    void getChildrenRecursive(int index, std::shared_ptr<GameState> head, std::shared_ptr<std::vector<std::shared_ptr<GameState>>> children);
	void moveGroup(int index, Direction dir, int count);
    int distance(const Group& group1, const Group& group2);
};

#endif
