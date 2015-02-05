#ifndef INCLUDE_MINMAX
#define INCLUDE_MINMAX

#include "Node.h"

class MinMax
{
	public:
		MinMax();
protected:
	int minValue(Node* current, int alpha, int beta, int depth, int* count);
	int maxValue(Node* current, int alpha, int beta, int depth, int* count);
	Node* testTree(int depth, int nbChildMin, int nbChildRange);
};

#endif