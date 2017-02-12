//
//  Client.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Client_hpp
#define Client_hpp

#include "Messenger.hpp"

class Client : public Messenger {
    ConnectionPtr server;
    std::vector<Geometry::Vector> addedUnits;
    
public:
    explicit Client(std::shared_ptr<Network::SocketAddress> address);
    virtual ~Client() override {}
    
    virtual void Turn() override;
    virtual void AddUnit(const Geometry::Vector vector) override;
    virtual void Init(class GameField *gameField) override;
    
protected:
    virtual void OnMessageRecv(const ConnectionPtr connection) override;
    virtual void OnMessageSend(const ConnectionPtr connection) override;
    virtual void OnDestroy() override;
};

#endif /* Client_hpp */
