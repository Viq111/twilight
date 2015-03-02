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
		MinMax();
protected:
    int minValue(std::shared_ptr<Node> current, int alpha, int beta, int depth, int* count);
    int maxValue(std::shared_ptr<Node> current, int alpha, int beta, int depth, int* count);
    std::shared_ptr<Node> testTree(int depth, int nbChildMin, int nbChildRange);
};

#endif