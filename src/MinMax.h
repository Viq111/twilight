#ifndef INCLUDE_MINMAX
#define INCLUDE_MINMAX

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <queue>
#include <stdlib.h>
#include <time.h>
#include "Node.h"
#include <memory>

class MinMax
{
public:
	MinMax(); //to test MinMax on random trees
    static int testNodePointersDesalocation(int nbIterations);
    static int proceedMinMax(std::shared_ptr<Node> current, int depth, bool beginWithMin);
    static int minValue(std::shared_ptr<Node> current, int alpha, int beta, int depth);
    static int maxValue(std::shared_ptr<Node> current, int alpha, int beta, int depth);
protected:
    static std::shared_ptr<Node> testTree(int depth, int nbChildMin, int nbChildRange, std::shared_ptr<Node> rootNode);
};

#endif