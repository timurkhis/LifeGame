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

namespace Messaging {

    class Command {
    public:
        enum class Cmd {
            Empty,
            AddUnits,
            AddPreset,
            Complex
        };
        
        static std::shared_ptr<Command> Parse(Network::InputMemoryStream &stream);
        
        virtual ~Command() = 0;
        virtual Cmd Type() = 0;
        virtual void Apply(class GameField *gameField) = 0;
        
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
        virtual void Apply(class GameField *gameField) override {}
        
    private:
        virtual void OnRead(Network::InputMemoryStream &stream) override {}
        virtual void OnWrite(Network::OutputMemoryStream &stream) override {}
    };

    class AddUnitsCommand : public Command {
        int id;
        std::vector<Geometry::Vector> units;
        
    public:
        AddUnitsCommand() : id(-1) {}
        AddUnitsCommand(int id, std::vector<Geometry::Vector> units) : id(id), units(std::move(units)) {}
        virtual ~AddUnitsCommand() override {}
        virtual Cmd Type() override { return Cmd::AddUnits; }
        virtual void Apply(class GameField *gameField) override;
        
    private:
        virtual void OnRead(Network::InputMemoryStream &stream) override;
        virtual void OnWrite(Network::OutputMemoryStream &stream) override;
    };

    class AddPresetCommand : public Command {
        Geometry::Matrix3x3 trs;
        unsigned char preset;
        int id;
        
    public:
        AddPresetCommand() {}
        AddPresetCommand(Geometry::Matrix3x3 trs, unsigned char preset, int id) : trs(trs), preset(preset), id(id) {}
        virtual ~AddPresetCommand() override {}
        virtual Cmd Type() override { return Cmd::AddPreset; }
        virtual void Apply(class GameField *gameField) override;
        
    private:
        virtual void OnRead(Network::InputMemoryStream &stream) override;
        virtual void OnWrite(Network::OutputMemoryStream &stream) override;
    };

    class ComplexCommand : public Command {
        std::vector<std::shared_ptr<Command>> commands;
        
    public:
        ComplexCommand() {}
        ComplexCommand(std::vector<std::shared_ptr<Command>> commands) : commands(std::move(commands)) {}
        virtual ~ComplexCommand() override {}
        virtual Cmd Type() override { return Cmd::Complex; }
        virtual void Apply(class GameField *gameField) override;
        
    private:
        virtual void OnRead(Network::InputMemoryStream &stream) override;
        virtual void OnWrite(Network::OutputMemoryStream &stream) override;
    };
    
}
#endif /* Command_hpp */
