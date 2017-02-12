//
//  Messanger.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Messanger_hpp
#define Messanger_hpp

#include <vector>
#include "Connection.hpp"
    
class Messenger {
    std::vector<Network::TCPSocketPtr> recvList;
    std::vector<Network::TCPSocketPtr> sendList;
    std::vector<Network::TCPSocketPtr> exceptList;
    Network::TCPSocketPtr listener;
    
protected:
    std::vector<ConnectionPtr> connections;
    std::shared_ptr<Network::SocketAddress> address;
    class GameField *gameField;
    
public:
    virtual ~Messenger() = 0;
    virtual void Init(class GameField *gameField) = 0;
    virtual void AddUnit(Geometry::Vector vec) {}
    virtual void Turn() {}
    void Destroy();
    void Update(bool block = false);
    std::shared_ptr<Network::SocketAddress> Address() { return address; }
    
protected:
    virtual void OnMessageRecv(const ConnectionPtr connection) {}
    virtual void OnMessageSend(const ConnectionPtr connection) {}
    virtual void OnNewConnection(const ConnectionPtr connection) {}
    virtual void OnCloseConnection(const ConnectionPtr connection) {}
    virtual void OnDestroy() {}
    void Listen();
    void AddConnection(const ConnectionPtr connection);
    void Send(const ConnectionPtr connection);
    
private:
    void Read(const std::vector<Network::TCPSocketPtr> &outRead);
    void Write(const std::vector<Network::TCPSocketPtr> &outWrite);
    void Except(const std::vector<Network::TCPSocketPtr> &outExcept);
    void NewConnection(const std::vector<Network::TCPSocketPtr> &outRead);
    void CloseConnection(const ConnectionPtr connection);
    void Remove(Network::TCPSocketPtr socket, std::vector<Network::TCPSocketPtr> &from);
};

#endif /* Messanger_hpp */
