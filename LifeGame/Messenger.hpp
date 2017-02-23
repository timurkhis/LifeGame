//
//  Messenger.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Messenger_hpp
#define Messenger_hpp

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
    
public:
    virtual ~Messenger() = 0;
    void Destroy();
    void Update(bool block = false);
    std::shared_ptr<Network::SocketAddress> Address() { return address; }
    
protected:
    virtual void OnMessageRecv(const ConnectionPtr connection) {}
    virtual void OnMessageSend(const ConnectionPtr connection) {}
    virtual void OnNewConnection(const ConnectionPtr connection) {}
    virtual void OnCloseConnection(const ConnectionPtr connection) {}
    virtual void OnDestroy() {}
    void Listen(std::shared_ptr<Network::SocketAddress> address = nullptr);
    void AddConnection(const ConnectionPtr connection);
    void Send(const ConnectionPtr connection);
    void CloseConnection(const ConnectionPtr connection);
    std::string ListenerAddress();
    
private:
    void Read(const std::vector<Network::TCPSocketPtr> &outRead);
    void Write(const std::vector<Network::TCPSocketPtr> &outWrite);
    void Except(const std::vector<Network::TCPSocketPtr> &outExcept);
    void NewConnection(const std::vector<Network::TCPSocketPtr> &outRead);
    void CloseConnection(const ConnectionPtr connection, bool callback);
    void Remove(Network::TCPSocketPtr socket, std::vector<Network::TCPSocketPtr> &from);
};

#endif /* Messenger_hpp */
