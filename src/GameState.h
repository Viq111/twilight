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
    Move() {};
    Move(int x, int y, Direction dir, int count):x(x), y(y), dir(dir), count(count) {};
    void print(); // for debugging purposes only
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

    std::vector<Group>& getAllies();
    std::vector<Group>& getHumans();
    std::vector<Group>& getEnemies();
    void setAllies(std::vector<Group>& allies);
    void setHumans(std::vector<Group>& humans);
    void setEnemies(std::vector<Group>& enemies);

protected:
    // Lists of groups of each race
    std::vector<Group> allies;
    std::vector<Group> humans;
    std::vector<Group> enemies;

    std::vector<std::vector<Move>> possibleEvolutions(Group& group);
    std::vector<std::vector<Move>> possibleEvolutionsOfRace(std::vector<Group> race);

    // Needs the evolutions to be ordered by groups, based on the order of the groups of the gameState
    std::shared_ptr<GameState> applyEvolutions(bool itsAlliesTurn, std::vector<Move> &evolutions);
    void resolve();

    static int distance(const Group& group1, const Group& group2);
};

#endif