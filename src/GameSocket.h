#ifndef INCLUDE_GAMESOCKET
#define INCLUDE_GAMESOCKET

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Node.h"

typedef std::shared_ptr<Node>(*PlayCallback)(std::shared_ptr<Node>); // Define a callback which take the new GameState and return the played moved (GameState)

class GameSocket
{
public:
	GameSocket(PlayCallback callback = nullptr);
	void setCallback(PlayCallback callback); // Set a new callback
	void connect(std::string ip, int port, std::string name); // Connect to a server and set the name
	std::pair<unsigned int, unsigned int> getBoardSize();
	bool isAlive(); // Return whether the connection with the server is still alive
	void join(); // Wait for thread to end
	void close(); // Close the socket
protected:
    void setup();
    void receive();
    void sendName();
    void sendMovement();
    void handler_receive_code(const boost::system::error_code& error);
    void handler_receive_size(const boost::system::error_code& error, const std::string& code);
    void handler_receive_data(const boost::system::error_code& error, const std::string& code, const int size);
    void handler_send(const boost::system::error_code& error);

    //Buffers
    char sendBuffer[128];
    char receiveBuffer[128];

    boost::asio::io_service io_service;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket = nullptr;
	std::string myName;
	std::shared_ptr<Node> currentNode;
	PlayCallback callback;
};

#endif