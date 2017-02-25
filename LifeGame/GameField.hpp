//
//  GameField.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 17.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef GameField_hpp
#define GameField_hpp

#include <unordered_map>
#include <unordered_set>
#include "Geometry.h"

struct Unit {
    int player;
    Geometry::Vector position;
    
    Unit(int player, Geometry::Vector position) : player(player), position(position) {}
    
    friend bool operator == (const Unit &lhs, const Unit &rhs) {
        return lhs.position == rhs.position;
    }
};

template <>
struct std::hash<Unit> {
    size_t operator () (const Unit &unit) const {
        return std::hash<Geometry::Vector>()(unit.position);
    }
};

class GameField {
    class Peer *peer;
    std::shared_ptr<std::unordered_set<Unit>> units;
    Geometry::Vector size;
    int player;
    unsigned turnTime;
    
public:
    explicit GameField();
    explicit GameField(Geometry::Vector size, unsigned turnTime, int player);
    
    int Player() const { return player; }
    unsigned TurnTime() const { return turnTime; }
    Geometry::Vector GetSize() const { return size; }
    bool IsInitialized() const { return player >= 0 && size.x > 0 && size.y > 0; }
    
    void SetPeer(Peer *peer) { this->peer = peer; }
    void SetTurnTime(unsigned turnTime) { this->turnTime = turnTime; }
    void SetSize(Geometry::Vector size) { this->size = size; }
    void SetPlayer(int player) { this->player = player; }
    
    const std::shared_ptr<std::unordered_set<Unit>> GetUnits() const { return units; }
    void ClampVector(Geometry::Vector &vec) const;
    void AddUnit(Geometry::Vector unit);
    bool AddUnit(Geometry::Vector unit, int id);
    
    void Turn();
    void Update();
    void ProcessUnits();
    void Destroy();
    
private:
    void ProcessUnit(const Unit &unit, std::unordered_map<Unit, int> &processCells);
    bool IsGameStopped() const;
};

#endif /* GameField_hpp */
