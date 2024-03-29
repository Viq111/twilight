#ifndef INCLUDE_MOVEMANAGER
#define INCLUDE_MOVEMANAGER

#include <memory>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <GameSocket.h>
#include <queue>

#include "Node.h"
class WorkerThread;


// Thread-safe queue
// See https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
// Adjusted to work better with our code

template<typename Data>
class concurrent_queue
{
private:
	std::queue<Data> the_queue;
	mutable boost::mutex the_mutex;
	boost::condition_variable the_condition_variable;
public:
	void push(Data const& data)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		the_queue.push(data);
		lock.unlock();
		the_condition_variable.notify_one();
	}

	bool empty() const
	{
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.empty();
	}

	bool try_pop(Data& popped_value)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		if (the_queue.empty())
		{
			return false;
		}

		popped_value = the_queue.front();
		the_queue.pop();
		return true;
	}

	Data wait_and_pop(bool& should_return)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		while ((the_queue.empty()) && (!(should_return)))
		{
			the_condition_variable.wait(lock);
		}
		if (should_return)
		{
			return nullptr;
		}
		Data popped_value = the_queue.front();
		the_queue.pop();
		return popped_value;
	}
	void free_all() // Free all threads
	{
		the_condition_variable.notify_all();
	}

};


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
	std::shared_ptr<Node> getWork(bool&); // Get a work item, non-blocking. If there is no work, the nullptr is sent

	// Callback for the server
	std::shared_ptr<Node> callback(std::shared_ptr<Node>);
protected:
	unsigned int cores;
	std::unique_ptr<GameSocket> socket;
	std::shared_ptr<Node> root;
	std::vector<std::unique_ptr<WorkerThread>> threads;
	concurrent_queue<std::shared_ptr<Node>> workQueue;
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