#pragma once
#include <vector>

class Node
{
    std::vector<Node*> children;
    int score;
    int alpha;
    int beta;
public:
    Node(int parentScore);
    ~Node();
    int addChildren(Node* child);
    std::vector<Node*> getSortedChildren();
    std::vector<Node*> getReverseSortedChildren();
    int getScore(){ return score; };
    int getAlpha() { return alpha; };
    int getBeta(){ return beta; };
};

