//
//  Args.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Args_hpp
#define Args_hpp

#include "Vector.hpp"
#include "SocketAddress.hpp"

class Args {
    static Geometry::Vector field;
    static Geometry::Vector window;
    static Network::SocketAddress address;
    static std::string presetsPath;
    static std::string label;
    static bool server;
    
public:
    static void Parse(int argc, char **argv);
    
    static Geometry::Vector Field() { return field; }
    static Geometry::Vector Window() { return window; }
    static Network::SocketAddress &Address() { return address; }
    static const std::string &PresetPath() { return presetsPath; }
    static const std::string &Label() { return label; }
    static bool Server() { return server; }
};

#endif /* Args_hpp */
