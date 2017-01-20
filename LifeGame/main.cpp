//
//  main.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include "Window.hpp"
#include "Presets.hpp"
#include "GameField.hpp"
#include "Server.hpp"
#include "Client.hpp"

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
    
    std::stringstream label;
    label << args.label << " " << *args.address;
    instance.MainLoop(argc, argv, label.str(), args.window);
    return 0;
}

void Parse(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (std::strcmp("field", argv[i]) == 0) {
            args.field.x = atoi(argv[++i]);
            args.field.y = atoi(argv[++i]);
        }
        if (std::strcmp("window", argv[i]) == 0) {
            args.window.x = atoi(argv[++i]);
            args.window.y = atoi(argv[++i]);
        }
        if (std::strcmp("server", argv[i]) == 0) {
            std::string address(argv[++i]);
            args.address = Network::SocketAddress::CreateIPv4(address);
            args.server = false;
        }
        if (std::strcmp("presets", argv[i]) == 0) {
            args.presetPath = argv[++i];
        }
    }
}
