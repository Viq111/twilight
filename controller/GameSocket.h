#ifndef INCLUDE_GAMESOCKET
#define INCLUDE_GAMESOCKET

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class GameSocket
{
public:
    GameSocket(std::string ip, int port, std::string name);
	void connect(); // Connect to a server and set the name
	void close(); // Close the socket
protected:
    void stop();
    void receive();
    void sendName();
    void sendOk();
    void sendError();
    void handler_receive_code(const boost::system::error_code& error);
    void handler_receive_data(const boost::system::error_code& error, const std::string& code, const int size);
    void handler_send(const boost::system::error_code& error);

    //Buffers
    char sendBuffer[128];
    char receiveBuffer[128];

    boost::asio::io_service io_service;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket = nullptr;
    std::unique_ptr<boost::asio::deadline_timer> timer = nullptr;

    std::string name;
    int port;
    std::string ip;
};

#endif