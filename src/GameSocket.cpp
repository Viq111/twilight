#include "GameSocket.h"
#include <iostream>

GameSocket::GameSocket(PlayCallback c) : myName("Edward"), callback(c) {}

void GameSocket::connect(std::string ip, int port, std::string name)
{
	// ToDo: Connect to the server and set the name
	// Create a thread to listen for answer and fire callback
	// When fiering callback, the return Node is the move
	// Compare to old Node and check where we moved
	// If callback is nullptr, do nothing (or maybe print)
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
    boost::asio::ip::tcp::socket socket(io_service);

    boost::system::error_code error;
    socket.connect(endpoint, error);
    if (error)
    {
        socket.close();
        std::cerr << "[FATAL] Couldn't connect to remote server";
    }
    else
    {
        std::cout << "[INFO] Connection to remote server succeeded";
    }
}

void GameSocket::setCallback(PlayCallback c)
{
	callback = c;
}

bool GameSocket::isAlive()
{
	// ToDo: Check if the thread or connection is still alive
	return true;
}

void GameSocket::join()
{
	// ToDo: Wait the thread
}

void GameSocket::close()
{
	// ToDo: Close the socket and stop the thread
}
