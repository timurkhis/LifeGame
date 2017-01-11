//
//  Client.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <iostream>
#include "Client.hpp"
#include "GameField.hpp"

using namespace Geometry;
using namespace Network;

Client::Client(SocketAddress &address) {
    memset(buffer, 0, size);
    server = TCPSocket::Create();
    server->Connect(address);
    server->Recv(buffer, size);
    memcpy(&fieldSize, buffer, sizeof(fieldSize));
}

void Client::Init(const GameField *gameField) {
    this->gameField = gameField;
}
