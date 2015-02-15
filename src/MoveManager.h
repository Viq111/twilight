#ifndef INCLUDE_MOVEMANAGER
#define INCLUDE_MOVEMANAGER

#include <memory>
#include <boost/thread.hpp>

class Socket // ToDo: Include real socket class
{
public:
	Socket();
};

#include "Node.h"
class MinMaxAlgorithm;	// ToDo: Inlude real algorithm


class MoveManager
{
public:
	MoveManager(std::shared_ptr<Node> root, int nbCores);
	void setSocket(std::unique_ptr<Socket> socket); // Set socket to be able to talk to it
	void benchmark(); // Benchmark the current hardware
	void mainloop();

	// Used by each worker thread
	bool stillWork(); // Return true is the worker thread should worker
	float timeRemaining(); // Time remaining before having to give the result or going to next phase
	std::shared_ptr<Node> getWork(); // Get a work item
protected:
	unsigned int cores;
	std::unique_ptr<Socket> socket;
	std::shared_ptr<Node> root;
};

// Each thread will call the MinMax algorithm on a particular node
class WorkerThread
{
public:
	WorkerThread(MoveManager parent);
	void stop();		// Stop working
	void join();		// Join the thread
protected:
	void run();
	boost::thread thread;
};
#endif