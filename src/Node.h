#pragma once
#include <vector>

class Node
{
    std::vector<Node*> children;
    int score;
    int alpha;
    int beta;
    bool childNotGet;
    bool scoreNotGet;
    static int currentClassScore;       //provisoir pour des tests (tant qu'on a pas un impl�ment� un calcul du score avec l'AS)
    static int currentSyracusNumber;       //provisoir pour des tests (tant qu'on a pas un impl�ment� un calcul du score avec l'AS)
public:
    Node(int parentScore);              //provisoir pour des tests (tant qu'on a pas un impl�ment� un calcul du score avec l'AS)
    Node();
    ~Node();
    int addChildren(Node* child);
    std::vector<Node*> getChildren();
    std::vector<Node*> getSortedChildren();         //provisoir pour tester � quel point utiliser des listes tri�es permettaient de faire plus de coupes alpha-beta et donc de parcourir moins de noeuds. Apr�s quelques tests, ca ne vaut pas le coup.
    std::vector<Node*> getReverseSortedChildren();  //provisoir pour tester � quel point utiliser des listes tri�es permettaient de faire plus de coupes alpha-beta et donc de parcourir moins de noeuds. Apr�s quelques tests, ca ne vaut pas le coup.
    int getScore();
    int getAlpha() { return alpha; };
    int getBeta(){ return beta; };
};

