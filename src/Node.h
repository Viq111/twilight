#ifndef INCLUDE_NODE
#define INCLUDE_NODE

#include <vector>
#include <memory>
#include <utility>
#include "GameState.h"

class Node
{
    std::vector<std::pair<std::shared_ptr<Node>, std::vector<Move>>> children;
    int score;
    bool childNotGet;                   //true tant que l'on a pas r�cup�r� ses enfants
    bool scoreNotGet;                   //true tant que l'on a pas r�cup�r� son score
    bool childNotSorted;                //true tant que l'on a pas tri� les enfants
    GameState gameState;
public:
    Node();                             //only for benchmark
    Node(GameState& game);
    Node(std::shared_ptr<GameState> game);
    int addChild(std::shared_ptr<Node> child, std::vector<Move> moves);
    int getScore();
    std::vector<std::pair<std::shared_ptr<Node>, std::vector<Move>>>& getChildren(bool itsAlliesTurn); //return moves with Nodes. Maybe : std::vector<std::pair<std::shared_ptr<Node>, std::vector<std::shared_ptr<Move>>>>
    std::vector<std::pair<std::shared_ptr<Node>, std::vector<Move>>>& getSortedChildren(bool itsAlliesTurn);
    GameState& getGameState();
};

#endif