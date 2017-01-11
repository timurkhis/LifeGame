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
#include "Args.hpp"
#include "Server.hpp"
#include "Client.hpp"

int main(int argc, char **argv) {
    Args::Parse(argc, argv);
    if (Args::Server()) {
        static Server server(Args::Field());
        Args::Address() = server.Address();
    }
    Client client(Args::Address());
    GameField gameField(&client);
    Presets presets(Args::PresetPath());
    Window &instance = Window::Instance();
    instance.Init(&gameField, &presets);
    instance.MainLoop(argc, argv, Args::Label(), Args::Window());
    return 0;
}

