//
//  Server.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <thread>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Server.hpp"

using namespace Geometry;
using namespace Network;

Server::Server(Vector fieldSize) : fieldSize(fieldSize) {
    address = std::make_shared<SocketAddress>();
    listen = TCPSocket::Create();
    listen->Bind(*address);
    listen->Listen();
    listen->Addr(*address);
    std::thread update(&Server::Update, this);
    update.detach();
}

void Server::Update() {
    auto newSock = listen->Accept(*address);
    players.push_back(newSock);
    output << fieldSize.x << fieldSize.y;
    newSock->Send(output.Data(), output.Length());
    output.Clear();
}
