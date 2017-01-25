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

Client::Client(std::shared_ptr<SocketAddress> address, uint32_t capacity) :
    address(address),
    input(capacity),
    output(capacity) {}

void Client::Turn() {
    Send();
    Recv();
}

void Client::AddUnit(const Geometry::Vector vector) {
    addedUnits.push_back(vector);
}

void Client::Init(GameField *gameField) {
    this->gameField = gameField;
    server = TCPSocket::Create();
    server->Connect(*address);
    server->Recv(input.Data(), input.Capacity());
    Read<Message::Init>(input, gameField->player, gameField->size);
    input.Clear();
}

void Client::Send() {
    Write<Message::Turn>(output, gameField->player, addedUnits);
    server->Send(output.Data(), output.Size());
    output.Clear();
    addedUnits.clear();
}

void Client::Recv() {
    server->Recv(input.Data(), input.Capacity());
    gameField->turn = false;
    std::vector<std::vector<Vector>> allAddedUnits;
    Read<Message::Process>(input, allAddedUnits);
    for (int i = 0; i < allAddedUnits.size(); i++) {
        for (int j = 0; j < allAddedUnits[i].size(); j++) {
            gameField->units->insert(allAddedUnits[i][j]);
        }
    }
    gameField->ProcessUnits();
    addedUnits.clear();
    input.Clear();
}
