//
//  main.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Window.hpp"
#include "Presets.hpp"

int main(int argc, char **argv) {
    Window &instance = Window::Instance();
    GameField gameField(instance, Vector(100, 100));
    Presets presets("presets.txt");
    std::vector<Vector> newPreset { Vector(100, 100), Vector(59, 50), Vector(0, 0) };
    presets.Save('1', &newPreset);
    instance.MainLoop(argc, argv, "LifeGame", Vector(800, 600));
    return 0;
}

