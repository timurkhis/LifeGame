//
//  Messenger.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 30.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <iostream>
#include <cassert>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include "Messenger.hpp"
#include "GameField.hpp"

using namespace Geometry;
using namespace Network;

Connection::Connection(TCPSocketPtr socket) :
    canRead(true),
    canWrite(true),
    input(1024),
    output(1024),
    recvData(0),
    allRecvData(0),
    sendData(0),
    socket(socket) {}

int Connection::Recv() {
    uint32_t dataSize = socket->DataSize();
    if (input.Capacity() - recvData < dataSize) {
        input.Realloc(std::max(input.Capacity() * 2, dataSize));
    }
    int result = socket->Recv(input.Data(recvData), input.Capacity() - recvData);
    recvData += result;
    if (allRecvData == 0) {
        input.Read(allRecvData, 0);
    }
    canRead = recvData == allRecvData;
    if (canRead) {
        recvData = 0;
        allRecvData = 0;
    }
    return result;
}

int Connection::Send() {
    int result = socket->Send(output.Data(sendData), output.Size() - sendData);
    sendData += result;
    canWrite = sendData == output.Size();
    if (canWrite) {
        sendData = 0;
    }
    return result;
}

Messenger::~Messenger() {}

void Messenger::Update(bool block) {
    std::vector<TCPSocketPtr> outRead;
    std::vector<TCPSocketPtr> outWrite;
    std::vector<TCPSocketPtr> outExcept;
    SocketSelector::Select(&recvList, &outRead, &sendList, &outWrite, &exceptList, &outExcept, block);
    Read(outRead);
    Write(outWrite);
    Except(outExcept);
}

void Messenger::Read(const std::vector<TCPSocketPtr> &outRead) {
    for (TCPSocketPtr socket : outRead) {
        auto iter = std::find_if(connections.begin(), connections.end(), [socket](ConnectionPtr c){ return socket == c->socket; });
        if (iter == connections.end()) continue;
        ConnectionPtr connection = *iter;
        if (connection->Recv() == 0) {
            OnCloseConnection(connection);
            connections.erase(iter);
            Remove(connection->socket, recvList);
            Remove(connection->socket, sendList);
        } else if (connection->CanRead()) {
            Remove(connection->socket, recvList);
        }
    }
    OnRead(outRead);
}

void Messenger::Write(const std::vector<TCPSocketPtr> &outWrite) {
    for (TCPSocketPtr socket : outWrite) {
        auto iter = std::find_if(connections.begin(), connections.end(), [socket](ConnectionPtr c){ return socket == c->socket; });
        if (iter == connections.end()) continue;
        ConnectionPtr connection = *iter;
        connection->Send();
        if (connection->CanWrite()) {
            Remove(connection->socket, sendList);
        }
    }
    OnWrite(outWrite);
}

void Messenger::Except(const std::vector<TCPSocketPtr> &outExcept) {
    OnExcept(outExcept);
}

void Messenger::Remove(TCPSocketPtr socket, std::vector<TCPSocketPtr> &from) {
    auto iter = std::find(from.begin(), from.end(), socket);
    if (iter != from.end()) {
        from.erase(iter);
    }
}

Client::Client(std::shared_ptr<SocketAddress> address) {
    this->address = address;
}

void Client::Turn() {
    ::Write<Message::Turn>(server->output, gameField->player, addedUnits);
    sendList.push_back(server->socket);
    addedUnits.clear();
}

void Client::AddUnit(const Vector vector) {
    addedUnits.push_back(vector);
}

void Client::Init(GameField *gameField) {
    this->gameField = gameField;
    server = std::make_shared<Connection>(TCPSocket::Create());
    server->socket->Connect(*address);
    connections.push_back(server);
    sendList.push_back(server->socket);
    do {
        Update(true);
    } while (!server->CanWrite());
    assert(sendList.size() == 0);
    recvList.push_back(server->socket);
    do {
        Update(true);
    } while (!server->CanRead());
    ::Read<Message::Init>(server->input, gameField->player, gameField->size);
    server->input.Clear();
}

void Client::OnRead(const std::vector<TCPSocketPtr> &outRead) {
    if (gameField->player == -1 || !server->CanRead()) return;
    assert(recvList.size() == 0);
    gameField->turn = false;
    std::vector<std::vector<Vector>> allAddedUnits;
    ::Read<Message::Process>(server->input, allAddedUnits);
    for (int i = 0; i < allAddedUnits.size(); i++) {
        for (int j = 0; j < allAddedUnits[i].size(); j++) {
            gameField->units->insert(allAddedUnits[i][j]);
        }
    }
    gameField->ProcessUnits();
    addedUnits.clear();
    server->input.Clear();
}

void Client::OnWrite(const std::vector<TCPSocketPtr> &outWrite) {
    if (server->CanWrite()) {
        assert(sendList.size() == 0);
        recvList.push_back(server->socket);
        server->output.Clear();
    }
}

Server::Server(Vector fieldSize) :
    fieldSize(fieldSize) {
    address = SocketAddress::CreateIPv4("localhost");
    listener = TCPSocket::Create();
    listener->Bind(*address);
    listener->Listen();
    listener->Addr(*address);
    recvList.push_back(listener);
}

void Server::Init(GameField *gameField) {
    gameField->size = fieldSize;
    gameField->turn = true;
}

void Server::OnRead(const std::vector<TCPSocketPtr> &outRead) {
    auto newPlayer = std::find(outRead.begin(), outRead.end(), listener);
    if (newPlayer != outRead.end()) {
        AddPlayer();
    }
    for (ConnectionPtr connection : connections) {
        if (connection->CanRead()) {
            int player;
            std::vector<Vector> newUnits;
            ::Read<Message::Turn>(connection->input, player, newUnits);
            allUnits[player] = std::move(newUnits);
            playerTurns[player] = true;
            connection->input.Clear();
        }
    }
    if (std::count_if(playerTurns.begin(), playerTurns.end(), [](bool value){ return value; }) == playerTurns.size()) {
        assert(sendList.size() == 0);
        for (int i = 0; i < connections.size(); i++) {
            ConnectionPtr connection = connections[i];
            ::Write<Message::Process>(connection->output, allUnits);
            sendList.push_back(connection->socket);
            playerTurns[i] = false;
        }
    }
}

void Server::OnWrite(const std::vector<TCPSocketPtr> &outWrite) {
    for (ConnectionPtr connection : connections) {
        if (connection->CanWrite()) {
            connection->output.Clear();
            recvList.push_back(connection->socket);
        }
    }
}

void Server::OnCloseConnection(const ConnectionPtr connection) {
    for (int i = 0; i < connections.size(); i++) {
        if (connections[i] == connection) {
            playerTurns.erase(playerTurns.begin() + i);
            allUnits.erase(allUnits.begin() + i);
            return;
        }
    }
}

void Server::AddPlayer() {
    TCPSocketPtr newPlayer = listener->Accept(*address);
    ConnectionPtr connection = std::make_shared<Connection>(newPlayer);
    int result = static_cast<int>(connections.size() - 1);
    ::Write<Message::Init>(connection->output, result, fieldSize);
    sendList.push_back(connection->socket);
    allUnits.emplace_back();
    playerTurns.push_back(false);
    connections.push_back(connection);
}
