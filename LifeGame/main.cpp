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

#include "nana/include/nana/gui.hpp"
#include "nana/include/nana/gui/widgets/label.hpp"
#include "nana/include/nana/gui/widgets/button.hpp"

struct {
    Geometry::Vector field = Geometry::Vector(1000, 1000);
    Geometry::Vector window = Geometry::Vector(800, 600);
	std::string address;
#if defined(_WIN32)
	std::string presetPath = "../../presets.txt";
#else
    std::string presetPath = "presets.txt";
#endif
    std::string label = "LifeGame";
    bool master = true;
    unsigned turnTime = 100;
    int players = 4;
} args;

void Parse(int argc, char **argv);

int main(int argc, char **argv) {

	using namespace nana;
	//Define a form.
	nana::form fm;
	//Define a label and display a text.
	nana::label lab{ fm, "Hello, <bold blue size=16>Nana C++ Library</>" };
	lab.format(true);
	//Define a button and answer the click event.
	nana::button btn{ fm, "Quit" };
	btn.events().click([&fm] {
		fm.close();
	});
	//Layout management
	fm.div("vert <><<><weight=80% text><>><><weight=24<><button><>><>");
	fm["text"] << lab;
	fm["button"] << btn;
	fm.collocate();
	//Show the form
	fm.show();
	//Start to event loop process, it blocks until the form is closed.
	exec();

  /*  Parse(argc, argv);
    std::shared_ptr<Peer> peer;
    std::shared_ptr<GameField> gameField;
    std::shared_ptr<Presets> presets = std::make_shared<Presets>(args.presetPath);
	
    if (args.master) {
        gameField = std::make_shared<GameField>(presets, args.field, args.turnTime, 0);
        peer = std::make_shared<Peer>(gameField, args.players);
        args.address = peer->Address();
    } else {
        gameField = std::make_shared<GameField>(presets);
        peer = std::make_shared<Peer>(gameField, args.address);
    }
    peer->Init();
    Window &instance = Window::Instance();
    instance.Init(gameField);
	args.label += std::string(args.master ? " Master " : " Slave ") + args.address;
    instance.MainLoop(argc, argv, args.label, args.window);*/
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
			args.address = std::string(argv[++i]);
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
