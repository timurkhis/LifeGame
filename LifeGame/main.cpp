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
    GameField gameField(instance, Vector(100, 100), "presets.txt");
    instance.MainLoop(argc, argv, "LifeGame", Vector(800, 600));
    return 0;
}

