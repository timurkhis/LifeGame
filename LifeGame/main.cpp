//
//  main.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Window.hpp"
#include "Presets.hpp"
#include "GameField.hpp"

int main(int argc, char **argv) {
    Window &instance = Window::Instance();
    GameField gameField(Vector(100, 100));
    Presets presets("presets.txt");
    instance.Init(&gameField, &presets);
    instance.MainLoop(argc, argv, "LifeGame", Vector(800, 600));
    return 0;
}

