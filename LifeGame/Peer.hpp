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

class Peer : public Messenger {
    struct Player;
    
    bool master;
    int playersCount;
    ConnectionPtr masterPeer;
    std::unordered_map<int, Player> players;
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
    class Command {
    public:
        enum class MsgType {
            NewPlayer,
            Connect
        };
        virtual ~Command() = 0;
        
        static std::shared_ptr<Command> Read(Network::InputMemoryStream &stream);
        void Write(Network::OutputMemoryStream &stream);
        virtual void Apply(Peer &peer, const ConnectionPtr connection) = 0;
        
    protected:
        virtual MsgType GetType() = 0;
        virtual void OnWrite(Network::OutputMemoryStream &stream) = 0;
    };
    typedef std::shared_ptr<Command> CommandPtr;
    typedef std::queue<CommandPtr> CommandsQueue;
    
    struct Player {
        Player(int id, uint32_t host, uint16_t port) : id(id), host(host), port(port) {}
        
        int id;
        uint32_t host;
        uint16_t port;
        CommandsQueue commands;
    };
    
    class NewPlayerCommand : public Command {
        uint16_t listenerPort;
        
    public:
        static std::shared_ptr<NewPlayerCommand> Read(Network::InputMemoryStream &stream);
        explicit NewPlayerCommand(uint16_t listenerPort) : listenerPort(listenerPort) {}
        virtual ~NewPlayerCommand() override {}
        
        virtual void Apply(Peer &peer, const ConnectionPtr connection) override;
        
    protected:
        virtual MsgType GetType() override { return MsgType::NewPlayer; }
        virtual void OnWrite(Network::OutputMemoryStream &stream) override;
    };
    
    class ConnectPlayerCommand : public Command {
        int id;
        int playersCount;
        Geometry::Vector fieldSize;
        unsigned turnTime;
        
    public:
        static std::shared_ptr<ConnectPlayerCommand> Read(Network::InputMemoryStream &stream);
        explicit ConnectPlayerCommand(int id, int playersCount, Geometry::Vector fieldSize, unsigned turnTime)
            : id(id), playersCount(playersCount), fieldSize(fieldSize) {}
        virtual ~ConnectPlayerCommand() override {}
        
        virtual void Apply(Peer &peer, const ConnectionPtr connection) override;
        
    protected:
        virtual MsgType GetType() override { return MsgType::Connect; }
        virtual void OnWrite(Network::OutputMemoryStream &stream) override;
    };
};

#endif /* Peer_hpp */
