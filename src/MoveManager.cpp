#include "MoveManager.h"
#include <thread>
#include <boost/chrono.hpp>
#include "standardFunctions.h"

int __getNbCores()
{
	unsigned int nbCores = std::thread::hardware_concurrency();
	if (nbCores == 0)	// Couldn't determine the number of threads, use 2 by default
	{
		std::cerr << "[WARNING] Cannot find the real number of cores, using default 2" << std::endl;
		nbCores = 2;
	}
	return nbCores;
}

MoveManager::MoveManager(int n) : socket(nullptr)
{
	if (n != 0)
	{
		cores = n;
	}
	else
	{
		cores = __getNbCores();
	}
}

MoveManager::MoveManager(std::shared_ptr<Node> r, int n) : socket(nullptr), root(r)
{
	if (n != 0)
	{
		cores = n;
	}
	else
	{
		cores = __getNbCores();
	}
}

void MoveManager::_initThreads()
{
	// Create all threads
	for (int i = 0; i < cores; i++)
	{
		std::string name = "Thread " + std::to_string(i);
		// We need :: before make_unique because of argument-dependent lookup (see http://stackoverflow.com/questions/28521822/c11-14-make-unique-ambigious-overload-for-stdstring)
		threads.push_back(::make_unique<WorkerThread>(name, this));
	}
	// Start them
	for (int i = 0; i < cores; i++)
	{
		threads.at(i)->start();
	}
}
void MoveManager::_stopThreads()
{
	for (int i = 0; i < cores; i++)
	{
		threads.at(i)->stop();
	}
	workQueue.free_all();
}
void MoveManager::_joinThreads()
{
	for (int i = 0; i < cores; i++)
	{
		threads.at(i)->join();
	}
}

void MoveManager::benchmark()
{
	std::cout << "[MM] Starting threads..." << std::endl;
	_initThreads();
	std::cout << "[MM] Starting benchmark..." << std::endl;
	auto start_time = boost::chrono::system_clock::now();
	// ToDo: Add items to queue and wait for it to finish
	boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));

	// Stop and join threads
	_stopThreads();
	_joinThreads();
	auto stop_time = boost::chrono::system_clock::now();
	auto diff_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(stop_time - start_time);
	std::cout << "[MM] Benchmark completed!" << std::endl;
	boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	std::cout << "[MM] The total test took " << static_cast<double>(diff_time.count()) / 1000 << "s" << std::endl;
}

void MoveManager::mainloop(std::unique_ptr<GameSocket> s)
{
	socket = std::move(s);
	std::cout << "[MM] Starting threads..." << std::endl;
	_initThreads();
	// ToDo: Do work
	boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));
	_stopThreads();
	_joinThreads();

}


bool MoveManager::stillWork() // Still work to do ?
{
	return !(workQueue.empty());
}
std::shared_ptr<Node> MoveManager::getWork(bool& stopping)
{
	std::shared_ptr<Node> result;
	if (workQueue.wait_and_pop(stopping))
	{
		return result;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<Node> MoveManager::callback(std::shared_ptr<Node> initialGameState)
{
	// ToDo: Implement
	std::cout << "Server has a new GameState!" << std::endl;
	return initialGameState;
}


// Worker thread part
WorkerThread::WorkerThread(std::string n, MoveManager* p) : name(n), parent(p), running(false), _stopping(false)
{
}

void WorkerThread::start()
{
	_stopping = false;
	thread = ::make_unique<boost::thread>(boost::bind(&WorkerThread::_launchLoop, this));
}
void WorkerThread::stop()
{
	_stopping = true;
}
void WorkerThread::join()
{
	while (running) { shortWait(); }
	thread->join();
}
void WorkerThread::shortWait()
{
	boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
}

void WorkerThread::_launchLoop()
{
	running = true;
	std::string start_string = "[MM] Thread " + name + " started !\n";
	std::cout << start_string;
	run();
	running = false;
	std::string stop_string = "[MM] Thread " + name + " ended !\n";
	std::cout << stop_string;
}

void WorkerThread::run()
{
	// This function will try to find and do a job while the thread is running
	while (!_stopping)
	{
		std::shared_ptr<Node> work = parent->getWork(_stopping);
		if (_stopping)
		{
			return;
		}
		// ToDo: Do something!
	}
}