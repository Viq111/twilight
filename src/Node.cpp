#include "Node.h"
#include <stdlib.h>
#include <algorithm>

int Node::currentSyracusNumber = 14;                 //temp
int Node::currentClassScore = currentSyracusNumber;  //temp

Node::Node(){
    childNotGet = true;
    scoreNotGet = true;
}

Node::Node(GameState game)
{
	// ToDo: Implement
	childNotGet = true;
	scoreNotGet = true;
}

int Node::getScore(){
    if (scoreNotGet){
        scoreNotGet = false;

        //temp : cr�ation de score moins couteuse que rand(), mais un suffisament al�atoire pour nous (j'ai utilis� un truc bas� sur la conjecture de Syracuse et commencant arbitrairement � 14 et qui passe au suivant avant de boucler...)
        if (currentClassScore == 1){
            currentClassScore = currentSyracusNumber;
        }
        else if (currentClassScore % 2 == 1){
            currentClassScore = (3 * currentClassScore) + 1;
        }
        else{
            currentClassScore = currentClassScore / 2;
        }
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