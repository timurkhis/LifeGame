//
//  main.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//
#include <iostream>
#include "Window.hpp"
#include "Presets.hpp"
#include "GameField.hpp"
#include "Geometry.h"

using namespace Geometry;

int main(int argc, char **argv) {
    GameField gameField(Vector(1000, 1000));
    Presets presets("presets.txt");
    Window &instance = Window::Instance();
    instance.Init(&gameField, &presets);
    instance.MainLoop(argc, argv, "LifeGame", Vector(800, 600));
    return 0;
}

