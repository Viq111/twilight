#ifndef INCLUDE_NETWORKMANAGER
#define INCLUDE_NETWORKMANAGER

#include <memory>
#include "GameSocket.h"

class NetworkManager
{
    NetworkManager();
    void connect();
private:
    std::unique_ptr<GameSocket> socket;
};

#endif