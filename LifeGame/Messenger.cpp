//
//  Messenger.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Messenger.hpp"

using namespace Network;

Messenger::~Messenger() {}

void Messenger::Destroy() {
    OnDestroy();
    listener.reset();
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
    NewConnection(outRead);
    for (TCPSocketPtr socket : outRead) {
        auto iter = std::find_if(connections.begin(), connections.end(), [socket](ConnectionPtr c){ return socket == c->socket; });
        if (iter == connections.end()) continue;
        ConnectionPtr connection = *iter;
        if (connection->Recv() == 0) {
            CloseConnection(connection);
        } else if (connection->CanRead()) {
            OnMessageRecv(connection);
        }
    }
}

void Messenger::Write(const std::vector<TCPSocketPtr> &outWrite) {
    for (TCPSocketPtr socket : outWrite) {
        auto iter = std::find_if(connections.begin(), connections.end(), [socket](ConnectionPtr c){ return socket == c->socket; });
        if (iter == connections.end()) continue;
        ConnectionPtr connection = *iter;
        connection->Send();
        if (connection->CanWrite()) {
            Remove(connection->socket, sendList);
            OnMessageSend(connection);
        }
    }
}

void Messenger::Except(const std::vector<TCPSocketPtr> &outExcept) {
    
}

void Messenger::NewConnection(const std::vector<TCPSocketPtr> &outRead) {
    if (listener == nullptr || std::find(outRead.begin(), outRead.end(), listener) == outRead.end()) return;
    TCPSocketPtr socket = listener->Accept(*address);
    ConnectionPtr connection = std::make_shared<Connection>(socket);
    OnNewConnection(connection);
}

void Messenger::CloseConnection(const ConnectionPtr connection) {
    OnCloseConnection(connection);
    Remove(connection->socket, recvList);
    Remove(connection->socket, sendList);
    Remove(connection->socket, exceptList);
    auto iter = std::find(connections.begin(), connections.end(), connection);
    if (iter != connections.end()) {
        connections.erase(iter);
    }
}

void Messenger::Remove(TCPSocketPtr socket, std::vector<TCPSocketPtr> &from) {
    auto iter = std::find(from.begin(), from.end(), socket);
    if (iter != from.end()) {
        from.erase(iter);
    }
}

void Messenger::Listen() {
    listener = TCPSocket::Create();
    listener->Bind(*address);
    listener->Listen();
    listener->Addr(*address);
    recvList.push_back(listener);
}

void Messenger::AddConnection(const ConnectionPtr connection) {
    connections.push_back(connection);
    recvList.push_back(connection->socket);
}

void Messenger::Send(const ConnectionPtr connection) {
    sendList.push_back(connection->socket);
}
