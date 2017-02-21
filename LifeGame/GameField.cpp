//
//  GameField.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 17.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <cstdlib>
#include "GameField.hpp"
#include "Peer.hpp"

using namespace Geometry;

GameField::GameField() : GameField(Vector(), 0, -1) {}

GameField::GameField(Geometry::Vector size, unsigned turnTime, int player) :
    turn(false),
    player(player),
    turnTime(turnTime),
    size(size),
    units(new std::unordered_set<Unit>()) {}

void GameField::AddUnit(Vector unit) {
    if (turn) return;
    auto inserted = units->emplace(player, unit);
    if (inserted.second) {
        peer->AddUnit(unit);
    }
}

void GameField::ClampVector(Vector &vec) const {
    vec.x %= size.x;
    vec.y %= size.y;
    if (vec.x < 0) vec.x = size.x + vec.x;
    if (vec.y < 0) vec.y = size.y + vec.y;
}

void GameField::ProcessUnits() {
    std::unordered_map<Unit, int> processCells;
    for (const auto &unit : *units) {
        ProcessUnit(unit, processCells);
    }
    for (const auto &cell : processCells) {
        if (cell.second == 3) {
            units->emplace(cell.first);
        } else if (cell.second < 2 || cell.second > 3) {
            auto contains = units->find(cell.first);
            if (contains != units->end()) {
                units->erase(contains);
            }
        }
    }
}

void GameField::ProcessUnit(const Unit &unit, std::unordered_map<Unit, int> &processCells) {
    processCells.emplace(unit, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) continue;
            Vector pos = unit.position + Vector(x, y);
            ClampVector(pos);
            Unit newUnit(unit.player, pos);
            const auto iter = processCells.emplace(newUnit, 1);
            if (!iter.second) {
                if (iter.first->second == 0) {
                    processCells.erase(iter.first);
                    processCells.emplace(newUnit, 1);
                } else if (iter.first->first.player == unit.player) {
                    iter.first->second++;
                } else {
                    iter.first->second--;
                }
            }
        }
    }
}

void GameField::Turn() {
    if (turn) return;
    turn = true;
    peer->Turn();
}

void GameField::Update() {
    peer->Update();
}

void GameField::Destroy() {
    peer->Destroy();
    std::exit(EXIT_SUCCESS);
}
