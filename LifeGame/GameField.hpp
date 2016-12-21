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
#include "Vector.hpp"

class GameField {
    std::unordered_set<Vector> units;
    const Vector size;
    const class Window *window;
    std::shared_ptr<class Presets> presets;
    
public:
    GameField(class Window &window, Vector size, const char *presetsFile);
    void ClampVector(Vector &vec) const;
    Vector GetSize() const { return size; }
    const std::unordered_set<Vector> *GetUnits() const { return &units; }
    
private:
    void MouseHandle(Vector cell);
    void KeyboardHandle(unsigned char key);
    void ProcessUnits();
    void ProcessUnit(Vector unit, std::unordered_map<Vector, int> &processCells);
};

#endif /* GameField_hpp */
