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

class GameField {
    friend class Client;
    std::shared_ptr<std::unordered_set<Geometry::Vector>> units;
    class Client *client;
    Geometry::Vector size;
    int player;
    bool turn;
    
public:
    explicit GameField(class Client *client);
    
    int Player() const { return player; }
    void ClampVector(Geometry::Vector &vec) const;
    Geometry::Vector GetSize() const { return size; }
    const std::shared_ptr<std::unordered_set<Geometry::Vector>> GetUnits() const { return units; }
    void AddUnit(Geometry::Vector unit);
    void Turn();
    void ProcessUnits();
    
private:
    void ProcessUnit(Geometry::Vector unit, std::unordered_map<Geometry::Vector, int> &processCells);
};

#endif /* GameField_hpp */
