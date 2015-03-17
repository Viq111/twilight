#include "GameSocket.h"
#include <iostream>
#include "standardFunctions.h"

GameSocket::GameSocket(std::string ip, int port, std::string name)
{
    this->ip = ip;
    this->port = port;
    this->name = name;
}

void GameSocket::connect()
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
    boost::system::error_code error;

    socket = make_unique<boost::asio::ip::tcp::socket>(io_service);
    socket->connect(endpoint, error);

    if (error)
    {
        std::cerr << "[FATAL] Couldn't connect to remote server" << std::endl;
        close();
    }
    else
    {
        std::cout << "[INFO] Connection to remote server succeeded" << std::endl;
        sendName();
        receive();
    }
}

void GameSocket::stop()
{
    receive();
}

void GameSocket::sendName()
{
    std::cout << "NME" << std::endl;
    std::cout << name << std::endl;
    std::cout << std::endl;

    strncpy(sendBuffer, "NME", 3);   // The code "NME" uses the first three bytes
    sendBuffer[3] = (char)name.size();   // The size of the name uses the 4th byte
    strncpy(sendBuffer + 4, name.c_str(), name.size());   // We copy the name after the 4th byte in the buffer

    socket->send(boost::asio::buffer(sendBuffer, name.size() + 4));
}

void GameSocket::sendOk()
{
    std::cout << "OK!" << std::endl;
    std::cout << std::endl;

    strncpy(sendBuffer, "OK!", 3);
    socket->send(boost::asio::buffer(sendBuffer, 3));
}

void GameSocket::sendError()
{
    std::cout << "ERR" << std::endl;
    std::cout << std::endl;

    strncpy(sendBuffer, "ERR", 3);
    socket->send(boost::asio::buffer(sendBuffer, 3));
}

void GameSocket::receive()
{
    socket->async_receive(boost::asio::buffer(receiveBuffer, 3), boost::bind(&GameSocket::handler_receive_code, this, boost::asio::placeholders::error));
    io_service.run();
}

void GameSocket::handler_receive_code(const boost::system::error_code& error)
{
    if (!error)
    {
        // Get 3 letters code from receive buffer
        std::string code(receiveBuffer, receiveBuffer + 3);
        std::cout << code << std::endl;

        if (code == "OK!")
        {
            receive();
        }
        else if (code == "GO!")
        {
            socket->async_receive(boost::asio::buffer(receiveBuffer + 3, 2), boost::bind(&GameSocket::handler_receive_data, this, boost::asio::placeholders::error, code, 4));
        }
        else if (code == "STP")
        {
            stop();
        }
    }
    else {
        std::cout << "Reception failed !" << std::endl;
    }
}

void GameSocket::handler_receive_data(const boost::system::error_code& error, const std::string& code, const int size)
{
    if (!error)
    {
        if (code == "GO!")
        {
            int port = receiveBuffer[4] + (256 * receiveBuffer[5]);
            std::cout << "Port: " << port << std::endl;
            std::string commandline = "/usr/bin/python ai_pylight.py --ip " + ip + " --port " + std::to_string(port);
            system(commandline.c_str());
            sendOk();
        }
        receive();
    }
    else {
        std::cerr << "[FATAL] Reception failed !" << std::endl;
    }
}

void GameSocket::close()
{
    socket->close();
}