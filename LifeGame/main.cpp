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
#include "Peer.hpp"

struct {
    Geometry::Vector field = Geometry::Vector(1000, 1000);
    Geometry::Vector window = Geometry::Vector(800, 600);
    std::shared_ptr<Network::SocketAddress> address;
    std::string presetPath = "presets.txt";
    std::string label = "LifeGame";
    bool master = true;
    unsigned turnTime = 100;
    int players = 4;
} args;

void Parse(int argc, char **argv);

int main(int argc, char **argv) {
    Parse(argc, argv);
    std::shared_ptr<Peer> peer;
    std::shared_ptr<GameField> gameField;
    std::shared_ptr<Presets> presets = std::make_shared<Presets>(args.presetPath);
    
    std::stringstream label;
    if (args.master) {
        gameField = std::make_shared<GameField>(presets, args.field, args.turnTime, 0);
        peer = std::make_shared<Peer>(gameField, args.players);
        args.address = peer->Address();
        label << "Master ";
    } else {
        gameField = std::make_shared<GameField>(presets);
        peer = std::make_shared<Peer>(gameField, args.address);
        label << "Slave ";
    }
    peer->Init();
    Window &instance = Window::Instance();
    instance.Init(gameField);
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
            args.master = false;
        }
        if (std::strcmp("presets", argv[i]) == 0) {
            args.presetPath = argv[++i];
        }
        if (std::strcmp("turn", argv[i]) == 0) {
            unsigned turnTime = atoi(argv[++i]);
            if (turnTime > 10) {
                turnTime = 10;
            }
            args.turnTime = turnTime > 0 ? 1000 / turnTime : 0;
        }
        if (std::strcmp("players", argv[i]) == 0) {
            int players = atoi(argv[++i]);
            if (players > 0) {
                if (players > 8) {
                    players = 8;
                }
                args.players = players;
            }
        }
    }
}
