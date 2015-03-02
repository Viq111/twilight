#include "MinMax.h"

MinMax::MinMax()
{
    srand(time(NULL));
    std::cout << "begin... " << std::endl;
    std::shared_ptr<Node> testNode = testTree(6, 10, 3);    // cree un arbre de profondeur 6, chaque noeuds non-feuille ayant 10 a 12 fils
    int* count = new int(0);
    (*count) = 0;                           // compte le nombre de noeuds parcourus (interessant a comparer avec le nombre total de noeuds pour mesurer l'efficacite de alpha-beta)
    std::cout << "max value sur profondeur 1 : " << maxValue(testNode, -100000000, 100000000, 1, count) << std::endl;
    std::cout << "Nombre de noeuds parcourus : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value sur profondeur 2 : " << maxValue(testNode, -100000000, 100000000, 2, count) << std::endl;
    std::cout << "Nombre de noeuds parcourus : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value sur profondeur 3 : " << maxValue(testNode, -100000000, 100000000, 3, count) << std::endl;
    std::cout << "Nombre de noeuds parcourus : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value sur profondeur 4 : " << maxValue(testNode, -100000000, 100000000, 4, count) << std::endl;
    std::cout << "Nombre de noeuds parcourus : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value sur profondeur 5 : " << maxValue(testNode, -100000000, 100000000, 5, count) << std::endl;
    std::cout << "Nombre de noeuds parcourus : " << *count << std::endl;
    (*count) = 0;
    std::cout << "max value sur profondeur 6 : " << maxValue(testNode, -100000000, 100000000, 6, count) << std::endl;
    std::cout << "Nombre de noeuds parcourus : " << *count << std::endl;
    system("PAUSE");
}

// cree un arbre de test
std::shared_ptr<Node> MinMax::testTree(int depth, int nbChildMin, int nbChildRange){
    int nbNodes = 1;
    std::shared_ptr<Node> root = std::make_shared<Node>(); //new Node(0);
    std::vector<std::shared_ptr<Node>> leaves;
    leaves.push_back(root);
    for (int i = 0; i < depth; i++){
        std::vector<std::shared_ptr<Node>> newLeaves;
        for (int j = 0; j < leaves.size(); j++){
            int nbChild = nbChildMin + rand() % nbChildRange;
            for (int k = 0; k < nbChild; k++){
                std::shared_ptr<Node> newLeaf = std::make_shared<Node>();  //new Node(leaves[j]->getScore());
                leaves[j]->addChild(newLeaf);
                newLeaves.push_back(newLeaf);
                nbNodes++;
            }
        }
        leaves = newLeaves;
    }
    std::cout << "Nombre de noeuds de l'arbre : " << nbNodes << std::endl;
    return root;
}

//fonction alpha-beta recursive simple : maxValue (cf. Chapitre 4 : Jeux, slide 65)
int MinMax::maxValue(std::shared_ptr<Node> current, int alpha, int beta, int depth, int* count){
    (*count)++;
    if ((depth <= 0) || (current->getChildren().size() == 0)){  //(current->getReverseSortedChildren().size()  //on est arrive au bas de l'arbre, sur "une feuille" => scoring
        return current->getScore();
    }
    std::vector<std::shared_ptr<Node>> children = current->getChildren();  //getReverseSortedChildren()
    for (int i = 0; i < children.size(); i++){
        alpha = std::max(alpha, minValue(children[i], alpha, beta, depth - 1, count));
        if (alpha >= beta){
            return beta;
        }
    }
    return alpha;
}

//fonction alpha-beta recursive simple : minValue (cf. Chapitre 4 : Jeux, slide 66)
int MinMax::minValue(std::shared_ptr<Node> current, int alpha, int beta, int depth, int* count){
    (*count)++;
    if ((depth <= 0) || (current->getChildren().size() == 0)){  //current->getSortedChildren().size()  //on est arrive au bas de l'arbre, sur "une feuille" => scoring
        return current->getScore();
    }
    std::vector<std::shared_ptr<Node>> children = current->getChildren();  //getSortedChildren()
    for (int i = 0; i < children.size(); i++){
        beta = std::min(beta, maxValue(children[i], alpha, beta, depth - 1, count));
        if (alpha >= beta){
            return alpha;
        }
    }
    return beta;
}
