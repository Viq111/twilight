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

// Represents a group of creatures
struct Group {
    int x;
    int y;
    int count;
};

// Represents a direction for a move
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

// Represents a move of some creatures
struct Move {
    int x;
    int y;
    Direction dir;
    int count;
};

// Represents a current board
class GameState {

public:
    // Size of game board
    static int n;
    static int m;
    static void setBoardSize(int n, int m);

    // Scoring factors
    static int opsFactor;
    static int populationCountsWeight;
    static int humanProximityWeight;
    static int enemyProximityWeight;

    // Constructors
    GameState();
    GameState(std::vector<Group> allies, std::vector<Group> humans, std::vector<Group> enemies);

    std::vector<std::pair<std::shared_ptr<GameState>, std::vector<Move>>> getChildren(bool itsAlliesTurn);
    int getScore();
    void print();   // Print object for debug purpose

protected:
    // Lists of groups of each race
    std::vector<Group> allies;
    std::vector<Group> humans;
    std::vector<Group> enemies;

    std::vector<std::vector<Move>> possibleEvolutions(Group& group);
    std::vector<std::vector<Move>> possibleEvolutions(std::vector<Group> race);
    std::shared_ptr<GameState> applyGroupEvolutions(std::vector<Move> evolutions, GameState* intial);
    void resolve();

    static int distance(const Group& group1, const Group& group2);
};

#endif