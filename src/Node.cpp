#include "Node.h"
#include <stdlib.h>
#include <algorithm>

Node::Node(){
    childNotGet = true;
    scoreNotGet = true;
    childNotSorted = true;
}

Node::Node(GameState& game){
    gameState = GameState(game);
    childNotGet = true;
    scoreNotGet = true;
    childNotSorted = true;
}

Node::Node(std::shared_ptr<GameState> game)
{
    gameState = GameState(*game);
	childNotGet = true;
	scoreNotGet = true;
    childNotSorted = true;
}

GameState& Node::getGameState(){
    return gameState;
}

int Node::getScore(){
    if (scoreNotGet){
        scoreNotGet = false;
        score = gameState.getScore();
    }
    return score; 
}

int Node::addChild(std::shared_ptr<Node> child, std::vector<Move> moves)
{
    children.push_back(std::pair<std::shared_ptr<Node>, std::vector<Move>>(child, moves));
    childNotSorted = true;
    return 0;
}

bool comparaison(std::pair<std::shared_ptr<Node>, std::vector<Move>> child1, std::pair<std::shared_ptr<Node>, std::vector<Move>> child2)
{
    return (child1.first->getScore() < child2.first->getScore());
}

std::vector<std::pair<std::shared_ptr<Node>, std::vector<Move>>>& Node::getChildren(bool itsAlliesTurn)
{
    if (childNotGet){
        childNotGet = false;
        std::vector<std::pair<std::shared_ptr<GameState>, std::vector<Move>>> gameStateAndMovesChildren = gameState.getChildren(itsAlliesTurn);
        for (int i = 0; i < gameStateAndMovesChildren.size(); i++){
            children.push_back(std::pair<std::shared_ptr<Node>, std::vector<Move>>(std::make_shared<Node>(gameStateAndMovesChildren[i].first), gameStateAndMovesChildren[i].second));
        }
    }
    return children;
}

std::vector<std::pair<std::shared_ptr<Node>, std::vector<Move>>>& Node::getSortedChildren(bool itsAlliesTurn)
{
    if (childNotGet){
        childNotGet = false;
        std::vector<std::pair<std::shared_ptr<GameState>, std::vector<Move>>> gameStateAndMovesChildren = gameState.getChildren(itsAlliesTurn);
        for (int i = 0; i < gameStateAndMovesChildren.size(); i++){
            children.push_back(std::pair<std::shared_ptr<Node>, std::vector<Move>>(std::make_shared<Node>(gameStateAndMovesChildren[i].first), gameStateAndMovesChildren[i].second));
        }
    }
    if (childNotSorted){
        std::sort(children.begin(), children.end(), comparaison);
    }
    return children;
}