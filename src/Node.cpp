#include "Node.h"
#include <stdlib.h>
#include <algorithm>


Node::Node(){
    childNotGet = true;
    scoreNotGet = true;
}

Node::Node(std::shared_ptr<GameState> game)
{
    gameState = game;
	childNotGet = true;
	scoreNotGet = true;
}

int Node::getScore(){
    if (scoreNotGet){
        scoreNotGet = false;
        gameState->getScore();
    }
    return score; 
}

int Node::addChild(std::shared_ptr<Node> child)
{
    children.push_back(child);
    return 0;
}

bool comparaisonInverse(std::shared_ptr<Node> first, std::shared_ptr<Node> second)  //temp
{
    return (first->getScore() > second->getScore());
}

bool comparaison(std::shared_ptr<Node> first, std::shared_ptr<Node> second)  //temp
{
    return (first->getScore() < second->getScore());
}

std::vector<std::shared_ptr<Node>> Node::getChildren()
{
    if (childNotGet){
        childNotGet = false;
        // To do : on r�cup�re les enfants ici et on les ajoutent � children
    }
    return children;
}

std::vector<std::shared_ptr<Node>> Node::getSortedChildren()  //temp
{
    std::sort(children.begin(), children.end(), comparaison);
    return children;
}

std::vector<std::shared_ptr<Node>> Node::getReverseSortedChildren()  //temp
{
    std::sort(children.begin(), children.end(), comparaisonInverse);
    return children;
}