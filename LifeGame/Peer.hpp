//
//  Peer.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 16.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Peer_hpp
#define Peer_hpp

#include <queue>
#include <unordered_map>
#include "Messenger.hpp"

enum class Msg {
    NewPlayer,
    AcceptPlayer,
    ConnectPlayer,
    ReadyForGame
};

class Peer : public Messenger {
    class Command;
    typedef std::shared_ptr<Command> CommandPtr;
    typedef std::queue<CommandPtr> CommandsQueue;
    
    bool master;
    int readyPlayers;
    int playersCount;
    std::unordered_map<int, CommandsQueue> players;
    std::unordered_map<ConnectionPtr, int> ids;
    std::shared_ptr<GameField> gameField;
    
public:
    explicit Peer(std::shared_ptr<GameField> gameField, std::shared_ptr<Network::SocketAddress> address);
    explicit Peer(std::shared_ptr<GameField> gameField, int players);
    virtual ~Peer() override {}
    
    void Init();
    void Turn();
    void AddUnit(const Geometry::Vector vector);
    
protected:
    virtual void OnMessageRecv(const ConnectionPtr connection) override;
    virtual void OnMessageSend(const ConnectionPtr connection) override;
    virtual void OnNewConnection(const ConnectionPtr connection) override;
    virtual void OnCloseConnection(const ConnectionPtr connection) override;
    virtual void OnDestroy() override;
    
private:
    void AddPlayer(int id, const ConnectionPtr connection);
    void AcceptNewPlayer(const ConnectionPtr connection);
    void BroadcastNewPlayer(uint32_t host, uint16_t port, int id);
    void ConnectNewPlayer(uint32_t host, uint16_t port, int32_t id);
    
    struct Message {
        static std::shared_ptr<Message> Parse(Network::InputMemoryStream &stream);
        
        virtual ~Message() = 0;
        virtual Msg Type() = 0;
        
        void Write(Peer *peer, const ConnectionPtr connection);
        void Read(Peer *peer, const ConnectionPtr connection);
        
    private:
        virtual void OnWrite(Peer *peer, const ConnectionPtr connection) = 0;
        virtual void OnRead(Peer *peer, const ConnectionPtr connection) = 0;
    };
    
    struct NewPlayerMessage : public Message {
    private:
        int id;
        uint32_t host;
        uint16_t port;
        
    public:
        NewPlayerMessage() {}
        NewPlayerMessage(int id, uint32_t host, uint16_t port) : id(id), host(host), port(port) {}
        
        virtual ~NewPlayerMessage() override {}
        virtual Msg Type() override { return Msg::NewPlayer; }
        
    private:
        virtual void OnWrite(Peer *peer, const ConnectionPtr connection) override;
        virtual void OnRead(Peer *peer, const ConnectionPtr connection) override;
    };
    
    struct AcceptPlayerMessage : public Message {
        virtual ~AcceptPlayerMessage() override {}
        virtual Msg Type() override { return Msg::AcceptPlayer; }
        
    private:
        virtual void OnWrite(Peer *peer, const ConnectionPtr connection) override;
        virtual void OnRead(Peer *peer, const ConnectionPtr connection) override;
    };
    
    struct ConnectNewPlayerMessage : public Message {
        virtual ~ConnectNewPlayerMessage() override {}
        virtual Msg Type() override { return Msg::ConnectPlayer; }
        
    private:
        virtual void OnWrite(Peer *peer, const ConnectionPtr connection) override;
        virtual void OnRead(Peer *peer, const ConnectionPtr connection) override;
    };
    
    struct ReadyForGameMessage : public Message {
        virtual ~ReadyForGameMessage() override {}
        virtual Msg Type() override { return Msg::ReadyForGame; }
        
    private:
        virtual void OnWrite(Peer *peer, const ConnectionPtr connection) override;
        virtual void OnRead(Peer *peer, const ConnectionPtr connection) override;
    };
};

#endif /* Peer_hpp */
