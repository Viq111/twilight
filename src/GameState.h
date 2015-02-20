//
//  GameState.h
//  dataStructures
//
//  Created by Damien VIELPEAU on 06/02/2015.
//  Copyright (c) 2015 Damien VIELPEAU. All rights reserved.
//

#ifndef INCLUDE_GAMESTATE
#define INCLUDE_GAMESTATE

#include <vector>
#include <memory>

// Represent a group of creature with its position and size
struct Group {
    char x;
    char y;
    char count;
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
    static unsigned int n;
	static unsigned int m;
    static void setBoardSize(unsigned int n, unsigned int m);

	// Constructor
	GameState(); // Create an empty game for Benchmarking
	GameState(	std::vector<Group> allies,
				std::vector<Group> humans,
				std::vector<Group> enemies);

	const unsigned int alliesCount;
	const unsigned int humansCount;
	const unsigned int enemiescount;
	std::vector<Group> allies;
    const std::vector<Group> humans;
    std::vector<Group> enemies;

    std::vector<std::unique_ptr<GameState>> getChildren();
private:
    void tryDirection(std::shared_ptr<GameState> head, Direction dir, char index, std::vector<std::shared_ptr<GameState>> children, char popMax);
    void getChildrenRecursive(char index, GameState head, std::vector<std::shared_ptr<GameState>> children);
	void moveGroup(char index, Direction dir, char count);
};

#endif
