//
//  Server.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <cassert>
#include "Server.hpp"
#include "Geometry.h"
#include "Network.h"
#include "GameField.hpp"

using namespace Geometry;
using namespace Network;
    
Server::Server(Vector fieldSize) :
    maxPlayers(8),
    fieldSize(fieldSize) {
    address = SocketAddress::CreateIPv4("localhost");
    Listen();
}

void Server::Init(GameField *gameField) {
    this->gameField = gameField;
    gameField->size = fieldSize;
    gameField->turn = true;
}

void Server::OnMessageRecv(const ConnectionPtr connection) {
    if (::IsMsg<Message::Init>(connection->input)) {
        AddPlayer(connection);
    } else if (::IsMsg<Message::Turn>(connection->input)) {
        int player;
        std::vector<Vector> newUnits;
        ::Read<Message::Turn>(connection->input, player, newUnits);
        for (int i = static_cast<int>(newUnits.size()) - 1; i >= 0; i--) {
            auto it = gameField->units->emplace(player, newUnits[i]);
            if (!it.second) {
                newUnits.erase(newUnits.begin() + i);
            }
        }
        allUnits[player] = std::move(newUnits);
        playerTurns[player] = true;
        Process();
    }
    connection->input.Clear();
}

void Server::OnMessageSend(const ConnectionPtr connection) {
    connection->output.Clear();
}

void Server::OnNewConnection(const ConnectionPtr connection) {
    if (ids.size() >= maxPlayers) return;
    AddConnection(connection);
}

void Server::OnCloseConnection(const ConnectionPtr connection) {
    int index = ids[connection];
    playerTurns[index] = true;
    Process();
}

void Server::Process() {
    for (const auto &iter : playerTurns) {
        if (!iter.second) return;
    }
    for (int i = 0; i < connections.size(); i++) {
        ConnectionPtr connection = connections[i];
        ::Write<Message::Process>(connection->output, allUnits);
        Send(connection);
        int index = ids[connection];
        playerTurns[index] = false;
    }
    gameField->ProcessUnits();
}

void Server::AddPlayer(const ConnectionPtr connection) {
    int result =  static_cast<int>(ids.size());
    std::unordered_map<int, std::vector<Vector>> units;
    for (const auto &unit : *gameField->units) {
        auto it = units.find(unit.player);
        if (it == units.end()) {
            auto newIt = units.insert(std::pair<int, std::vector<Vector>>(unit.player, std::vector<Vector>()));
            assert(newIt.second);
            newIt.first->second.push_back(unit.position);
        } else {
            it->second.push_back(unit.position);
        }
    }
    ::Write<Message::Init>(connection->output, result, fieldSize, units);
    Send(connection);
    allUnits[result] = std::vector<Vector>();
    playerTurns[result] = false;
    ids[connection] = result;
}
