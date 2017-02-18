//
//  Peer.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 16.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <cassert>
#include "GameField.hpp"
#include "Peer.hpp"

using namespace Network;
using namespace Geometry;

Peer::Peer(std::shared_ptr<GameField> gameField, std::shared_ptr<Network::SocketAddress> address) : master(false), playersCount(0) {
    this->gameField = gameField;
    gameField->peer = std::shared_ptr<Peer>(this);
    this->address = address;
}

Peer::Peer(std::shared_ptr<GameField> gameField, int players) : master(true), playersCount(players) {
    this->gameField = gameField;
    gameField->peer = std::shared_ptr<Peer>(this);
    address = SocketAddress::CreateIPv4("localhost");
    Listen(address);
}

void Peer::Init() {
    if (master) return;
    masterPeer = std::make_shared<Connection>(TCPSocket::Create());
    masterPeer->socket->Connect(*address);
    AddConnection(masterPeer);
    NewPlayerCommand command(Listen());
    command.Write(masterPeer->output);
    Send(masterPeer);
    do {
        Update(true);
    } while (!masterPeer->CanWrite());
    do {
        Update(true);
    } while (!masterPeer->CanRead());
    assert(gameField->player >= 0 && gameField->size.x > 0 && gameField->size.y > 0);
}

void Peer::Turn() {}

void Peer::AddUnit(const Vector vector) {}

void Peer::OnMessageRecv(const ConnectionPtr connection) {
    CommandPtr command = Command::Read(connection->input);
    if (command == nullptr) {
        CloseConnection(connection);
    } else {
        command->Apply(*this, connection);
    }
    connection->input.Clear();
}

void Peer::OnMessageSend(const ConnectionPtr connection) {
    connection->output.Clear();
}

void Peer::OnNewConnection(const ConnectionPtr connection) {
    if (players.size() >= playersCount - 1) return;
    AddConnection(connection);
}

void Peer::OnCloseConnection(const ConnectionPtr connection) {}

void Peer::OnDestroy() {
    if (!master) {
        masterPeer.reset();
    }
}



Peer::Command::~Command() {}

Peer::CommandPtr Peer::Command::Read(Network::InputMemoryStream &stream) {
    uint32_t msgSize;
    int32_t type;
    stream >> msgSize >> type;
    MsgType msgType = static_cast<MsgType>(type);
    
    switch (msgType) {
        case MsgType::NewPlayer:
            return NewPlayerCommand::Read(stream);
        case MsgType::Connect:
            return ConnectPlayerCommand::Read(stream);
        default:
            return nullptr;
    }
}

void Peer::Command::Write(Network::OutputMemoryStream &stream) {
    uint32_t msgSize;
    stream << msgSize << static_cast<int32_t>(GetType());
    OnWrite(stream);
    msgSize = stream.Size();
    stream.Write(msgSize, 0);
}

std::shared_ptr<Peer::NewPlayerCommand> Peer::NewPlayerCommand::Read(Network::InputMemoryStream &stream) {
    uint16_t listenerPort;
    stream >> listenerPort;
    return std::make_shared<NewPlayerCommand>(listenerPort);
}

void Peer::NewPlayerCommand::Apply(Peer &peer, const ConnectionPtr connection) {
    SocketAddress remoteAddress;
    connection->socket->Addr(remoteAddress, true);
    int id = static_cast<int>(peer.players.size()) + 1;
    
    ConnectPlayerCommand connect(id, peer.playersCount, peer.gameField->size, peer.gameField->turnTime);
    connect.Write(connection->output);
    peer.Send(connection);
    
    auto pair = std::make_pair(id, Player(id, remoteAddress.GetHost(), listenerPort));
    peer.players.emplace(pair);
    peer.ids[connection] = id;
}

void Peer::NewPlayerCommand::OnWrite(Network::OutputMemoryStream &stream) {
    stream << listenerPort;
}

std::shared_ptr<Peer::ConnectPlayerCommand> Peer::ConnectPlayerCommand::Read(Network::InputMemoryStream &stream) {
    int32_t id, playersCount, x, y;
    uint32_t turnTime;
    stream >> id >> playersCount >> x >> y >> turnTime;
    Vector fieldSize = Vector(static_cast<int>(x), static_cast<int>(y));
    return std::make_shared<ConnectPlayerCommand>(static_cast<int>(id), static_cast<int>(playersCount), fieldSize, static_cast<unsigned>(turnTime));
}

void Peer::ConnectPlayerCommand::Apply(Peer &peer, const ConnectionPtr connection) {
    peer.gameField->size = fieldSize;
    peer.gameField->player = id;
    peer.gameField->turnTime = turnTime;
    peer.playersCount = playersCount;
}

void Peer::ConnectPlayerCommand::OnWrite(Network::OutputMemoryStream &stream) {
    stream
    << static_cast<int32_t>(id)
    << static_cast<int32_t>(playersCount)
    << static_cast<int32_t>(fieldSize.x)
    << static_cast<int32_t>(fieldSize.y)
    << static_cast<uint32_t>(turnTime);
}




