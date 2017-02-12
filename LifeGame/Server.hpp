//
//  Server.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Server_hpp
#define Server_hpp

#include <unordered_map>
#include "Messenger.hpp"
#include "Log.hpp"
    
class Server : public Messenger {
    const int maxPlayers;
    std::unordered_map<int, std::vector<Geometry::Vector>> allUnits;
    std::unordered_map<int, bool> playerTurns;
    std::unordered_map<ConnectionPtr, int> ids;
    Geometry::Vector fieldSize;
    
public:
    explicit Server(Geometry::Vector fieldSize);
    virtual ~Server() override {}
    
    virtual void Turn() override { Log::Error("Trying to turn on server!"); }
    virtual void AddUnit(const Geometry::Vector vector) override { Log::Error("Trying to add unit on server!"); }
    virtual void Init(class GameField *gameField) override;
    
protected:
    virtual void OnMessageRecv(const ConnectionPtr connection) override;
    virtual void OnMessageSend(const ConnectionPtr connection) override;
    virtual void OnNewConnection(const ConnectionPtr connection) override;
    virtual void OnCloseConnection(const ConnectionPtr connection) override;
    
private:
    void Process();
    void AddPlayer(const ConnectionPtr connection);
};

#endif /* Server_hpp */
