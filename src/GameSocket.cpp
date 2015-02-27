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
    std::cout << "Hello" << socket.get() << std::endl;
    socket->connect(endpoint, error);
    if (error)
    {
        this->socket->close();
        std::cerr << "[FATAL] Couldn't connect to remote server";
    }
    else
    {
        this->socket->send(boost::asio::buffer("NME" + myName));
        setup();
        std::cout << "[INFO] Connection to remote server succeeded";
    }
}

void GameSocket::setup()
{
//    this->socket.async_receive(boost::asio::buffer(receiveBuffer, 128), boost::bind(&GameSocket::handler_receive, this, boost::asio::placeholders::error));
}

void GameSocket::handler_receive(const boost::system::error_code& error)
{
    if (!error)
    {
        // Get 3 letters code from receive buffer
        std::string code(receiveBuffer, receiveBuffer + 2);
        std::cout << code << std::endl;
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
