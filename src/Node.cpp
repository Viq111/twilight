#include "Node.h"
#include <stdlib.h>
#include <algorithm>

int Node::currentSyracusNumber = 14;
int Node::currentClassScore = currentSyracusNumber;  //temp

Node::Node(int parentScore)  //temp
{
    childNotGet = true;
    scoreNotGet = true;
    score = parentScore + rand() % 100 - 50;
}

Node::Node(){
    childNotGet = true;
    scoreNotGet = true;
}

int Node::getScore(){  //temp : création de score moins couteuse que rand(), mais un suffisament aléatoire pour nous (j'ai utilisé un truc basé sur la conjecture de Syracuse et commencant arbitrairement à 14 et qui passe au suivant avant de boucler...)
    if (scoreNotGet){
        scoreNotGet = false;
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

int Node::addChildren(Node* child)
{
    children.push_back(child);
    return 0;
}

Node::~Node()
{
    if (children.size() > 0){
        for (int i = 0; i < children.size(); i++){
            delete(children[i]);
        }
    }
}

bool comparaisonInverse(Node* first, Node* second)  //temp
{
    return (first->getScore() > second->getScore());
}

bool comparaison(Node* first, Node* second)  //temp
{
    return (first->getScore() < second->getScore());
}

std::vector<Node*> Node::getChildren()
{
    if (childNotGet){
        childNotGet = false;
        // To add : on récupère les enfants ici et on les ajoutent à children
    }
    return children;
}

std::vector<Node*> Node::getSortedChildren()  //temp
{
    std::sort(children.begin(), children.end(), comparaison);
    return children;
}

std::vector<Node*> Node::getReverseSortedChildren()  //temp
{
    std::sort(children.begin(), children.end(), comparaisonInverse);
    return children;
}