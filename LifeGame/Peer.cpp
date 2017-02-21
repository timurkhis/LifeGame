//
//  Peer.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 16.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <string>
#include <cassert>
#include "Log.hpp"
#include "GameField.hpp"
#include "Peer.hpp"

using namespace Network;
using namespace Geometry;

Peer::Peer(std::shared_ptr<GameField> gameField, std::shared_ptr<Network::SocketAddress> address) :
    master(false),
    playersCount(0),
    readyPlayers(0) {
    this->gameField = gameField;
    gameField->peer = std::shared_ptr<Peer>(this);
    this->address = address;
}

Peer::Peer(std::shared_ptr<GameField> gameField, int players) : master(true), playersCount(players), readyPlayers(0) {
    this->gameField = gameField;
    gameField->peer = std::shared_ptr<Peer>(this);
    address = SocketAddress::CreateIPv4("localhost");
    Listen(address);
}

void Peer::Init() {
    if (master) return;
    ConnectionPtr masterPeer = std::make_shared<Connection>(TCPSocket::Create());
    masterPeer->socket->Connect(*address);
    AddConnection(masterPeer);
    Listen();
    NewPlayerMessage().Write(this, masterPeer);
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
    std::shared_ptr<Message> msg = Message::Parse(connection->input);
    if (msg == nullptr) {
        CloseConnection(connection);
    } else {
        msg->Read(this, connection);
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

void Peer::OnDestroy() {}

void Peer::AddPlayer(int id, const ConnectionPtr connection) {
    players.emplace(id, CommandsQueue());
    ids.emplace(connection, id);
}

void Peer::AcceptNewPlayer(const ConnectionPtr connection) {
    AcceptPlayerMessage().Write(this, connection);
    Send(connection);
}

void Peer::BroadcastNewPlayer(uint32_t host, uint16_t port, int id) {
    NewPlayerMessage msg(id, host, port);
    for (const auto it : ids) {
        msg.Write(this, it.first);
        Send(it.first);
    }
}

void Peer::ConnectNewPlayer(uint32_t host, uint16_t port, int32_t id) {
    SocketAddress address(host, port);
    Log::Warning("Peer ", gameField->player, " connects to other peer ", id, " ", address);
    TCPSocketPtr socket = TCPSocket::Create();
    socket->Connect(address);
    ConnectionPtr newPlayer = std::make_shared<Connection>(socket);
    AddConnection(newPlayer);
    AddPlayer(static_cast<int>(id), newPlayer);
    ConnectNewPlayerMessage().Write(this, newPlayer);
    Send(newPlayer);
}



Peer::Message::~Message() {}

std::shared_ptr<Peer::Message> Peer::Message::Parse(Network::InputMemoryStream &stream) {
    int32_t type;
    stream.Read(type, sizeof(uint32_t));
    switch (static_cast<Msg>(type)) {
        case Msg::NewPlayer:     return std::make_shared<NewPlayerMessage>();
        case Msg::AcceptPlayer:  return std::make_shared<AcceptPlayerMessage>();
        case Msg::ConnectPlayer: return std::make_shared<ConnectNewPlayerMessage>();
        case Msg::ReadyForGame:  return std::make_shared<ReadyForGameMessage>();
        default:                 return nullptr;
    }
}

void Peer::Message::Read(Peer *peer, const ConnectionPtr connection) {
    uint32_t msgSize;
    int32_t type;
    connection->input >> msgSize >> type;
    if (static_cast<Msg>(type) != Type()) {
        Log::Error("Messages types are not the same!");
    }
    OnRead(peer, connection);
    Log::Warning("Peer ", peer->gameField->player, " reads message of type ", type);
}

void Peer::Message::Write(Peer *peer, const ConnectionPtr connection) {
    uint32_t msgSize;
    connection->output << msgSize << static_cast<int32_t>(Type());
    OnWrite(peer, connection);
    msgSize = connection->output.Size();
    connection->output.Write(msgSize, 0);
    Log::Warning("Peer ", peer->gameField->player, " writes message of type ", static_cast<int32_t>(Type()));
}

void Peer::NewPlayerMessage::OnRead(Peer *peer, const ConnectionPtr connection) {
    if (peer->master) {
        SocketAddress address;
        connection->socket->Addr(address, true);
        uint16_t listenerPort;
        connection->input >> listenerPort;
        int id = static_cast<int>(peer->players.size() + 1);
        peer->BroadcastNewPlayer(address.GetHost(), listenerPort, id);
        peer->AddPlayer(id, connection);
        peer->AcceptNewPlayer(connection);
    } else {
        uint32_t host, port;
        int32_t id;
        connection->input >> host >> port >> id;
        peer->ConnectNewPlayer(host, port, id);
    }
}

void Peer::NewPlayerMessage::OnWrite(Peer *peer, const ConnectionPtr connection) {
    if (peer->master) {
        connection->output << host << port << static_cast<int32_t>(id);
    } else {
        connection->output << peer->ListenerPort();
    }
}

void Peer::AcceptPlayerMessage::OnRead(Peer *peer, const ConnectionPtr connection) {
    int32_t playersCount, x, y, id, masterId;
    uint32_t turnTime;
    connection->input >> playersCount >> x >> y >> id >> masterId >> turnTime;
    peer->playersCount = static_cast<int>(playersCount);
    
    peer->gameField->size = Vector(static_cast<int>(x), static_cast<int>(y));
    peer->gameField->player = static_cast<int>(id);
    peer->gameField->turnTime = static_cast<unsigned>(turnTime);
    
    int master = static_cast<int>(masterId);
    
    peer->AddPlayer(master, connection);
}

void Peer::AcceptPlayerMessage::OnWrite(Peer *peer, const ConnectionPtr connection) {
    connection->output
    << static_cast<int32_t>(peer->playersCount)
    << static_cast<int32_t>(peer->gameField->size.x)
    << static_cast<int32_t>(peer->gameField->size.y)
    << static_cast<int32_t>(peer->players.size())
    << static_cast<int32_t>(peer->gameField->player)
    << static_cast<uint32_t>(peer->gameField->turnTime);
}

void Peer::ConnectNewPlayerMessage::OnRead(Peer *peer, const ConnectionPtr connection) {
    int32_t id;
    connection->input >> id;
    peer->AddPlayer(static_cast<int>(id), connection);
}

void Peer::ConnectNewPlayerMessage::OnWrite(Peer *peer, const ConnectionPtr connection) {
    connection->output << static_cast<int32_t>(peer->gameField->player);
}

void Peer::ReadyForGameMessage::OnRead(Peer *peer, const ConnectionPtr connection) {
    
}

void Peer::ReadyForGameMessage::OnWrite(Peer *peer, const ConnectionPtr connection) {
    assert(peer->master);
    peer->readyPlayers++;
    if (peer->readyPlayers == peer->playersCount) {
        Log::Warning("Ready!");
    }
}
