//
//  Server.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Server_hpp
#define Server_hpp

#include <vector>
#include <unordered_set>
#include "Vector.hpp"
#include "Network.h"

class Server {
    std::vector<std::vector<Geometry::Vector>> addedUnits;
    std::vector<bool> playerTurns;
    std::shared_ptr<Network::SocketAddress> address;
    Geometry::Vector fieldSize;
    Network::TCPSocketPtr listener;
    Network::SocketSelector players;
    Network::InputMemoryStream input;
    Network::OutputMemoryStream output;
    
public:
    explicit Server(Geometry::Vector fieldSize, uint32_t capacity = 1024);
    
    std::shared_ptr<Network::SocketAddress> Address() { return address; }
    
private:
    void Update();
    void AddPlayer();
    void RemovePlayer(int player);
};

#endif /* Server_hpp */
