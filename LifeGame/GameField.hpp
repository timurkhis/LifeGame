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
    std::unordered_set<Geometry::Vector> units;
    const class Client *client;
    Geometry::Vector size;
    
public:
    explicit GameField(const class Client *client);
    void ClampVector(Geometry::Vector &vec) const;
    Geometry::Vector GetSize() const { return size; }
    const std::unordered_set<Geometry::Vector> *GetUnits() const { return &units; }
    void AddUnit(Geometry::Vector unit);
    void ProcessUnits();
    
private:
    void ProcessUnit(Geometry::Vector unit, std::unordered_map<Geometry::Vector, int> &processCells);
};

#endif /* GameField_hpp */
