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

int Node::addChild(std::shared_ptr<Node> child)
{
    children.push_back(child);
    childNotSorted = true;
    return 0;
}

bool comparaison(std::shared_ptr<Node> first, std::shared_ptr<Node> second)
{
    return (first->getScore() < second->getScore());
}

std::vector<std::shared_ptr<Node>> Node::getChildren(bool itsAlliesTurn)
{
    if (childNotGet){
        childNotGet = false;
        std::shared_ptr<std::vector<std::shared_ptr<GameState>>> gameStateChildren = gameState.getChildren(itsAlliesTurn);
        for (int i = 0; i < gameStateChildren->size(); i++){
            children.push_back(std::make_shared<Node>(gameStateChildren->at(i)));
        }
    }
    return children;
}

std::vector<std::shared_ptr<Node>> Node::getSortedChildren(bool itsAlliesTurn)
{
    if (childNotGet){
        childNotGet = false;
        std::shared_ptr<std::vector<std::shared_ptr<GameState>>> gameStateChildren = gameState.getChildren(itsAlliesTurn);
        for (int i = 0; i < gameStateChildren->size(); i++){
            children.push_back(std::make_shared<Node>(gameStateChildren->at(i)));
        }
    }
    if (childNotSorted){
        std::sort(children.begin(), children.end(), comparaison);
    }
    return children;
}
