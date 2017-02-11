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

void Messenger::Destroy() {
    OnDestroy();
    recvList.clear();
    sendList.clear();
    exceptList.clear();
    connections.clear();
}

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
            CloseConnection(connection);
            connections.erase(iter);
        } else if (connection->CanRead()) {
            Remove(connection->socket, recvList);
            OnMessageRecv(connection);
        }
    }
    OnRead(outRead);
}

void Messenger::Write(const std::vector<TCPSocketPtr> &outWrite) {
    for (TCPSocketPtr socket : outWrite) {
        auto iter = std::find_if(connections.begin(), connections.end(), [socket](ConnectionPtr c){ return socket == c->socket; });
        if (iter == connections.end()) continue;
        ConnectionPtr connection = *iter;
        if (connection->Send() == 0) {
            CloseConnection(connection);
            connections.erase(iter);
        } else if (connection->CanWrite()) {
            Remove(connection->socket, sendList);
            OnMessageSend(connection);
        }
    }
    OnWrite(outWrite);
}

void Messenger::Except(const std::vector<TCPSocketPtr> &outExcept) {
    OnExcept(outExcept);
}

void Messenger::CloseConnection(const ConnectionPtr connection) {
    OnCloseConnection(connection);
    Remove(connection->socket, recvList);
    Remove(connection->socket, sendList);
    Remove(connection->socket, exceptList);
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
    ::Write<Message::Init>(server->output, gameField->player, gameField->size);
    do {
        Update(true);
    } while (!server->CanWrite());
    assert(sendList.size() == 0);
    do {
        Update(true);
    } while (!server->CanRead());
    assert(recvList.size() == 0);
    server->input.Clear();
}

void Client::OnMessageRecv(ConnectionPtr connection) {
    if (::IsMsg<Message::Init>(connection->input)) {
        ::Read<Message::Init>(server->input, gameField->player, gameField->size);
    } else if (::IsMsg<Message::Process>(connection->input)) {
        assert(recvList.size() == 0);
        gameField->turn = false;
        std::unordered_map<int, std::vector<Vector>> allAddedUnits;
        ::Read<Message::Process>(server->input, allAddedUnits);
        for (const auto &iter : allAddedUnits) {
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

void Client::OnMessageSend(ConnectionPtr connection) {
    assert(sendList.size() == 0);
    recvList.push_back(connection->socket);
    connection->output.Clear();
}

void Client::OnDestroy() {
    server.reset();
}

Server::Server(Vector fieldSize) :
    maxPlayers(8),
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
        TCPSocketPtr newPlayer = listener->Accept(*address);
        ConnectionPtr connection = std::make_shared<Connection>(newPlayer);
        connections.push_back(connection);
        recvList.push_back(connection->socket);
    }
}

void Server::OnMessageRecv(ConnectionPtr connection) {
    if (::IsMsg<Message::Init>(connection->input)) {
        AddPlayer(connection);
    } else if (::IsMsg<Message::Turn>(connection->input)) {
        int player;
        std::vector<Vector> newUnits;
        ::Read<Message::Turn>(connection->input, player, newUnits);
        assert(player == ids[connection]);
        allUnits[player] = std::move(newUnits);
        playerTurns[player] = true;
        Process();
    }
    connection->input.Clear();
}

void Server::OnMessageSend(ConnectionPtr connection) {
    connection->output.Clear();
    recvList.push_back(connection->socket);
}

void Server::OnCloseConnection(const ConnectionPtr connection) {
    int index = ids[connection];
    playerTurns[index] = true;
    Process();
    playerTurns.erase(index);
    allUnits.erase(index);
    ids.erase(connection);
}

void Server::OnDestroy() {
    listener.reset();
}

void Server::Process() {
    for (const auto &iter : playerTurns) {
        if (!iter.second) return;
    }
    assert(sendList.size() == 0);
    for (int i = 0; i < connections.size(); i++) {
        ConnectionPtr connection = connections[i];
        ::Write<Message::Process>(connection->output, allUnits);
        sendList.push_back(connection->socket);
        int index = ids[connection];
        playerTurns[index] = false;
    }
}

void Server::AddPlayer(ConnectionPtr connection) {
    int result;
    for (result = 0; result < maxPlayers && playerTurns.find(result) != playerTurns.end(); result++);
    if (result >= maxPlayers) {
        auto iter = std::find(connections.begin(), connections.end(), connection);
        connections.erase(iter);
        return;
    }
    ::Write<Message::Init>(connection->output, result, fieldSize);
    sendList.push_back(connection->socket);
    allUnits[result] = std::vector<Vector>();
    playerTurns[result] = false;
    ids[connection] = result;
}
