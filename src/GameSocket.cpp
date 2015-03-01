#include "GameSocket.h"
#include <iostream>
#include "standardFunctions.h"

GameSocket::GameSocket(PlayCallback c) : myName("Edward"), callback(c) {}

void GameSocket::connect(std::string ip, int port, std::string name)
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
        setup();
    }
}

void GameSocket::setup()
{
    sendName();
    receive();
}

void GameSocket::sendName()
{
    char* data = new char[myName.size() + 4];   // Sending buffer

    strncpy(data, "NME", 3);   // The code "NME" uses the first three bytes
    data[3] = (char)myName.size();   // The size of the name uses the 4th byte
    strncpy(data + 4, myName.c_str(), myName.size());   // We copy the name after the 4th byte in the buffer

    socket->send(boost::asio::buffer(data, myName.size() + 4));
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

        if (code == "SET")
        {
            socket->async_receive(boost::asio::buffer(receiveBuffer + 3, 2), boost::bind(&GameSocket::handler_receive_data, this, boost::asio::placeholders::error, code, 2));
        }
        else if (code == "HME")
        {
            socket->async_receive(boost::asio::buffer(receiveBuffer + 3, 2), boost::bind(&GameSocket::handler_receive_data, this, boost::asio::placeholders::error, code, 2));
        }
        else if (code == "END")
        {
            setup();
        }
        else if (code == "BYE")
        {
            close();
        }
        else
        {
            socket->async_receive(boost::asio::buffer(receiveBuffer + 3, 1), boost::bind(&GameSocket::handler_receive_size, this, boost::asio::placeholders::error, code));
        }
    }
    else {
        std::cout << "Reception failed !" << std::endl;
    }
}

void GameSocket::handler_receive_size(const boost::system::error_code& error, const std::string& code)
{
    if (!error)
    {
        char size = receiveBuffer[3];
        std::cout << "Size : " << (int)size << std::endl;

        if (code == "HUM")
        {
            socket->async_receive(boost::asio::buffer(receiveBuffer + 4, 2 * size), boost::bind(&GameSocket::handler_receive_data, this, boost::asio::placeholders::error, code, size));
        }
        else if (code == "UPD")
        {
            socket->async_receive(boost::asio::buffer(receiveBuffer + 4, 5 * size), boost::bind(&GameSocket::handler_receive_data, this, boost::asio::placeholders::error, code, size));
        }
        else if (code == "MAP")
        {
            socket->async_receive(boost::asio::buffer(receiveBuffer + 4, 5 * size), boost::bind(&GameSocket::handler_receive_data, this, boost::asio::placeholders::error, code, size));
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
        if (code == "SET")
        {
            char n = receiveBuffer[3];
            char m = receiveBuffer[4];
            GameState::setBoardSize(n, m);
            std::cout << "Size : " << (int)n << " x " << (int)m << std::endl;
        }
        else if (code == "HUM")
        {
            for (int i = 0; i < size; ++i)
            {
                char x = receiveBuffer[4 + (2 * i)];
                char y = receiveBuffer[5 + (2 * i)];
                std::cout << "Human home " << i << " : (" << (int)x << ", " << (int)y << ")" << std::endl;
            }
        }
        else if (code == "HME")
        {
            char x = receiveBuffer[3];
            char y = receiveBuffer[4];
            std::cout << "Home : (" << (int)x << ", " << (int)y << ")" << std::endl;
        }
        else if (code == "UPD")
        {
            // TODO Implement 
            for (int i = 0; i < size; ++i)
            {
                char x = receiveBuffer[4 + (5 * i)];
                char y = receiveBuffer[5 + (5 * i)];
                char humans = receiveBuffer[6 + (5 * i)];
                char vampires = receiveBuffer[7 + (5 * i)];
                char werewolves = receiveBuffer[8 + (5 * i)];
                if (humans > 0)
                {
                    std::cout << "Humans : " << (int)humans << " on (" << (int)x << ", " << (int)y << ")" << std::endl;
                }
                if (vampires > 0)
                {
                    std::cout << "Vampires : " << (int)vampires << " on (" << (int)x << ", " << (int)y << ")" << std::endl;
                }
                if (werewolves > 0)
                {
                    std::cout << "Werewolves : " << (int)werewolves << " on (" << (int)x << ", " << (int)y << ")" << std::endl;
                }
            }
        }
        else if (code == "MAP")
        {
            // TODO Implement 
            for (int i = 0; i < size; ++i)
            {
                char x = receiveBuffer[4 + (5 * i)];
                char y = receiveBuffer[5 + (5 * i)];
                char humans = receiveBuffer[6 + (5 * i)];
                char vampires = receiveBuffer[7 + (5 * i)];
                char werewolves = receiveBuffer[8 + (5 * i)];
                if (humans > 0)
                {
                    std::cout << "Humans : " << (int)humans << " on (" << (int)x << ", " << (int)y << ")" << std::endl;
                }
                if (vampires > 0)
                {
                    std::cout << "Vampires : " << (int)vampires << " on (" << (int)x << ", " << (int)y << ")" << std::endl;
                }
                if (werewolves > 0)
                {
                    std::cout << "Werewolves : " << (int)werewolves << " on (" << (int)x << ", " << (int)y << ")" << std::endl;
                }
            }
        }
        std::cout << std::endl;
        receive();
    }
    else {
        std::cout << "Reception failed !" << std::endl;
    }
}

void GameSocket::setCallback(PlayCallback c)
{
	callback = c;
}

void GameSocket::close()
{
    socket->close();
}