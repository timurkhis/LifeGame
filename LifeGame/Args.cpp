//
//  Args.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <string>
#include "Args.hpp"

Geometry::Vector Args::field = Geometry::Vector(1000, 1000);
Geometry::Vector Args::window = Geometry::Vector(800, 600);
Network::SocketAddress Args::address = Network::SocketAddress();
std::string Args::presetsPath = std::string("presets.txt");
std::string Args::label = std::string("LifeGame");
bool Args::server = true;

void Args::Parse(int argc, char **argv) {
    
}
