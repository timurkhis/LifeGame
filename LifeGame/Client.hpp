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
    const static int size = 1000;
    std::vector<Geometry::Vector> addedUnits;
    std::vector<Geometry::Vector> removedUnits;
    std::shared_ptr<Network::TCPSocket> server;
    char buffer[size];
    Geometry::Vector fieldSize;
    const class GameField *gameField;
    
public:
    explicit Client(Network::SocketAddress &address);
    
    void Init(const class GameField *gameField);
    Geometry::Vector FieldSize() const { return fieldSize; }
};

#endif /* Client_hpp */
