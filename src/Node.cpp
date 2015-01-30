#include "Node.h"
#include <stdlib.h>
#include <algorithm>

Node::Node(int parentScore)
{
    score = parentScore + rand() % 100 - 50;
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

bool comparaisonInverse(Node* first, Node* second)
{
    return (first->getScore() > second->getScore());
}

bool comparaison(Node* first, Node* second)
{
    return (first->getScore() < second->getScore());
}

std::vector<Node*> Node::getSortedChildren()
{
    std::sort(children.begin(), children.end(), comparaison);
    return children;
}

std::vector<Node*> Node::getReverseSortedChildren()
{
    std::sort(children.begin(), children.end(), comparaisonInverse);
    return children;
}