#ifndef INCLUDE_NODE
#define INCLUDE_NODE

#include <vector>
#include <memory>
#include "GameState.h"

class Node
{
    std::vector<std::shared_ptr<Node>> children;
    int score;
    bool childNotGet;                   //true tant que l'on a pas récupéré ses enfants
    bool scoreNotGet;                   //true tant que l'on a pas récupéré son score
    bool childNotSorted;                //true tant que l'on a pas trié les enfants
    GameState gameState;
public:
    Node();                             //only for benchmark
    Node(GameState& game);
    Node(std::shared_ptr<GameState> game);
    int addChild(std::shared_ptr<Node> child);
    int getScore();
    std::vector<std::shared_ptr<Node>> getChildren(bool itsAlliesTurn); //return moves with Nodes. Maybe : std::vector<std::pair<std::shared_ptr<Node>, std::vector<std::shared_ptr<Move>>>>
    std::vector<std::shared_ptr<Node>> getSortedChildren(bool itsAlliesTurn);
    GameState& getGameState();
};

#endif