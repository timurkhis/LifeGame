//
//  Peer.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 16.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <cassert>
#include "Log.hpp"
#include "GameField.hpp"
#include "Peer.hpp"

using namespace Network;
using namespace Geometry;

Peer::Peer(std::shared_ptr<GameField> gameField, std::shared_ptr<SocketAddress> address) :
    masterPeer(new Connection(TCPSocket::Create())),
    playersCount(0),
    readyPlayers(0) {
    this->gameField = gameField;
    gameField->peer = std::shared_ptr<Peer>(this);
    this->address = address;
}

Peer::Peer(std::shared_ptr<GameField> gameField, int players) : playersCount(players), readyPlayers(1) {
    this->gameField = gameField;
    gameField->peer = std::shared_ptr<Peer>(this);
    address = SocketAddress::CreateIPv4("localhost");
    Listen(address);
}

void Peer::Init() {
    if (IsMaster()) return;
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

void Peer::OnCloseConnection(const ConnectionPtr connection) {
    if (ids.size() == 0 || !IsGameStarted()) {
        gameField->Destroy();
        return;
    }
    const int id = ids[connection];
    if (connection == masterPeer) {
        if (gameField->player - id == 1) {
            masterPeer.reset();
        } else {
            typedef std::unordered_map<ConnectionPtr, int>::value_type iterator;
            auto newMaster = std::find_if(ids.begin(), ids.end(), [id](const iterator &it){ return it.second - id == 1; });
            assert(newMaster->second);
            masterPeer = newMaster->first;
        }
    }
    ids.erase(connection);
    players.erase(id);
    Log::Warning("Peer", id, "closed connection. Master is", IsMaster() ? gameField->player : ids[masterPeer]);
}

void Peer::OnDestroy() {
    players.clear();
    if (!IsMaster()) {
        masterPeer.reset();
    }
}

void Peer::AddPlayer(int id, const ConnectionPtr connection) {
    players.emplace(id, CommandsQueue());
    ids.emplace(connection, id);
}

void Peer::AcceptNewPlayer(const ConnectionPtr connection) {
    AcceptPlayerMessage().Write(this, connection);
    Send(connection);
}

void Peer::BroadcastNewPlayer(const std::string &listenerAddress, int id) {
    NewPlayerMessage msg(id, listenerAddress);
    for (const auto it : ids) {
        msg.Write(this, it.first);
        Send(it.first);
    }
}

void Peer::ConnectNewPlayer(const std::string &listenerAddress, int id) {
    auto address = SocketAddress::CreateIPv4(listenerAddress);
    Log::Warning("Peer", gameField->player, "connects to other peer", id, *address);
    TCPSocketPtr socket = TCPSocket::Create();
    socket->Connect(*address);
    ConnectionPtr newPlayer = std::make_shared<Connection>(socket);
    AddConnection(newPlayer);
    AddPlayer(id, newPlayer);
    ConnectNewPlayerMessage().Write(this, newPlayer);
    Send(newPlayer);
}

void Peer::CheckReadyForGame() {
    if (players.size() == playersCount - 1) {
        ReadyForGameMessage().Write(this, masterPeer);
        Send(masterPeer);
    }
}



Peer::Message::~Message() {}

std::shared_ptr<Peer::Message> Peer::Message::Parse(InputMemoryStream &stream) {
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
    Log::Warning("Peer", peer->gameField->player, "reads message of type", type);
    OnRead(peer, connection);
}

void Peer::Message::Write(Peer *peer, const ConnectionPtr connection) {
    Log::Warning("Peer", peer->gameField->player, "writes message of type", static_cast<int32_t>(Type()));
    uint32_t msgSize;
    connection->output << msgSize << static_cast<int32_t>(Type());
    OnWrite(peer, connection);
    msgSize = connection->output.Size();
    connection->output.Write(msgSize, 0);
}

void Peer::NewPlayerMessage::OnRead(Peer *peer, const ConnectionPtr connection) {
    if (peer->IsMaster()) {
        SocketAddress address;
        connection->socket->Addr(address, true);
        std::string remoteAddress = address.ToString();
        std::string listenerAddress;
        ReadAddress(connection->input, listenerAddress);
        std::string host;
        std::string port;
        host = remoteAddress.substr(0, remoteAddress.find_last_of(':'));
        port = listenerAddress.substr(listenerAddress.find_last_of(':'));
        std::string listenerRemoteAddress = host + port;
        int id = static_cast<int>(peer->players.size() + 1);
        peer->BroadcastNewPlayer(listenerRemoteAddress, id);
        peer->AddPlayer(id, connection);
        peer->AcceptNewPlayer(connection);
    } else {
        int32_t id;
        std::string listenerAddress;
        ReadAddress(connection->input, listenerAddress);
        connection->input >> id;
        assert(id >= 0);
        peer->ConnectNewPlayer(listenerAddress, static_cast<int>(id));
    }
}

void Peer::NewPlayerMessage::OnWrite(Peer *peer, const ConnectionPtr connection) {
    if (peer->IsMaster()) {
        WriteAddress(connection->output, listenerAddress);
        connection->output << static_cast<int32_t>(id);
    } else {
        WriteAddress(connection->output, peer->ListenerAddress());
    }
}

void Peer::NewPlayerMessage::ReadAddress(InputMemoryStream &stream, std::string &address) {
    uint32_t size;
    stream >> size;
    address.resize(size);
    for (int i = 0; i < size; i++) {
        uint8_t symbol;
        stream >> symbol;
        address[i] = static_cast<char>(symbol);
    }
}

void Peer::NewPlayerMessage::WriteAddress(OutputMemoryStream &stream, const std::string &address) {
    stream << static_cast<uint32_t>(address.size());
    for (int i = 0; i < address.size(); i++) {
        stream << static_cast<uint8_t>(address[i]);
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
    peer->AddPlayer(static_cast<int>(masterId), connection);
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
    peer->CheckReadyForGame();
}

void Peer::ConnectNewPlayerMessage::OnWrite(Peer *peer, const ConnectionPtr connection) {
    connection->output << static_cast<int32_t>(peer->gameField->player);
    peer->CheckReadyForGame();
}

void Peer::ReadyForGameMessage::OnRead(Peer *peer, const ConnectionPtr connection) {
    int32_t playersSize, playersCount, readyPlayers;
    connection->input >> playersSize >> playersCount >> readyPlayers;
    assert(playersCount == peer->playersCount);
    assert(playersSize == peer->players.size());
    
    if (peer->IsMaster()) {
        assert(readyPlayers == 0);
        if (++peer->readyPlayers == peer->playersCount) {
            Log::Warning("Ready for game!");
            ReadyForGameMessage msg;
            for (const auto it : peer->ids) {
                msg.Write(peer, it.first);
                peer->Send(it.first);
            }
        }
    } else {
        assert(peer->readyPlayers == 0);
        peer->readyPlayers = readyPlayers;
        assert(peer->playersCount == peer->readyPlayers);
    }
}

void Peer::ReadyForGameMessage::OnWrite(Peer *peer, const ConnectionPtr connection) {
    assert(peer->playersCount == peer->players.size() + 1);
    connection->output
    << static_cast<int32_t>(peer->players.size())
    << static_cast<int32_t>(peer->playersCount)
    << static_cast<int32_t>(peer->readyPlayers);
    
    if (!peer->IsMaster()) {
        assert(connection == peer->masterPeer);
    }
}
