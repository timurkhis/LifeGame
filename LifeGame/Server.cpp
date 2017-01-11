//
//  Server.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <iostream>
#include <thread>
#include "Server.hpp"

using namespace Geometry;
using namespace Network;

Server::Server(Vector fieldSize) : fieldSize(fieldSize), playersTurns(0) {
    listen = TCPSocket::Create();
    listen->Bind(address);
    listen->Listen();
    listen->Addr(address);
    std::thread update(&Server::Update, this);
    update.detach();
}

void Server::Update() {
    auto newSock = listen->Accept(address);
    players.push_back(newSock);
    newSock->Send(&fieldSize, sizeof(fieldSize));
}
