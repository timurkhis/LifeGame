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

using namespace Geometry;

class GameField {
    std::unordered_set<Vector> units;
    const Vector size;
    
public:
    explicit GameField(Vector size);
    void ClampVector(Vector &vec) const;
    Vector GetSize() const { return size; }
    const std::unordered_set<Vector> *GetUnits() const { return &units; }
    void AddUnit(Vector unit);
    void ProcessUnits();
    
private:
    void ProcessUnit(Vector unit, std::unordered_map<Vector, int> &processCells);
};

#endif /* GameField_hpp */
