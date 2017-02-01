//
//  Messenger.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 30.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Messenger_hpp
#define Messenger_hpp

#include <vector>
#include "Geometry.h"
#include "Network.h"
#include "Vector.hpp"

class Connection {
    bool canRead;
    bool canWrite;
    uint32_t recvData;
    uint32_t allRecvData;
    uint32_t sendData;
    
public:
    Network::TCPSocketPtr socket;
    Network::InputMemoryStream input;
    Network::OutputMemoryStream output;
    
    explicit Connection(Network::TCPSocketPtr socket);
    
    bool CanRead() const { return canRead; }
    bool CanWrite() const { return canWrite; }
    
    int Recv();
    int Send();
};

typedef std::shared_ptr<Connection> ConnectionPtr;

class Messenger {
protected:
    std::shared_ptr<Network::SocketAddress> address;
    std::vector<Network::TCPSocketPtr> recvList;
    std::vector<Network::TCPSocketPtr> sendList;
    std::vector<Network::TCPSocketPtr> exceptList;
    std::vector<ConnectionPtr> connections;
    
public:
    virtual ~Messenger() = 0;
    virtual void Init(class GameField *gameField) = 0;
    virtual void AddUnit(Geometry::Vector vec) {}
    virtual void Turn() {}
    void Update(bool block = false);
    std::shared_ptr<Network::SocketAddress> Address() { return address; }
    
protected:
    virtual void OnRead(const std::vector<Network::TCPSocketPtr> &outRead) {}
    virtual void OnWrite(const std::vector<Network::TCPSocketPtr> &outWrite) {}
    virtual void OnExcept(const std::vector<Network::TCPSocketPtr> &outExcept) {}
    virtual void OnCloseConnection(const ConnectionPtr connection) {}
    
private:
    void Read(const std::vector<Network::TCPSocketPtr> &outRead);
    void Write(const std::vector<Network::TCPSocketPtr> &outWrite);
    void Except(const std::vector<Network::TCPSocketPtr> &outExcept);
    void Remove(Network::TCPSocketPtr socket, std::vector<Network::TCPSocketPtr> &from);
};

class Client : public Messenger {
    ConnectionPtr server;
    std::vector<Geometry::Vector> addedUnits;
    class GameField *gameField;
    
public:
    explicit Client(std::shared_ptr<Network::SocketAddress> address);
    virtual ~Client() override {}
    
    virtual void Turn() override;
    virtual void AddUnit(const Geometry::Vector vector) override;
    virtual void Init(class GameField *gameField) override;
    
protected:
    virtual void OnRead(const std::vector<Network::TCPSocketPtr> &outRead) override;
    virtual void OnWrite(const std::vector<Network::TCPSocketPtr> &outWrite) override;
};

class Server : public Messenger {
    std::vector<std::vector<Geometry::Vector>> allUnits;
    std::vector<bool> playerTurns;
    Geometry::Vector fieldSize;
    Network::TCPSocketPtr listener;
    
public:
    explicit Server(Geometry::Vector fieldSize);
    virtual ~Server() override {}
    
    virtual void Turn() override { Log::Error("Trying to turn on server!"); }
    virtual void AddUnit(const Geometry::Vector vector) override { Log::Error("Trying to add unit on server!"); }
    virtual void Init(class GameField *gameField) override;
    
protected:
    virtual void OnRead(const std::vector<Network::TCPSocketPtr> &outRead) override;
    virtual void OnWrite(const std::vector<Network::TCPSocketPtr> &outWrite) override;
    virtual void OnCloseConnection(const ConnectionPtr connection) override;
    
private:
    void AddPlayer();
    void RemovePlayer(int player);
};

#endif /* Messenger_hpp */
