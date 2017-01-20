//
//  Reporter.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 11.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Reporter_hpp
#define Reporter_hpp

#include <stdexcept>

namespace Network {
    
    class NetworkException : public std::runtime_error {
    public:
        NetworkException(const std::string &message) : std::runtime_error(message) {}
    };
    
    class Reporter {
    public:
        static void Report(const char *report);
    };
    
}

#endif /* Reporter_hpp */
