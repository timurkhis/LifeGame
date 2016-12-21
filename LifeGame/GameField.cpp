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

GameField::GameField(Window &window, Vector size, const char *presetsFile) : size(size) {
    this->window = &window;
    window.InitField(this);
    MouseHandler mouseHandler = std::bind(&GameField::MouseHandle, this, std::placeholders::_1);
    window.AddMouseHandler(mouseHandler);
    KeyboardHandler keyboardHandler = std::bind(&GameField::KeyboardHandle, this, std::placeholders::_1);
    window.AddKeyboardHandler(keyboardHandler);
    presets = std::make_shared<Presets>(presetsFile);
}

void GameField::MouseHandle(Vector cell) {
    if (units.find(cell) != units.end()) return;
    units.insert(cell);
    window->Refresh();
}

void GameField::KeyboardHandle(unsigned char key) {
    if (key == Window::KeySpace) {
        ProcessUnits();
    } else if (key == Window::KeyEscape) {
        presets->SaveOnDisk();
    } else if (key >= '0' && key <= '9') {
        const Rect rect = window->GetSelectedCells();
        if (rect.IsZero()) return;
        std::vector<Vector> preset;
        for (const auto &unit : units) {
            if (rect.Contains(unit)) {
                preset.push_back(unit);
            }
        }
        presets->Save(key, &preset);
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
    for (const auto &unit : units) {
        ProcessUnit(unit, processCells);
    }
    for (const auto &cell : processCells) {
        if (cell.second == 3 && units.find(cell.first) == units.end()) {
            units.insert(cell.first);
        } else if ((cell.second < 2 || cell.second > 3) && units.find(cell.first) != units.end()) {
            units.erase(cell.first);
        }
    }
}

void GameField::ProcessUnit(Vector unit, std::unordered_map<Vector, int> &processCells) {
    if (processCells.find(unit) == processCells.end()) {
        processCells.insert(std::make_pair(unit, 0));
    }
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) continue;
            Vector pos = unit + Vector(x, y);
            ClampVector(pos);
            if (processCells.find(pos) == processCells.end()) {
                processCells.insert(std::make_pair(pos, 1));
            } else {
                processCells[pos]++;
            }
        }
    }
}
