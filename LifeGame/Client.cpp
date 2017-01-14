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

Client::Client(std::shared_ptr<SocketAddress> address, size_t inputSize) : address(address), input(inputSize) {
    server = TCPSocket::Create();
    server->Connect(*address);
    server->Recv(input.Data(), input.Length());
    input >> fieldSize.x >> fieldSize.y;
    input.Clear();
}

void Client::Init(const GameField *gameField) {
    this->gameField = gameField;
}
