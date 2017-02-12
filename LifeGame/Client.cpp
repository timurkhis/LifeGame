//
//  Client.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <unordered_map>
#include "Client.hpp"
#include "Geometry.h"
#include "Network.h"
#include "GameField.hpp"

using namespace Geometry;
using namespace Network;
    
Client::Client(std::shared_ptr<SocketAddress> address) {
    this->address = address;
}

void Client::Turn() {
    ::Write<Message::Turn>(server->output, gameField->player, addedUnits);
    Send(server);
    addedUnits.clear();
}

void Client::AddUnit(const Vector vector) {
    addedUnits.push_back(vector);
}

void Client::Init(GameField *gameField) {
    this->gameField = gameField;
    server = std::make_shared<Connection>(TCPSocket::Create());
    server->socket->Connect(*address);
    AddConnection(server);
    std::unordered_map<int, std::vector<Vector>> units;
    ::Write<Message::Init>(server->output, gameField->player, gameField->size, units);
    Send(server);
    do {
        Update(true);
    } while (!server->CanWrite());
    do {
        Update(true);
    } while (!server->CanRead());
    server->input.Clear();
}

void Client::OnMessageRecv(const ConnectionPtr connection) {
    if (::IsMsg<Message::Init>(connection->input)) {
        std::unordered_map<int, std::vector<Vector>> units;
        ::Read<Message::Init>(server->input, gameField->player, gameField->size, units);
        for (const auto iter : units) {
            const std::vector<Vector> &vec = iter.second;
            for (int i = 0; i < vec.size(); i++) {
                gameField->units->emplace(iter.first, vec[i]);
            }
        }
    } else if (::IsMsg<Message::Process>(connection->input)) {
        gameField->turn = false;
        std::unordered_map<int, std::vector<Vector>> allAddedUnits;
        ::Read<Message::Process>(server->input, allAddedUnits);
        for (const auto iter : allAddedUnits) {
            const std::vector<Vector> &vec = iter.second;
            for (int i = 0; i < vec.size(); i++) {
                gameField->units->emplace(iter.first, vec[i]);
            }
        }
        gameField->ProcessUnits();
        addedUnits.clear();
    }
    connection->input.Clear();
}

void Client::OnMessageSend(const ConnectionPtr connection) {
    connection->output.Clear();
}

void Client::OnDestroy() {
    server.reset();
}
