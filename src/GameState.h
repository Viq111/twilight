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

class GameState {
public:
    static char n;
    static char m;
    static void setBoardSize(char n, char m);
    
    std::vector<Group> allies;
    const std::vector<Group> humans;
    std::vector<Group> enemies;
    
    const char alliesCount;
    const char humansCount;
    const char enemiescount;
    
    GameState(std::vector<Group> allies,
              std::vector<Group> humans,
              std::vector<Group> enemies);
    
    void moveGroup (char index, Direction dir, char count);
    
    std::vector<std::shared_ptr<GameState>> getChildren();
    
    void print();

private:
    void tryDirection(std::shared_ptr<GameState> head, Direction dir, char index, std::vector<std::shared_ptr<GameState>> children, char popMax);
    void getChildrenRecursive(char index, GameState head, std::vector<std::shared_ptr<GameState>> children);
};

#endif
