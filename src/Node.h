#ifndef INCLUDE_NODE
#define INCLUDE_NODE

#include <vector>
#include <memory>
#include "GameState.h"

class Node
{
    std::vector<std::shared_ptr<Node>> children;
    int score;
    int alpha;
    int beta;
    bool childNotGet;                   //true tant que l'on a pas r�cup�r� ses enfants
    bool scoreNotGet;                   //true tant que l'on a pas r�cup�r� son score
    GameState gameState;
public:
    Node();                             //only for benchmark
    Node(GameState game);
    Node(std::shared_ptr<GameState> game);
    int addChild(std::shared_ptr<Node> child);
    std::vector<std::shared_ptr<Node>> getChildren(bool itsAlliesTurn);
    std::vector<std::shared_ptr<Node>> getSortedChildren();
    std::vector<std::shared_ptr<Node>> getReverseSortedChildren();
    int getScore();
    int getAlpha() { return alpha; };
    int getBeta(){ return beta; };
    GameState getGameState(){ return gameState; };
};

#endif