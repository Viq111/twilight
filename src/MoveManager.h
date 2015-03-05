#ifndef INCLUDE_MOVEMANAGER
#define INCLUDE_MOVEMANAGER

#include <memory>
#include <boost/thread.hpp>
#include <GameSocket.h>

#include "Node.h"
class WorkerThread;


class MoveManager
{
public:
	MoveManager(int nbCores = 0); // If nbCores = 0, try to autodetect number of cores
	MoveManager(std::shared_ptr<Node> root, int nbCores = 0);

	void benchmark(); // Benchmark the current hardware
	void mainloop(std::unique_ptr<GameSocket> socket); // The life of the move manager is tied to the life of the socket

	// Used by each worker thread
	bool stillWork(); // Return true is the worker thread should work
	float timeRemaining(); // Time remaining before having to give the result or going to next phase
	std::shared_ptr<Node> getWork(); // Get a work item

	// Callback for the server
	std::shared_ptr<Node> callback(std::shared_ptr<Node>);
protected:
	unsigned int cores;
	std::unique_ptr<GameSocket> socket;
	std::shared_ptr<Node> root;
	std::vector<std::unique_ptr<WorkerThread>> threads;
	void _initThreads(); // Init all threads
	void _stopThreads();
	void _joinThreads();
};

// Each thread will call the MinMax algorithm on a particular node
class WorkerThread
{
public:
	WorkerThread(std::string thread_name, MoveManager* parent);
	bool running;
	std::string name;
	void start();
	void stop();		// Stop working
	void join();		// Join the thread
	void shortWait();
protected:
	void _launchLoop();
	void run();
	bool _stopping;
	MoveManager* parent;
	std::unique_ptr<boost::thread> thread;
};
#endif