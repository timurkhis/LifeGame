//
//  GameField.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 17.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Presets.hpp"
#include "GameField.hpp"
#include "Window.hpp"
#include "Messenger.hpp"

using namespace Geometry;

GameField::GameField(Messenger *messenger) : messenger(messenger), turn(false), player(-1) {
    units = std::make_shared<std::unordered_set<Vector>>();
    messenger->Init(this);
}

void GameField::AddUnit(Vector unit) {
    if (turn) return;
    auto inserted = units->insert(unit);
    if (inserted.second) {
        messenger->AddUnit(unit);
    }
}

void GameField::ClampVector(Vector &vec) const {
    vec.x %= size.x;
    vec.y %= size.y;
    if (vec.x < 0) vec.x = size.x + vec.x;
    if (vec.y < 0) vec.y = size.y + vec.y;
}

void GameField::ProcessUnits() {
    std::unordered_map<Vector, int> processCells;
    for (const auto &unit : *units) {
        ProcessUnit(unit, processCells);
    }
    for (const auto &cell : processCells) {
        if (cell.second == 3 && units->find(cell.first) == units->end()) {
            units->insert(cell.first);
        } else if ((cell.second < 2 || cell.second > 3) && units->find(cell.first) != units->end()) {
            units->erase(cell.first);
        }
    }
}

void GameField::Turn() {
    if (turn) return;
    turn = true;
    messenger->Turn();
}

void GameField::Update() {
    messenger->Update();
}

void GameField::ProcessUnit(Vector unit, std::unordered_map<Vector, int> &processCells) {
    processCells.insert(std::make_pair(unit, 0));
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) continue;
            Vector pos = unit + Vector(x, y);
            ClampVector(pos);
            const auto iter = processCells.insert(std::make_pair(pos, 1));
            if (!iter.second) {
                iter.first->second++;
            }
        }
    }
}
