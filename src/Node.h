#ifndef INCLUDE_NODE
#define INCLUDE_NODE

#include <vector>
#include "GameState.h"

class Node
{
    std::vector<Node*> children;
    int score;
    int alpha;
    int beta;
    bool childNotGet;                   //true tant que l'on a pas récupéré ses enfants
    bool scoreNotGet;                   //true tant que l'on a pas récupéré son score
    static int currentClassScore;       //provisoir pour des tests (tant qu'on a pas un implemente un calcul du score avec l'AS)
    static int currentSyracusNumber;    //provisoir pour des tests (tant qu'on a pas un implemente un calcul du score avec l'AS)
public:
    Node();
	Node(GameState);
    ~Node();
    int addChild(Node* child);
    std::vector<Node*> getChildren();
    std::vector<Node*> getSortedChildren();
    std::vector<Node*> getReverseSortedChildren();
    int getScore();
    int getAlpha() { return alpha; };
    int getBeta(){ return beta; };
};

#endif