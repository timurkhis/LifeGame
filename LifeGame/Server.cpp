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

Server::Server(Vector fieldSize, size_t outputCapacity) : fieldSize(fieldSize), output(outputCapacity) {
    address = std::make_shared<SocketAddress>();
    listener = TCPSocket::Create();
    listener->Bind(*address);
    listener->Listen();
    listener->Addr(*address);
    selector.Add(listener);
    std::thread update(&Server::Update, this);
    update.detach();
}

void Server::Update() {
    std::vector<TCPSocketPtr> read;
    std::vector<TCPSocketPtr> write;
    std::vector<TCPSocketPtr> except;
    
    while (selector.Select(&read, &write, &except) > 0) {
        auto newPlayer = std::find(read.begin(), read.end(), listener);
        if (newPlayer != read.end()) {
            TCPSocketPtr newSocket = newPlayer->get()->Accept(*address);
            selector.Add(newSocket);
            output << fieldSize.x << fieldSize.y;
            newSocket->Send(output.Data(), output.Size());
            output.Clear();
        }
    }
}
