#include "GameSocket.h"
#include <iostream>
#include "standardFunctions.h"

GameSocket::GameSocket(PlayCallback c) : myName("Edward"), callback(c) {}

void GameSocket::connect(std::string ip, int port, std::string name)
{
	// ToDo: Connect to the server and set the name
	// Create a thread to listen for answer and fire callback
	// When fiering callback, the return Node is the move
	// Compare to old Node and check where we moved
	// If callback is nullptr, do nothing (or maybe print)
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
    socket = make_unique<boost::asio::ip::tcp::socket>(io_service);

    boost::system::error_code error;
    socket->connect(endpoint, error);
    if (error)
    {
        this->socket->close();
        std::cerr << "[FATAL] Couldn't connect to remote server";
    }
    else
    {
        char* data = new char[myName.size() + 4];
        strncpy(data, "NME", 3);
        data[3] = (char)myName.size();
        strncpy(data + 4, myName.c_str(), myName.size());
        std::cout << data << std::endl;
        this->socket->send(boost::asio::buffer(data, myName.size() + 4));
        std::cout << "[INFO] Connection to remote server succeeded" << std::endl;
        setup();
    }
}

void GameSocket::setup()
{
    socket->async_receive(boost::asio::buffer(receiveBuffer, 4), boost::bind(&GameSocket::handler_receive, this, boost::asio::placeholders::error));
    io_service.run();
}

void GameSocket::handler_receive(const boost::system::error_code& error)
{
    if (!error)
    {
        // Get 3 letters code from receive buffer
        std::string code(receiveBuffer, receiveBuffer + 3);
        std::cout << code << std::endl;
        setup();
    }
    else {
        std::cout << "Reception failed !" << std::endl;
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
