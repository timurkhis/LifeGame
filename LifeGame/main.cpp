//
//  main.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Window.hpp"

int main(int argc, char **argv) {
    Window &instance = Window::Instance();
    std::vector<Vector> units;
    MouseHandler clickHandler = [&units, &instance](Vector cell) {
        units.push_back(cell);
        instance.Refresh();
    };
    instance.AddMouseHandler(clickHandler);
    instance.InitField(&units, Vector(100, 100));
    instance.MainLoop(argc, argv, "LifeGame", 800, 600);
    return 0;
}

