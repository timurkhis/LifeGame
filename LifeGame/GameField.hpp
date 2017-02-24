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
    friend class Peer;
    class Peer *peer;
    std::shared_ptr<std::unordered_set<Unit>> units;
    Geometry::Vector size;
    int player;
    unsigned turnTime;
    bool turn;
    
public:
    explicit GameField();
    explicit GameField(Geometry::Vector size, unsigned turnTime, int player);
    
    int Player() const { return player; }
    unsigned TurnTime() const { return turnTime; }
    void ClampVector(Geometry::Vector &vec) const;
    Geometry::Vector GetSize() const { return size; }
    const std::shared_ptr<std::unordered_set<Unit>> GetUnits() const { return units; }
    void AddUnit(Geometry::Vector unit);
    void Turn();
    void Update();
    void ProcessUnits();
    void Destroy();
    
private:
    void ProcessUnit(const Unit &unit, std::unordered_map<Unit, int> &processCells);
    bool AddUnit(Geometry::Vector unit, int id);
};

#endif /* GameField_hpp */
