//
//  Client.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <iostream>
#include <cassert>
#include "Client.hpp"
#include "GameField.hpp"

using namespace Geometry;
using namespace Network;

Client::Client(std::shared_ptr<SocketAddress> address, size_t inputCapacity) : address(address), input(inputCapacity) {
    server = TCPSocket::Create();
    server->Connect(*address);
}

void Client::Init(GameField *gameField) {
    this->gameField = gameField;
    server->Recv(input.Data(), input.Capacity());
    int32_t type, player, sizeX, sizeY;
    input >> type >> player >> sizeX >> sizeY;
    assert(static_cast<Message>(type) == Message::Init);
    gameField->player = static_cast<int>(player);
    gameField->size.x = static_cast<int>(sizeX);
    gameField->size.y = static_cast<int>(sizeY);
    input.Clear();
}
