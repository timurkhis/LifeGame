//
//  Client.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Client_hpp
#define Client_hpp

#include <vector>
#include "Geometry.h"
#include "Network.h"

class Client {
    std::vector<Geometry::Vector> addedUnits;
    std::shared_ptr<Network::SocketAddress> address;
    Network::TCPSocketPtr server;
    Network::InputMemoryStream input;
    Network::OutputMemoryStream output;
    class GameField *gameField;
    
public:
    explicit Client(std::shared_ptr<Network::SocketAddress> address, size_t capacity = 1024);
    
    void Turn();
    void AddUnit(const Geometry::Vector vector);
    void Init(class GameField *gameField);
    std::shared_ptr<Network::SocketAddress> Address() { return address; }
    
private:
    void Send();
    void Recv();
};

#endif /* Client_hpp */
