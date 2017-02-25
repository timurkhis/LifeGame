//
//  Command.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 24.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Command_hpp
#define Command_hpp

#include <vector>
#include <memory>

enum class Cmd {
    Empty,
    AddUnits
};

class Command {
public:
    static std::shared_ptr<Command> Parse(Network::InputMemoryStream &stream);
    
    virtual ~Command() = 0;
    virtual Cmd Type() = 0;
    virtual void Apply(class Peer *peer) = 0;
    
    void Read(Network::InputMemoryStream &stream);
    void Write(Network::OutputMemoryStream &stream);
    
private:
    virtual void OnRead(Network::InputMemoryStream &stream) = 0;
    virtual void OnWrite(Network::OutputMemoryStream &stream) = 0;
};

class EmptyCommand : public Command {
public:
    virtual ~EmptyCommand() override {}
    virtual Cmd Type() override { return Cmd::Empty; }
    virtual void Apply(class Peer *peer) override {}
    
private:
    virtual void OnRead(Network::InputMemoryStream &stream) override {}
    virtual void OnWrite(Network::OutputMemoryStream &stream) override {}
};

class AddUnitsCommand : public Command {
    int id;
    std::vector<Geometry::Vector> units;
    
public:
    AddUnitsCommand() : id(-1) {}
    AddUnitsCommand(int id, std::vector<Geometry::Vector> units) : id(id), units(units) {}
    virtual ~AddUnitsCommand() override {}
    virtual Cmd Type() override { return Cmd::AddUnits; }
    virtual void Apply(class Peer *peer) override;
    
private:
    virtual void OnRead(Network::InputMemoryStream &stream) override;
    virtual void OnWrite(Network::OutputMemoryStream &stream) override;
};

#endif /* Command_hpp */
