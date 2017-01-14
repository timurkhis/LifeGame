//
//  main.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//
#include <string>
#include <iostream>
#include "Window.hpp"
#include "Presets.hpp"
#include "GameField.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Network.h"

struct {
    Geometry::Vector field = Geometry::Vector(1000, 1000);
    Geometry::Vector window = Geometry::Vector(800, 600);
    std::shared_ptr<Network::SocketAddress> address = std::make_shared<Network::SocketAddress>();
    std::string presetPath = "presets.txt";
    std::string label = "LifeGame";
    bool server = true;
} args;

void Parse(int argc, char **argv);

int main(int argc, char **argv) {
    Parse(argc, argv);
    if (args.server) {
        static Server server(args.field);
        args.address = server.Address();
    }
    Client client(args.address);
    GameField gameField(&client);
    Presets presets(args.presetPath);
    Window &instance = Window::Instance();
    instance.Init(&gameField, &presets);
    instance.MainLoop(argc, argv, args.label, args.window);
    return 0;
}

void Parse(int argc, char **argv) {
    
}
