#include "Node.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <queue>
#include <stdlib.h>
#include <time.h>

int minValue(Node* current, int alpha, int beta, int depth, int* count);
int maxValue(Node* current, int alpha, int beta, int depth, int* count);
Node* testTree(int depth, int nbChildMin, int nbChildRange);

int main(){
    srand(time(NULL));


    /*
    std::cout << "begin... " << std::endl;
    Node* testNode = testTree(6, 10, 3); // cree un arbre de profondeur 6, chaque noeuds non-feuille ayant 10 à 12 fils
    int* count = new int(0);
    (*count) = 0;
    std::cout << "max value 1 : " << maxValue(testNode, -100000000, 100000000, 1, count);
    std::cout << ", count : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value 2 : " << maxValue(testNode, -100000000, 100000000, 2, count);
    std::cout << ", count : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value 3 : " << maxValue(testNode, -100000000, 100000000, 3, count);
    std::cout << ", count : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value 4 : " << maxValue(testNode, -100000000, 100000000, 4, count);
    std::cout << ", count : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value 5 : " << maxValue(testNode, -100000000, 100000000, 5, count);
    std::cout << ", count : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value 6 : " << maxValue(testNode, -100000000, 100000000, 6, count);
    std::cout << ", count : " << *count << std::endl;
    system("PAUSE");
    */
    return 0;
}

// cree un arbre de test
Node* testTree(int depth, int nbChildMin, int nbChildRange){
    int nbNodes = 1;
    Node* root = new Node(0);
    std::vector<Node*> leaves;
    leaves.push_back(root);
    for (int i = 0; i < depth; i++){
        std::vector<Node*> newLeaves;
        for (int j = 0; j < leaves.size(); j++){
            int nbChild = nbChildMin + rand() % nbChildRange;
            for (int k = 0; k < nbChild; k++){
                Node* newLeaf = new Node(leaves[j]->getScore());
                leaves[j]->addChildren(newLeaf);
                newLeaves.push_back(newLeaf);
                nbNodes++;
            }
        }
        leaves = newLeaves;
    }
    return root;
}

//fonction alpha-beta récursive simple 1
int maxValue(Node* current, int alpha, int beta, int depth, int* count){
    (*count)++;
    if ((depth <= 0) || (current->getSortedChildren().size() == 0)){    //on est arrivé au bas de l'arbre, sur "une feuille" => scoring
        return current->getScore();
    }
    std::vector<Node*> children = current->getSortedChildren();
    for (int i = 0; i < children.size(); i++){
        alpha = std::max(alpha, minValue(children[i], alpha, beta, depth - 1, count));
        if (alpha >= beta){
            return beta;
        }
    }
    return alpha;
}

//fonction alpha-beta récursive simple 2
int minValue(Node* current, int alpha, int beta, int depth, int* count){
    (*count)++;
    if ((depth <= 0) || (current->getSortedChildren().size() == 0)){    //on est arrivé au bas de l'arbre, sur "une feuille" => scoring
        return current->getScore();
    }
    std::vector<Node*> children = current->getSortedChildren();
    for (int i = 0; i < children.size(); i++){
        beta = std::min(beta, maxValue(children[i], alpha, beta, depth - 1, count));
        if (alpha >= beta){
            return alpha;
        }
    }
    return beta;
}
